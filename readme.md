# Raycer

A C++11/OpenMP/OpenCL raytracer with an interactive preview mode.

[Image gallery](https://www.flickr.com/photos/136293057@N06/albums/72157660998109840)
[Feature video](https://youtu.be/0t6a2wXFJkE)

* Author: [Mikko Ronkainen](http://mikkoronkainen.com)
* Website: [github.com/mikoro/raycer](https://github.com/mikoro/raycer)

![Screenshot](http://mikoro.github.io/images/raycer/readme-screenshot.jpg "Screenshot")

## Download

Download the latest version:

| Windows 64-bit                                                                                         | Mac OS X (10.9+)                                                                                       | Linux 64-bit (Ubuntu 15.10)                                                                                      |
|--------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------|
| [raycer-1.0.0-win.zip](https://github.com/mikoro/raycer/releases/download/v1.0.0/raycer-1.0.0-win.zip) | [raycer-1.0.0-mac.zip](https://github.com/mikoro/raycer/releases/download/v1.0.0/raycer-1.0.0-mac.zip) | [raycer-1.0.0-ubuntu15.zip](https://github.com/mikoro/raycer/releases/download/v1.0.0/raycer-1.0.0-ubuntu15.zip) |

**Note:** To run the program you have to install graphics drivers that have support for OpenCL. Otherwise the program will complain of missing OpenCL libraries. The recent graphics driver packages from Intel/Nvidia/AMD will include the necessary files. 

## Features

- Multiplatform modern C++11/14 + OpenMP + OpenCL + OpenGL
  - Runs on 64-bit Windows, Mac OS X and Linux
  - Multithreaded using OpenMP
  - GPU features are implemented with OpenCL
  - OpenGL used for interactive mode only (display raytracing results with textured quads)
- Interactive and non-interactive mode
  - Interactive mode allows moving in the scene and editing primitives
  - Non-interactive mode renders the scene to an image file with helpful progress indicators
  - In the interactive mode the scene can be rendered in a lower resolution and then scaled up with good quality resampling shaders for real-time performance
- Raytracing (Whitted-style recursive algorithm)
  - Mirror and glossy reflections
  - Single and glossy refractions with refractive indices
  - Fresnel term with reflection/refraction
  - Attenuation by transparent objects (Beer's law)
  - Ambient, directional, point and spot lights
    - Phong shading (ambient, diffuse and specular terms)
    - Ambient light occlusion (hemisphere sampling for occlusion)
    - Soft shadows with point and spot lights (area light sampling)
  - Simpler version (ray casting) implemented for GPUs (OpenCL)
    - Works on scenes composed of triangles only (3, 18 and 19)
- Pathtracing
  - Cosine distribution sampling for diffuse surfaces
  - Functionally almost identical version implemented for GPUs (OpenCL)
    - Works on scenes composed of triangles only (3, 18 and 19)
- Sampling
  - Multisampling for antialiasing
    - Multiple filter types: box, tent, bell, gaussian, mitchell and lanczos sinc
  - Time sampling for motion blur
    - Camera and all primitives can move independently
  - Camera sampling for depth of field
    - Focal distance and aperture size
  - Different sampling distributions (both procedural and pre-generated)
    - Regular, random, jittered, correlated multi-jitter and poisson disc
- Primitives
  - Plane
  - Sphere
  - Box (slab method)
  - Triangle (Möller-Trumbore algorithm)
  - Cylinder (2nd order polynomial)
  - Torus (4th order polynomial, Durand-Kerner algorithm root solver)
  - Blinn blobs / metaballs (regula falsi method root finder)
  - Constructive solid geometry (CSG)
    - All primitives can be used
    - Supports union, difference and intersection operations
  - Bounding volume hierarchy (BVH)
    - Can contain any primitives (also other BVHs)
    - Surface area heuristics (SAH) used for building
    - Flat construction and usage (no recursion)
  - Instance
    - Can duplicate any other primitive and apply different transformation and material
  - Primitive group
    - Used to combine multiple primitives (e.g. a model composed of triangles) to be used with an instance
- Camera projections
  - Orthogonal, perspective and fish eye projections supported
- Normal mapping
  - Tangent space calculated for triangles
  - Bump map images
  - Normal map images
  - Gradient normal mapping (normals are perturbed by a gradient calculated from a procedural 3D texture)
- Texture mapping
  - Sphere and triangle have UV mapping
  - HDR texture support
  - Bilinear filtering
- Procedural textures
  - Both 2D and 3D procedural generation
  - Solid color, gradient color and checkerboard textures
  - Perlin noise texture with fractional brownian motion (FBM)
  - Wood, marble and fire textures (based on perlin noise)
  - Cell noise texture with different distance metrics and combination operators
  - Voronoi color texture with different distance metrics
  - Atmosphere texture
- Simple and volumetric fog
  - Simple fog is based on distance and can be dispersed according to height
  - Volumetric fog is ray marched with single scattering
- Tone mapping
  - Linear, simple and Reinhard tone mappers
  - Gamma correction
  - Exposure and key adjustment
  - Delayed exposure adjustment with the Reinhard tone mapper (interactive mode)
- Image file format support
  - Loading textures from (and saving output to) PNG, TGA, BMP and HDR (Radiance RGBE) image file formats
- Model file format support
  - OBJ format
    - All valid OBJ files are supported (groups, relative indices etc.)
    - MTL files are supported (with added material properties)
  - PLY format
    - Both ascii and binary formats supported
    - No material support (single material for the whole model)
- Distributed network renderer
  - The program can be started in server mode in which it will announce its presence and wait for rendering jobs over the network
  - In client mode the program will listen for available servers and then divide the scene to be partially rendered by each server
  - Server discovery is done by UDP broadcasts and scene data and rendering results are sent over using TCP
- Serialization
  - All scene data can be serialized to XML, JSON or binary
  - Externally loaded models and generated BVHs can also be serialized for faster scene loading
- Math library
  - A simple and minimal math library implementation

## Instructions

Running the program will open the first test scene in an interactive windowed mode. Use WASD to move and left mouse button to look around. Cycle through the test scenes with F2/F3.

Edit the settings.ini file to further configure the program. Some options can be overridden with command line switches.

### Controls

For the interactive mode:

| Key                     | Action                                                                                |
|-------------------------|---------------------------------------------------------------------------------------|
| **W/A/S/D**             | Move around (+ Q/E for up/down)                                                       |
| **Mouse left**          | Look around                                                                           |
| **Mouse right**         | Select primitive for editing                                                          |
| **Mouse wheel**         | Scale selected primitive                                                              |
| **Mouse middle**        | Change translation/rotation axis when editing                                         |
| **Space**               | Rotate selected primitive                                                             |
| **Ctrl**                | Move slowly                                                                           |
| **Shift**               | Move fast                                                                             |
| **Alt**                 | Move even faster                                                                      |
| **Insert/Delete**       | Fine tune movement speed                                                              |
| **Home**                | Cycle between different camera projections                                            |
| **Page Up/Down**        | Increase/decrease FOV                                                                 |
| **Ctrl + Page Up/Down** | Increase/decrease exposure or key                                                     |
| **F1**                  | Toggle info text on/off                                                               |
| **F2**                  | Select next test scene                                                                |
| **F3**                  | Select previous test scene                                                            |
| **F4**                  | Toggle between raytracer and pathtracer                                               |
| **F5**                  | Cycle between different tone mappers                                                  |
| **F6**                  | Toggle depth visualization on/off                                                     |
| **F7**                  | Save current scene to XML file (Ctrl + F7 to JSON, Ctrl + Shift + F7 to binary)       |
| **F8**                  | Render current scene externally                                                       |
| **F10**                 | Decrease internal rendering resolution                                                |
| **F11**                 | Increase internal rendering resolution                                                |
| **F12**                 | Take screenshot                                                                       |
| **R**                   | Reset camera                                                                          |
| **N**                   | Toggle normal mapping on/off                                                          |
| **B**                   | Rebuild BVH                                                                           |

## References

Some books and websites I found helpful while developing the program:

- [Computer Graphics: Principles and Practice](http://www.amazon.com/Computer-Graphics-Principles-Practice-Edition/dp/0321399528)
- [An Introduction to Ray Tracing](http://www.amazon.com/Introduction-Tracing-Kaufmann-Computer-Graphics/dp/0122861604)
- [Ray Tracing from the Ground Up](http://www.amazon.com/Ray-Tracing-Ground-Kevin-Suffern/dp/1568812728)
- [Realistic Ray Tracing](http://www.amazon.com/Realistic-Ray-Tracing-Second-Edition/dp/1568814615)
- [Advanced Global Illumination](http://www.amazon.com/Advanced-Global-Illumination-Second-Edition/dp/1568813074)
- [Realistic Image Synthesis Using Photon Mapping](http://www.amazon.com/Realistic-Image-Synthesis-Photon-Mapping/dp/1568811470)
- [Physically Based Rendering](http://www.amazon.com/Physically-Based-Rendering-Second-Edition/dp/0123750792)
- [High Dynamic Range Imaging](http://www.amazon.com/Dynamic-Range-Imaging-Second-Edition/dp/012374914X)
- [Scratchapixel](http://www.scratchapixel.com/)

## Test scene data sources

Websites where the test scene data was sourced from:

- [The Stanford 3D Scanning Repository](http://graphics.stanford.edu/data/3Dscanrep/)
- [McGuire Graphics Data](http://graphics.cs.williams.edu/data/meshes.xml)
- [sIBL Archive](http://www.hdrlabs.com/sibl/archive.html)
- [Bernhard Vogl - Light probes](http://dativ.at/lightprobes/)
- [CGSkies](https://www.cgskies.com/)

## License

    Raycer
    Copyright © 2015 Mikko Ronkainen
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
