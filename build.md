## Windows

- Install boost headers and binaries (http://sourceforge.net/projects/boost/files/boost-binaries/1.59.0/).
- Adjust solution include paths to point to the boost libraries.
- Compile with MSVC 14.0 (2015) or ICC 16.0 using VS 2015.
- Install graphics drivers that include OpenCL DLLs.

### Boost

If using ICC, the boost framework needs to be compiled separately. Probably a good idea to do it with MSVC too.

Download the sources and run the commands from a corresponding developer console (MSVC or ICC):

	msvc:
    bootstrap.bat
    b2 --build-type=minimal toolset=msvc-14.0 address-model=64 stage
    
	intel:
	compilervars.bat intel64 vs2015
	bootstrap.bat
    b2 --build-type=minimal toolset=intel address-model=64 stage

## Linux

- Install boost
- Install GLFW
- Install Freetype
- Install OpenCL headers + something that provides the libraries (e.g. beignet, intel/nvidia drivers)
- Optional: Compile freetype-gl

Compile and run:

    export CXX=<compiler>
    make -j4
    cd bin && ./raycer

The included freetype-gl library file is compiled against certain Freetype library version. It might be necessary to recompile the library and replace the file in *platform/linux/lib*.

The GLFW library maybe named as *glfw* or *glfw3*. If there is a linking error, try adding or removing the last number.

## Mac

- Install Xcode + Command Line Tools
- Install MacPorts
- Install boost (macports)
- Install glfw (macports)
- Install freetype (macports)
- Install libomp (macports)
- Optional: Compile freetype-gl

Can be compiled with the Apple/clang supplied with Xcode 7.0 (OS X 10.11).

Compile and run:

    make -j4
    cd bin && ./raycer

Build and run bundle:

    platform/mac/build_bundle.sh
    open Raycer.app

See remarks of the linux build.

## Framework versions

- boost 1.59.0
- Freetype 2.5.5
- GLFW 3.1.1
- freetype-gl (github f629c1d02101c1fa757f8b8d7fe91b4d1cf90cc0)
- CATCH v1.2.1
- cereal 1.1.2
- stb (github 947bdcd027)
- tclap 1.2.1
- tinyformat (github 3913307c28)
- utf8 2.3.4
