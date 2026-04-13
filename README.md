# 3D Rendering Project

A personal project focused on understanding and implementing core 3D rendering techniques from scratch in C++.

![image](/3d_render/examples/screenshot.png)

## Overview

This project is an exploration of how modern rendering pipelines work at a low level. Instead of relying on high-level engines, I am building key components manually to deepen my understanding of graphics programming.

## Goals

- Implement fundamental 3D rendering concepts from scratch
- Gain a deeper understanding of the graphics pipeline
- Experiment with performance and low-level optimizations in C++
- Continuously extend the renderer with new features over time

## Getting Started

##### Clone the Repository

`git clone https://github.com/Zekesy/3d-render-cpp.git`

##### Build the project

  ```bash
  cd 3d-render-cpp 
  mkdir build 
  cd build 
  cmake ..
  make 
  ```

##### Run the executable

  ```bash
  ./render3d_
  ```

## Features

- [x] Parsing .obj and .mtl files for meshes
- [x] SDL_Rendering window + ImGui settings window
- [x] Standard Render Pipeline: Triangulate Faces
  - Apply transform on vertices
  - Project vertices to 2D plane
  - Rasterize triangles (with depth and color buffers)
    - Frustrum and Back-face culling
    - Barycentric coordinates
- [x] WireFrame option to draw only the edges
- [x] User transformation control
- [x] Choosing from 3 default meshes and can load your own model
- [x] Basic Mesh simplification with QEM
  - Triangle Flip Check
  - Two-neighborhood connectivity check
  - Bounadry merge check
  - Cost function simple QEM

## Planned features

- [ ] Allowing parsing of different file formats (.stl, .gltf)
- [ ] Allow picking of different mesh simplification algorithms
- [ ] Lighting
- [ ] Textures and shading
- [ ] Shadows in the scene
- [ ] Optimizations where possible

## Tech Stack

- C++

## Resources

QEM:
<https://arxiv.org/pdf/2512.19959>
