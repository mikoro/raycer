raytracer
 - height map
 - relativistic effects

pathtracer
 - pathtracer materials
 - direct light sampling
 - russian roulette

textures
 - mip map generation
 - ray differentials
 - EWA texture filtering

misc
 - convert to using opencl c++ wrapper
 - opencl work group sizes
 - add spline curve class
 - add camera movement with splines
 - add BVH -> QBVH conversion
 - implement QBVH travelsal and SIMD triangle intersect with ispc
 - change tclap to boost program options and move settings to root
 - remove freetype headers from windows platform
 - replace private constructors with =deleted
 - add scope exit to obj loader and consolerunner text color
 - replace align macro with alignas
 - check if camera dof needs jitter
 - fast preview with "dot product lighting"
 - replace mersenne twister with pcg
 - remove recursion from cpu pathtracer
 - implement info text panel + more statistics
 - move ray-scene intersection to scene
 - remove csg suppport
 - add image data to serialization
 - disable reinhard averaging when not in interactive mode
 - templetize image (float and double images)