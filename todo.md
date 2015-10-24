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
 - convert to using cl c++ wrapper
 - point light distance attenuation
 - check path tracers pixel averaging
 - run clang-modernize
 - move random gens to thread locals + remove local gens from everywhere
 - check all shared data usage between threads
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
 - use make_unique (check gcc support)
 - remove freetype headers from windows platform
 - replace private constructors with =deleted
 - rename default state -> tracer state
 - add scope exit to obj loader and consolerunner text color
 - replace align macro with alignas
 - sizeof(Color) -> colorf, check networkrunner image stuff