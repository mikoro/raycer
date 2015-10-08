﻿// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include "stdafx.h"

#include "Runners/NetworkRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"
#include "Raytracing/Tracers/TracerState.h"
#include "Raytracing/Camera.h"
#include "Runners/ConsoleRunner.h"
#include "Math/Color.h"

using namespace Raycer;
using namespace boost::asio;

int NetworkRunner::run()
{
	Settings& settings = App::getSettings();

	interrupted = false;

	boost::system::error_code error;
	localAddress = ip::address_v4::from_string(settings.network.localAddress, error);

	if (error)
		localAddress = getLocalAddress();

	if (localAddress.is_unspecified())
		throw std::runtime_error("Could not find local IP address");

	if (settings.network.isClient)
		runClient();
	else if (settings.network.isServer)
		runServer();

	return 0;
}

void NetworkRunner::interrupt()
{
	interrupted = true;
}

void NetworkRunner::runClient()
{
	auto receiveBroadcastsFunction = [&]()
	{
		receiveBroadcasts();
	};

	std::thread receiveBroadcastsThread(receiveBroadcastsFunction);

	std::cout << "\nClient is now waiting for servers\n\n";
	std::cout << "Press CTRL+C to cancel the wait and quit\nPress Enter to finish the wait and continue\n\n";

	std::cin.get();
	receiveBroadcastsInterrupted = true;
	receiveBroadcastsThread.join();

	if (interrupted || serverEndpoints.size() == 0)
		return;

	std::cout << "Sending a job to servers...\n\n";
	sendJobs();

	std::cout << "\nWaiting for results...\n\n";
	receiveResults();
}

void NetworkRunner::runServer()
{
	Settings& settings = App::getSettings();

	auto sendBroadcastsFunction = [&]()
	{
		sendBroadcasts();
	};

	auto receiveJobsFunction = [&]()
	{
		receiveJobs();
	};

	auto handleJobsFunction = [&]()
	{
		handleJobs();
	};

	std::thread sendBroadcastsThread(sendBroadcastsFunction);
	std::thread receiveJobsThread(receiveJobsFunction);
	std::thread handleJobsThread(handleJobsFunction);

	std::cout << tfm::format("\nServer is announcing its presence (UDP port %d)\n", settings.network.broadcastPort);
	std::cout << tfm::format("Server is listening for jobs on %s:%d\n", localAddress.to_string(), settings.network.localPort);
	std::cout << "\nPress CTRL+C to close the server\n";

	while (!interrupted)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	sendBroadcastsThread.join();
	receiveJobsThread.join();
	handleJobsThread.join();
}

