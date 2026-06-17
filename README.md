# CubeLand

A 3D open-world voxel terrain built from scratch using OpenGL and C++. No game engine. No high-level framework. Every system — rendering, camera, lighting, collision, and interaction — is implemented manually using raw OpenGL APIs.

Developed as a semester project for **HCI & Computer Graphics (CSC-2079L)**.

## Overview

CubeLand is a first-person interactive 3D world where the player can explore a randomly generated block-based terrain, place and remove blocks, and build room structures. The project serves as a practical demonstration of core computer graphics concepts applied in a real-time interactive environment.

## Features

- Randomly generated world on every run — grass, dirt, and stone blocks
- Height variation — elevated terrain and hills
- First-person WASD movement with mouse look
- Block placing and breaking via mouse clicks
- DDA raycasting for precise block targeting
- Collision detection with wall sliding
- Randomly placed trees
- Place a pre-defined room structure (**H** key)
- Live XYZ coordinates on screen
- On-screen control reference panel
- **F2** key saves a screenshot as a BMP file
- Windowed or fullscreen support

## Controls

| Key / Input     | Action              |
|-----------------|---------------------|
| **W A S D**     | Move                |
| **Mouse**       | Look around         |
| **Left Click**  | Break block         |
| **Right Click** | Place block         |
| **H**           | Place a room        |
| **F2**          | Save screenshot     |
| **ESC**         | Quit                |

## Requirements

- Windows (tested on Windows 10/11)
- MSYS2 with MinGW64
- Required packages (install via pacman):

```bash
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-freeglut
```

## Build & Run

```bash
C:/msys64/mingw64/bin/g++.exe code.cpp -o code.exe \
  -LC:/msys64/mingw64/lib \
  -IC:/msys64/mingw64/include \
  -lglew32 -lglfw3 -lopengl32 -lgdi32 -lglu32 -lfreeglut

./code.exe
```

Alternatively, use the VS Code task if configured.

## Configuration

The following constants are defined at the top of `code.cpp` and can be easily adjusted:

| Define          | Description                          | Default |
|-----------------|--------------------------------------|---------|
| `WORLD_SIZE`    | Grid dimensions (square)             | 16      |
| `WORLD_SCALE`   | Block size multiplier                | 1.0     |
| `CAM_HEIGHT`    | Camera height above ground           | 1.5     |
| `PLAYER_SPEED`  | Movement speed                       | 5.0     |
| `MAX_TREES`     | Number of trees in the world         | 8       |

- Mouse sensitivity can be adjusted via the `sensitivity` variable.
- To customize the room structure, edit the `layout[]` character map in the `placeHome()` function:
  - `W` = wall
  - `D` = door gap
  - `.` = floor

## Computer Graphics Concepts Demonstrated

- Perspective and orthographic projection
- Model, View, and Projection (MVP) matrix stack
- Flat shading with per-face normals
- Phong lighting model (ambient + diffuse, directional light)
- DDA raycasting for block selection
- Depth buffering and polygon offset (z-fighting prevention)
- Alpha blending for HUD overlay
- 2D HUD rendering using orthographic projection
- Double buffering for smooth rendering
- Procedural terrain generation using seeded RNG

### Lighting Model

```cpp
// Phong equation (simplified)
I = Ia·ka + Id·kd·(N·L)

Ia = 0.3 (ambient)
Id = 1.0 (diffuse)
Light direction: (1.0, 2.0, 1.0)
```

## Known Limitations

- Terrain is based on a 2D heightmap — breaking blocks only affects the top of a column
- No texture mapping (blocks use flat colors only)
- No dynamic lighting or shadows
- World state is not saved between runs

