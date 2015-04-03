// Copyright © 2015 Mikko Ronkainen <firstname@mikkoronkainen.com>
// License: MIT, see the LICENSE file.

#include <tclap/CmdLine.h>

#include "App/App.h"

using namespace Raycer;

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	return App().run();
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;

	return main(0, nullptr);
}
#endif
