# Procedural Terrain Generator
A real-time procedural terrain rendering engine built with OpenGL. It leverages the power of the GPU by using Compute Shaders for noise generation and Tessellation Shaders.

---

## Features 💥

* **Fully GPU-Driven Pipeline**: The entire terrain is generated and rendered on the GPU, minimizing CPU overhead, taking ~5-10ms to generate a map
* **Procedural Generation**: Utilizes a compute shader to generate heightmaps with perlin noise, and variations (domain-warp, fractal brownian motion)
* **Dynamic Level of Detail (LOD)**: Employs Hardware Tessellation Shaders to dynamically generate a high-resolution mesh from a simple base grid, allowing for vast and detailed landscapes with minimal performance impact.
* **Advanced Procedural Shading**: The fragment shader colors the terrain based on a combination of:
    * **Altitude**: A smooth gradient blending between water, sand, grass, forest, rock, and snow.
    * **Slope**: Steeper surfaces are realistically rendered as exposed rock.
* **Atmospheric Effects**: Includes toggleable fog and atmospheric scattering to enhance realism and depth perception.
* **Particle-Based Hydraulic Erosion**: Implements the particle-based hydraulic erosion method by *Hans Theobald Beyer (2015)*
* **Real-time Shadows**: Implemented using shadow-mapping

---

## Results ⛰️
| Screenshot           | Preview                                                         |
| -------------------- | --------------------------------------------------------------- |
| **Terrain Sample 1** | ![Terrain Sample 1](Terrain%20Generation/Media/Screenshot%202025-11-01%20185701.png)     |
| **Terrain Sample 2** | ![Terrain Sample 2](Terrain%20Generation/Media/Screenshot%202025-11-01%20204423.png) |
| **Perlin-Based Generation** | ![Perlin-Based Generation](Terrain%20Generation/Media/snow_before.png) |
| **After Erosion** | ![After Erosion](Terrain%20Generation/Media/snow_after.png) |

---

## Improvements!! 🔧

* **Hydraulic Erosion**: Exploring and implementing a hydraulic erosion simulation to create more realistic riverbeds and terrain.
* **Texture Mapping**: Blend detail textures (rock, grass, sand) in the fragment shader instead of solid colors for higher fidelity.
* **Realistic Rendering**: using different techniques to generate a realistic terrain (shadowmaps, skybox, PCG trees...)
* **Bugs**: need to deal with the directionality of perlin noise, which has been causing weird artifacts
* **END GOAL**: Make the terrain a portable api that generates an interactble mesh, which can be used in a world-simulation project (future!!)

  ---




