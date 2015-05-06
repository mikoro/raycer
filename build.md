## Windows

- Install boost headers and binaries (http://sourceforge.net/projects/boost/files/boost-binaries/1.58.0/).
- Adjust include paths to point to the boost libraries.
- Compile with MSVC 12.0 (2013) or ICC 15.0 using VS 2013.
- Install graphics drivers that include OpenCL DLLs.

### Boost

If using ICC, the boost libraries need to be compiled separately. Probably a good idea to do it with MSVC too.

Download the sources and run the commands from a corresponding developer console (MSVC or ICC):

    bootstrap.bat
    .\b2 toolset=msvc-12.0 address-model=64
    or
    .\b2 toolset=intel-15.0-vc12 address-model=64

## Linux

- Install boost.
- Install Freetype.
- Install GLFW.
- Install GLEW.
- Install OpenCL headers, libraries and a suitable driver for the compute device (CPU/GPU/etc). E.g. for Intel CPUs: https://software.intel.com/en-us/articles/opencl-drivers
- Compile using the Makefile.

## Mac

TBD

## Framework versions

- boost 1.57.0
- Freetype 2.5.5
- GLFW 3.1.1
- GLEW 1.12.0
- freetype-gl (github master ?)
- CATCH CATCH v1.1 build 1 (master branch)
- cereal 1.1.1
- stb (github 947bdcd027)
- tclap 1.2.1
- tinyformat (github 3913307c28)
- utf8 2.3.4