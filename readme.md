# Raycer

A raytracer and a pathtracer with an interactive real-time mode and GPGPU support (OpenCL). Work in progress.

* Author: [Mikko Ronkainen](http://mikkoronkainen.com)
* Website: [github.com/mikoro/raycer](https://github.com/mikoro/raycer)

## Download

[Scene data](https://dl.dropboxusercontent.com/u/41666920/raycer/raycer_data.zip)

Extract and merge the scene data zip to the existing data folder.

No prebuilt binaries available at the moment.

## Instructions

Edit the settings.ini file to configure the program. Some options can be overridden with command line.

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
| **R**                   | Reset camera                                                                          |
| **N**                   | Toggle normal mapping on/off                                                          |
| **B**                   | Rebuild BVH                                                                           |

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
