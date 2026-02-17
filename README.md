# C Raytracer

![Render Output](assets/FS12.png)

## Overview
A CPU-based raytracer written in C. This project implements a 3D graphics pipeline from scratch, including custom vector mathematics, memory management, and file parsing.

## Features
* **Primitives:** Spheres.
* **Lighting:** Diffuse shading with point lights.
* **Shadows:** Hard shadows with intensity attenuation.
* **Anti-Aliasing:** 3x3 Grid Supersampling.
* **Math:** Custom 3D vector library implemented from scratch.
* **Memory Management:** Dynamic array resizing (`realloc`) for scene object management.

## Usage
The project uses a Makefile to manage three build stages. 

### Building
```bash
# creates all executables
make all
```
In particular generates the executable FS_assg, which includes the full graphics pipeline and anti-aliasing.

### Running
```bash
# Usage: ./FS_assg <input_scene_file> <output_image_file>
./FS_assg scene.txt output.ppm
```

### Example Input File
```text
640 480                 # image Width, image Height
2.0                     # view port height
5.0                     # view port focal length
0.0 20.0 0.0 500.0      # light source: x position, y position, z position, brightness
5                       # number of colors
0x97C4EB 0x88CC66 0xEB6E0A 0xEBBE0A 0xEB3EDA  # colors
0                       # background color index
4                       # number of spheres
0.0 -41.0 -18.0 40.0 1  # sphere: x position, y position, z position, radius, color index
0.4 1.0 -10.0 0.5 3 
0.6 -0.3 -10.2 0.3 4
-0.4 -0.1 -10.5 0.7 2
```



