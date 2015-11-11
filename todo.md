tracer
 - height map
 - volumetric fog
 - relativistic effects

lighting
 - path tracer materials

textures
 - EWA texture filtering

misc
 - convert to using cl c++ wrapper
 - check all shared data usage between threads
 - add spline curve class
 - add camera movement with splines
 - add support to for ispc compiler
 - add BVH -> QBVH conversion
 - implement QBVH travelsal and SIMD triangle intersect with ispc
 - implement non-recursive versions of raytracing and pathtracing algorithms (wavefront-style / multipass stuff)
 - implement opencl tracers with multipass/wavefront style
 - change tclap to boost program options and move settings to root
 - remove freetype headers from windows platform
 - replace private constructors with =deleted
 - add scope exit to obj loader and consolerunner text color
 - replace align macro with alignas
 - sizeof(Color) -> colorf, check networkrunner image stuff
 - check if camera dof needs jitter
 - fast preview with "dot product lighting"
 - replace pow with fastPow