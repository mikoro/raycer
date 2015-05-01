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

- Install both boost headers and libraries.
- Install both OpenCL headers and a graphics driver that has OpenCL support.
- Install Freetype.
- Install GLFW.
- Compile using the Makefile (either GCC or Clang).

## Mac

TBD