void NetworkRunner::sendBroadcasts()
{
	try
	{
		Settings& settings = App::getSettings();

		io_service io;
		boost::system::error_code error;
		ip::udp::socket socket(io, ip::udp::v4());

		socket.set_option(ip::udp::socket::reuse_address(true));
		socket.set_option(socket_base::broadcast(true));

		std::vector<ip::udp::endpoint> endpoints;
		endpoints.push_back(ip::udp::endpoint(ip::address_v4::broadcast(), static_cast<unsigned short>(settings.network.broadcastPort)));

		ip::address_v4 broadcastAddress = ip::address_v4::from_string(settings.network.broadcastAddress, error);

		if (!error && broadcastAddress != ip::address_v4::broadcast())
			endpoints.push_back(ip::udp::endpoint(broadcastAddress, static_cast<unsigned short>(settings.network.broadcastPort)));

		std::string message = tfm::format("Raycer Server 1.0.0\nAddress: %s\nPort: %d", localAddress.to_string(), settings.network.localPort);
		uint counter = 0;

		while (!interrupted)
		{
			if (++counter % 20 == 0)
			{
				for (auto& endpoint : endpoints)
					socket.send_to(boost::asio::buffer(message), endpoint);

				counter = 0;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		socket.close();
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not send broadcast messages: %s", ex.what());
	}
}

void NetworkRunner::receiveBroadcasts()
{
	try
	{
		Settings& settings = App::getSettings();

		io_service io;
		ip::udp::socket socket(io, ip::udp::v4());
		deadline_timer timer(io, boost::posix_time::milliseconds(100));

		socket.bind(ip::udp::endpoint(ip::udp::v4(), static_cast<unsigned short>(settings.network.broadcastPort)));
		socket.set_option(ip::udp::socket::reuse_address(true));
		socket.set_option(socket_base::broadcast(true));

		serverEndpoints.clear();

		boost::array<char, 128> receiveBuffer;
		ip::udp::endpoint ignoredEndpoint;
		bool timerHasRun = false;

		auto receiveHandler = [&](const boost::system::error_code& error, size_t bytes)
		{
			if (error || bytes == 0)
				return;

			std::string message(receiveBuffer.data(), bytes);
			std::smatch match;
			std::istringstream ss;

			if (!std::regex_match(message, match, std::regex("^Raycer Server 1.0.0\nAddress: (.+)\nPort: (.+)$")))
				return;

			uint port;
			ss.str(match[2]);
			ss >> port;

			ip::tcp::endpoint serverEndpoint(ip::address_v4::from_string(match[1]), static_cast<unsigned short>(port));

			if (std::find(serverEndpoints.begin(), serverEndpoints.end(), serverEndpoint) == serverEndpoints.end())
			{
				serverEndpoints.push_back(serverEndpoint);
				std::cout << tfm::format("Server found (%d): %s\n", serverEndpoints.size(), serverEndpoint);
			}
		};

		auto timerHandler = [&](const boost::system::error_code& error)
		{
			(void)error;
			timerHasRun = true;
		};

		timer.async_wait(timerHandler);
		socket.async_receive_from(boost::asio::buffer(receiveBuffer), ignoredEndpoint, receiveHandler);

		while (!interrupted && !receiveBroadcastsInterrupted)
		{
			io.run_one();

			if (timerHasRun)
			{
				timerHasRun = false;
				timer.async_wait(timerHandler);
			}
			else
				socket.async_receive_from(boost::asio::buffer(receiveBuffer), ignoredEndpoint, receiveHandler);
		}

		timer.cancel();
		socket.close();
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not receive broadcast messages: %s", ex.what());
	}
}

void NetworkRunner::sendJobs()
{
	try
	{
		Settings& settings = App::getSettings();

		std::string sceneString;

		if (settings.scene.enableTestScenes)
		{
			Scene scene = Scene::createTestScene(settings.scene.testSceneNumber);
			sceneString = scene.getJsonString();
		}
		else
		{
			std::ifstream sceneFile(settings.scene.fileName);

			if (!sceneFile.good())
				throw std::runtime_error("Could not open the scene file");

			std::stringstream ss;
			ss << sceneFile.rdbuf();
			sceneString = ss.str();
			sceneFile.close();
		}

		size_t serverCount = serverEndpoints.size();
		size_t width = settings.image.width;
		size_t height = settings.image.height;
		size_t totalPixelCount = width * height;
		size_t pixelsPerServer = totalPixelCount / serverCount;
		size_t pixelsForLastServer = pixelsPerServer + totalPixelCount % serverCount;

		io_service io;

		for (size_t i = 0; i < serverEndpoints.size(); ++i)
		{
			bool isLastServer = (i == serverEndpoints.size() - 1);
			size_t pixelOffset = i * pixelsPerServer;
			size_t pixelCount = isLastServer ? pixelsForLastServer : pixelsPerServer;

			std::string message = tfm::format("Raycer 1.0.0\nAddress: %s\nPort: %d\nWidth: %d\nHeight: %d\nPixelOffset: %d\nPixelCount: %d\n\n%s", localAddress.to_string(), settings.network.localPort, width, height, pixelOffset, pixelCount, sceneString);

			ip::tcp::socket socket(io);
			socket.connect(serverEndpoints.at(i));
			boost::asio::write(socket, boost::asio::buffer(message));
			socket.close();
		}
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not send jobs: %s", ex.what());
	}
}

void NetworkRunner::receiveJobs()
{
	try
	{
		Settings& settings = App::getSettings();

		io_service io;
		ip::tcp::socket socket(io);
		ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(localAddress, static_cast<unsigned short>(settings.network.localPort)));
		deadline_timer timer(io, boost::posix_time::milliseconds(100));
		bool timerHasRun = false;

		auto acceptHandler = [&](const boost::system::error_code& error)
		{
			if (error)
				return;

			streambuf receiveBuffer;
			boost::system::error_code readError;

			while (!readError)
			{
				size_t bytes = socket.read_some(boost::asio::buffer(receiveBuffer.prepare(512)), readError);
				receiveBuffer.commit(bytes);
			}

			streambuf::const_buffers_type receiveBufferConst = receiveBuffer.data();
			std::string receivedString(buffers_begin(receiveBufferConst), buffers_begin(receiveBufferConst) + receiveBuffer.size());
			size_t headerEndIndex = receivedString.find("\n\n");

			if (headerEndIndex == std::string::npos)
				return;

			std::string headerString = receivedString.substr(0, headerEndIndex);
			std::string bodyString = receivedString.substr(headerEndIndex);
			std::smatch match;
			std::istringstream ss;

			if (!std::regex_match(headerString, match, std::regex("^Raycer 1.0.0\nAddress: (.+)\nPort: (.+)\nWidth: (.+)\nHeight: (.+)\nPixelOffset: (.+)\nPixelCount: (.+)$")))
				return;

			std::cout << "\nJob received!\n\n";

			ServerJob job;

			uint port;
			ss.str(match[2]);
			ss >> port;

			job.clientEndpoint = ip::tcp::endpoint(ip::address_v4::from_string(match[1]), static_cast<unsigned short>(port));

			size_t imageWidth;
			ss.clear();
			ss.str(match[3]);
			ss >> imageWidth;

			size_t imageHeight;
			ss.clear();
			ss.str(match[4]);
			ss >> imageHeight;

			ss.clear();
			ss.str(match[5]);
			ss >> job.pixelOffset;

			ss.clear();
			ss.str(match[6]);
			ss >> job.pixelCount;

			job.image.resize(imageWidth, imageHeight);
			job.scene = Scene::loadFromJsonString(bodyString);

			jobQueueMutex.lock();
			jobQueue.push(job);
			jobQueueMutex.unlock();
		};

		auto timerHandler = [&](const boost::system::error_code& error)
		{
			(void)error;
			timerHasRun = true;
		};

		timer.async_wait(timerHandler);
		acceptor.async_accept(socket, acceptHandler);

		while (!interrupted)
		{
			io.run_one();

			if (timerHasRun)
			{
				timerHasRun = false;
				timer.async_wait(timerHandler);
			}
			else
			{
				socket.close();
				acceptor.async_accept(socket, acceptHandler);
			}
		}

		acceptor.close();
		socket.close();
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not receive jobs: %s", ex.what());
	}
}

void NetworkRunner::handleJobs()
{
	try
	{
		while (!interrupted)
		{
			jobQueueMutex.lock();

			if (jobQueue.empty())
			{
				jobQueueMutex.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			ServerJob job = jobQueue.front();
			jobQueue.pop();
			jobQueueMutex.unlock();

			TracerState state;
			state.image = &job.image;
			state.scene = &job.scene;
			state.pixelOffset = job.pixelOffset;
			state.pixelCount = job.pixelCount;

			job.scene.initialize();
			job.scene.camera.setImagePlaneSize(state.image->getWidth(), state.image->getHeight());
			job.scene.camera.update(job.scene, 0.0);

			App::getConsoleRunner().run(state);

			std::cout << "Sending results back...\n\n";

			io_service io;
			std::string message = tfm::format("Raycer 1.0.0\nPixelOffset: %d\nPixelCount: %d\n\n", job.pixelOffset, job.pixelCount);

			for (uint i = 0; i < 60 && !interrupted; ++i)
			{
				try
				{
					ip::tcp::socket socket(io);
					socket.connect(job.clientEndpoint);
					boost::asio::write(socket, boost::asio::buffer(message));
					boost::asio::write(socket, boost::asio::buffer(&job.image.getPixelData()[0], job.image.getPixelData().size() * sizeof(Color)));
					socket.close();

					break;
				}
				catch (const std::exception& ex)
				{
					std::cout << tfm::format("Sending results failed: %s. Retrying in one second...\n\n", ex.what());
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}

			std::cout << "Finished! Waiting for more jobs...\n\n";
		}
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not handle jobs: %s", ex.what());
	}
}

namespace
{
	struct ImagePart
	{
		size_t pixelOffset = 0;
		size_t pixelCount = 0;
		std::vector<unsigned char> pixelData;
	};
}

void NetworkRunner::receiveResults()
{
	Settings& settings = App::getSettings();

	size_t imagePartCount = serverEndpoints.size();
	std::vector<ImagePart> imageParts;
	Image resultImage;

	try
	{
		resultImage.resize(settings.image.width, settings.image.height);

		io_service io;
		ip::tcp::socket socket(io);
		ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(localAddress, static_cast<unsigned short>(settings.network.localPort)));
		deadline_timer timer(io, boost::posix_time::milliseconds(100));
		bool timerHasRun = false;

		auto acceptHandler = [&](const boost::system::error_code& error)
		{
			if (error)
				return;

			streambuf receiveBuffer;
			boost::system::error_code readError;

			while (!readError)
			{
				size_t bytes = socket.read_some(boost::asio::buffer(receiveBuffer.prepare(1024 * 1024)), readError);
				receiveBuffer.commit(bytes);
			}

			std::ostream os(&receiveBuffer);
			os << '\0';

			streambuf::const_buffers_type receiveBufferConst = receiveBuffer.data();
			const char* receiveBufferPtr = boost::asio::buffer_cast<const char*>(receiveBufferConst);
			const char* headerEndPtr = strstr(receiveBufferPtr, "\n\n");
			const char* dataStartPtr = headerEndPtr + 2;

			if (headerEndPtr == nullptr)
				return;

			size_t headerSize = size_t(headerEndPtr - receiveBufferPtr);
			size_t dataSize = receiveBuffer.size() - headerSize - 2 - 1;

			std::string headerString(buffers_begin(receiveBufferConst), buffers_begin(receiveBufferConst) + headerSize);
			std::smatch match;
			std::istringstream ss;

			if (!std::regex_match(headerString, match, std::regex("^Raycer 1.0.0\nPixelOffset: (.+)\nPixelCount: (.+)$")))
				return;

			size_t pixelOffset, pixelCount;
			ss.str(match[1]);
			ss >> pixelOffset;
			ss.clear();
			ss.str(match[2]);
			ss >> pixelCount;

			assert(dataSize == pixelCount * sizeof(Color));

			ImagePart imagePart;
			imagePart.pixelOffset = pixelOffset;
			imagePart.pixelCount = pixelCount;
			imagePart.pixelData.resize(dataSize);

			memcpy(&imagePart.pixelData[0], dataStartPtr, dataSize);
			imageParts.push_back(imagePart);

			std::cout << tfm::format("Image part %d of %d received!\n", imageParts.size(), imagePartCount);
		};

		auto timerHandler = [&](const boost::system::error_code& error)
		{
			(void)error;
			timerHasRun = true;
		};

		timer.async_wait(timerHandler);
		acceptor.async_accept(socket, acceptHandler);

		while (!interrupted && imageParts.size() < imagePartCount)
		{
			io.run_one();

			if (timerHasRun)
			{
				timerHasRun = false;
				timer.async_wait(timerHandler);
			}
			else
			{
				socket.close();
				acceptor.async_accept(socket, acceptHandler);
			}
		}

		std::cout << std::endl;

		acceptor.close();
		socket.close();

		if (interrupted)
			return;

		for (ImagePart& part : imageParts)
			memcpy(reinterpret_cast<char*>(&resultImage.getPixelData()[0]) + part.pixelOffset * sizeof(Color), &part.pixelData[0], part.pixelCount * sizeof(Color));

		resultImage.save(settings.image.fileName);

		if (settings.image.autoView)
			ConsoleRunner::openImageExternally(settings.image.fileName);
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not receive results: %s", ex.what());
	}
}

ip::address_v4 NetworkRunner::getLocalAddress()
{
	io_service io;

	ip::tcp::resolver resolver(io);
	ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
	ip::tcp::resolver::iterator it = resolver.resolve(query);
	ip::tcp::resolver::iterator end;

	while (it != end)
	{
		ip::address ipAddress = it->endpoint().address();
		++it;

		if (ipAddress.is_v4())
			return ipAddress.to_v4();
	}

	return ip::address_v4();
}
