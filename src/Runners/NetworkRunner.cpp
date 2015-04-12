// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <thread>
#include <atomic>

#include <boost/asio.hpp>

#ifdef WIN32
#include <windows.h>
#endif

#include "tinyformat/tinyformat.h"

#include "Runners/NetworkRunner.h"
#include "App.h"
#include "Utils/Log.h"
#include "Utils/Settings.h"

using namespace Raycer;

int NetworkRunner::run()
{
	boost::asio::io_service io;

	return 0;
}
