## Windows

- Install boost headers and binaries (http://sourceforge.net/projects/boost/files/boost-binaries/1.57.0/).
- Adjust solution include paths to point to the boost libraries.
- Compile with MSVC 12.0 (2013) or ICC 15.0 using VS 2013.
- Install graphics drivers that include OpenCL DLLs.

### Boost

If using ICC, the boost framework needs to be compiled separately. Probably a good idea to do it with MSVC too.

Download the sources and run the commands from a corresponding developer console (MSVC or ICC):

    bootstrap.bat
    .\b2 toolset=msvc-12.0 address-model=64
    or
    .\b2 toolset=intel-15.0-vc12 address-model=64

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
- Install gcc49 (macports) and set active
- Install boost (macports)
- Install GLFW (macports)
- Install Freetype (macports)
- Optional: Compile freetype-gl (gcc49)

Compile:

    export CXX=g++
    make -j4

## Framework versions

- boost 1.57.0
- Freetype 2.5.5
- GLFW 3.1.1
- freetype-gl (github master ?)
- CATCH v1.1 build 1 (master branch)
- cereal 1.1.1
- stb (github 947bdcd027)
- tclap 1.2.1
- tinyformat (github 3913307c28)
- utf8 2.3.4
