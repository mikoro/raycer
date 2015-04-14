// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <vector>

#include <boost/asio.hpp>

namespace ba = boost::asio;

namespace Raycer
{
	class NetworkRunner
	{
	public:

		int run();
		void interrupt();

	private:

		void runClient();
		void runServer();
		void sendBroadcasts();
		void receiveBroadcasts();
		void receiveJobs();
		void sendJob();

		ba::ip::address_v4 getLocalAddress();

		std::atomic<bool> interrupted;
		std::atomic<bool> receiveBroadcastsInterrupted;
		std::vector<ba::ip::tcp::endpoint> serverEndpoints;
		ba::ip::address_v4 localAddress;
	};
}
