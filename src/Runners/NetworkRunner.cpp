// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <thread>
#include <atomic>
#include <regex>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

#include "tinyformat/tinyformat.h"

#include "Runners/NetworkRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"

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

	std::cout << "\nClient is now waiting for servers\n";
	std::cout << "Press CTRL+C to cancel the wait and quit.\nPress Enter to finish the wait and continue.\n\n";

	std::cin.get();
	receiveBroadcastsInterrupted = true;
	receiveBroadcastsThread.join();

	if (interrupted)
		return;
}

void NetworkRunner::runServer()
{
	Settings& settings = App::getSettings();

	auto sendBroadcastsFunction = [&]()
	{
		sendBroadcasts();
	};

	std::thread sendBroadcastsThread(sendBroadcastsFunction);

	std::cout << tfm::format("\nServer is announcing its presence (UDP port %d)\n", (int)settings.network.broadcastPort);
	std::cout << tfm::format("Server is listening for jobs on %s:%d\n", localAddress.to_string(), settings.network.localPort);
	std::cout << "\nPress CTRL+C to close the server.\n\n";
	
	while (!interrupted)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	sendBroadcastsThread.join();
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
		endpoints.push_back(ip::udp::endpoint(ip::address_v4::broadcast(), (unsigned short)settings.network.broadcastPort));

		ip::address_v4 broadcastAddress = ip::address_v4::from_string(settings.network.broadcastAddress, error);

		if (!error && broadcastAddress != ip::address_v4::broadcast())
			endpoints.push_back(ip::udp::endpoint(broadcastAddress, (unsigned short)settings.network.broadcastPort));
		
		std::string message = tfm::format("Raycer Server 1.0.0\nAddress: %s\nPort: %d\n", localAddress.to_string(), settings.network.localPort);
		int counter = 0;

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

		socket.bind(ip::udp::endpoint(ip::udp::v4(), (unsigned short)settings.network.broadcastPort));
		socket.set_option(ip::udp::socket::reuse_address(true));
		socket.set_option(socket_base::broadcast(true));

		serverEndpoints.clear();

		std::regex addressRegex("^Address: (.+)$");
		std::regex portRegex("^Port: (.+)$");

		while (!interrupted && !receiveBroadcastsInterrupted)
		{
			boost::array<char, 128> buffer;
			ip::udp::endpoint ignoredEndpoint;
			size_t length = socket.receive_from(boost::asio::buffer(buffer), ignoredEndpoint);

			if (length > 0)
			{
				std::string message(buffer.data(), length);
				std::istringstream ss(message);
				std::string line, addressString;
				std::getline(ss, line);

				if (line == "Raycer Server 1.0.0")
				{
					std::smatch match;
					std::getline(ss, line);

					if (std::regex_match(line, match, addressRegex))
					{
						addressString = match[1];
						std::getline(ss, line);

						if (std::regex_match(line, match, portRegex))
						{
							std::istringstream portString(match[1]);
							int port;
							portString >> port;

							ip::tcp::endpoint serverEndpoint(ip::address_v4::from_string(addressString), (unsigned short)port);

							if (std::find(serverEndpoints.begin(), serverEndpoints.end(), serverEndpoint) == serverEndpoints.end())
							{
								serverEndpoints.push_back(serverEndpoint);
								std::cout << tfm::format("Server found (%d): %s\n", serverEndpoints.size(), serverEndpoint);
							}
						}
					}
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		socket.close();
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not receive broadcast messages: %s", ex.what());
	}
}

void NetworkRunner::receiveJobs()
{
	try
	{
		//Settings& settings = App::getSettings();

		//io_service io;
		//ip::tcp::socket socket(io, ip::tcp::v4());
		//socket.bind(ip::tcp::endpoint(localAddress, (unsigned short)settings.network.localPort));

	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not receive jobs: %s", ex.what());
	}
}

void NetworkRunner::sendJob()
{

}

ip::address_v4 NetworkRunner::getLocalAddress()
{
	io_service io;
	boost::system::error_code error;

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
