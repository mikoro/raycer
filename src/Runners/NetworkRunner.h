// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#pragma once

#include <atomic>
#include <vector>
#include <queue>
#include <mutex>

#include <boost/asio.hpp>

#include "Raytracing/Scene.h"
#include "Rendering/Image.h"

namespace ba = boost::asio;

namespace Raycer
{
	struct ServerJob
	{
		ba::ip::tcp::endpoint clientEndpoint;

		Scene scene;
		Image image;

		int sceneWidth;
		int sceneHeight;
		int pixelOffset;
		int pixelCount;
	};

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
		void sendJobs();
		void receiveJobs();
		void handleJobs();
		void receiveResults();

		ba::ip::address_v4 getLocalAddress();

		std::atomic<bool> interrupted;
		std::atomic<bool> receiveBroadcastsInterrupted;
		ba::ip::address_v4 localAddress;
		std::vector<ba::ip::tcp::endpoint> serverEndpoints;
		std::queue<ServerJob> jobQueue;
		std::mutex jobQueueMutex;
	};
}
