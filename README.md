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

---

## Improvements!! 🔧

* **Hydraulic Erosion**: Exploring and implementing a hydraulic erosion simulation to create more realistic riverbeds and terrain.
* **Texture Mapping**: Blend detail textures (rock, grass, sand) in the fragment shader instead of solid colors for higher fidelity.
* **Realistic Rendering**: using different techniques to generate a realistic terrain (shadowmaps, skybox, PCG trees...)
* **END GOAL**: Make the terrain a portable api that generates an interactble mesh, which can be used in a world-simulation project (future!!)
