tracer
 - height map
 - volumetric fog
 - soft reflections
 - relativistic effects

lighting
 - path tracer materials

textures
 - EWA texture filtering

misc
 - update cl headers (cpp wrapper)
 - convert to using cl c++ wrapper
 - point light distance attenuation
 - check path tracers pixel averaging
 - run clang-modernize
 - move random gens to thread locals + remove local gens from everywhere
 - check all shared data usage between threads
 - modernize matrix class + check column-row ordering
 - add quaternion <-> matrix conversion stuff
 - add quaternion interpolation
 - add vector4 and update matrix to calculate correct stuff
 - change from doubles to float
 - add better clang format support to visual studio
 - add scripts to linux to run clang stuff
 - add spline curve class
 - add camera movement with splines
 - add support to for ispc compiler
 - add BVH -> QBVH conversion
 - implement QBVH travelsal and SIMD triangle intersect with ispc
 - implement non-recursive versions of raytracing and pathtracing algorithms (wavefront-style / multipass stuff)
 - integrate framebuffer to windowrunner and add film class that holds image buffers
 - implement opencl tracers with multipass/wavefront style
 - bvh on/off for editing and store original visible in scene
 - change tclap to boost program options and move settings to root
 - implement timer class
 - change globals variables to constexpr
 - fix atomic variable initializations
 - alias instead of typedef
 - use make_unique (check gcc support)
 - remove freetype headers from windows platform
 - add text coloring to logger output
 - replace private constructors with =deleted
 - check expection propagation from threads
 - rename default state -> tracer state
 - move file opening and console colors to SysUtils
 - fast file -> string in StringUtils
 - add scope exit to obj loader