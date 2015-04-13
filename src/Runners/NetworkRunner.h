// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <vector>

#include <boost/asio.hpp>

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

		std::atomic<bool> interrupted;
		std::atomic<bool> receiveBroadcastsInterrupted;

		std::vector<boost::asio::ip::udp::endpoint> serverEndpoints;
	};
}
