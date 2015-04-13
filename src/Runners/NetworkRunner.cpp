// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <stdexcept>
#include <thread>
#include <atomic>

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

	if (settings.network.client)
		runClient();
	else if (settings.network.server)
		runServer();

	return 0;
}

void NetworkRunner::interrupt()
{
	interrupted = true;
}

void NetworkRunner::runClient()
{
	Log& log = App::getLog();

	log.logInfo("Client is now waiting for server broadcasts");

	auto receiveBroadcastsFunction = [&]()
	{
		receiveBroadcasts();
	};

	std::thread receiveBroadcastsThread(receiveBroadcastsFunction);

	std::cin.get();
	receiveBroadcastsInterrupted = true;
	receiveBroadcastsThread.join();

	if (interrupted)
		return;
}

void NetworkRunner::runServer()
{
	Log& log = App::getLog();

	auto sendBroadcastsFunction = [&]()
	{
		sendBroadcasts();
	};

	std::thread sendBroadcastsThread(sendBroadcastsFunction);

	log.logInfo("Server is now broadcasting and waiting for clients");

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
		//ip::udp::socket socket(io, ip::udp::v4());
		ip::udp::socket socket(io, ip::udp::endpoint(ip::address_v4::from_string("192.168.1.2"), (unsigned short)settings.network.broadcastPort));
		
		socket.set_option(ip::udp::socket::reuse_address(true));
		socket.set_option(socket_base::broadcast(true));

		std::vector<ip::udp::endpoint> endpoints;

		endpoints.push_back(ip::udp::endpoint(ip::address_v4::broadcast(), (unsigned short)settings.network.broadcastPort));

		boost::system::error_code error;
		ip::address_v4 ipAddress = ip::address_v4::from_string(settings.network.subnetBroadcast, error);

		if (!error)
			endpoints.push_back(ip::udp::endpoint(ipAddress, (unsigned short)settings.network.broadcastPort));
		
		std::string message = "Raycer 1.0.0";
		int counter = 0;

		std::cout << "\nPress CTRL+C to close the server.\n\n";

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
		//ip::udp::socket socket(io, ip::udp::endpoint(ip::udp::v4(), settings.network.broadcastPort));
		ip::udp::socket socket(io, ip::udp::endpoint(ip::address_v4::from_string("192.168.1.3"), (unsigned short)settings.network.broadcastPort));

		socket.set_option(ip::udp::socket::reuse_address(true));
		socket.set_option(socket_base::broadcast(true));

		serverEndpoints.clear();

		std::cout << "\nPress CTRL+C to cancel the wait and quit.\nPress Enter to finish the wait and continue.\n\n";

		while (!interrupted && !receiveBroadcastsInterrupted)
		{
			boost::array<char, 128> buffer;
			ip::udp::endpoint serverEndpoint;
			size_t length = socket.receive_from(boost::asio::buffer(buffer), serverEndpoint);

			if (length > 0)
			{
				std::string message(buffer.data(), length);

				if (message == "Raycer 1.0.0")
				{
					if (std::find(serverEndpoints.begin(), serverEndpoints.end(), serverEndpoint) == serverEndpoints.end())
					{
						serverEndpoints.push_back(serverEndpoint);
						std::cout << tfm::format("Server found (%d): %s\n", serverEndpoints.size(), serverEndpoint);
					}
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		socket.close();
		std::cout << std::endl;
	}
	catch (const std::exception& ex)
	{
		App::getLog().logError("Could not receive broadcast messages: %s", ex.what());
	}
}
