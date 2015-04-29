// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

int test_main(int argc, char** argv)
{
	return Catch::Session().run(argc, argv);
}
