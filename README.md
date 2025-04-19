# 2D Minecraft
Simple 2D Minecraft clone written in C++ with SDL2 and glew.

### Current feature's
* 2D chunks (thus infinit x and y generation)
* Perlin Noise chunk generation
* Efficient GPU rendering using byte grouping
* Region based chunk storage
* 2D Perlin Noise cave generation
* Background and foreground tiles

### Planned feature's
* Lighting
* Entites rendering
* Inventory and items
* Physics elements (e.g. Gravity)
* Tile and entity collision
* Metadata Tiles

## Build
CMake build system

Tested on both **Windows** and **Linux**
```
git clone https://github.com/KeanBuyst/2DMinecraft.git
cd ./2DMinecarft
mkdir build
cd ./build
cmake ..
make
```
If your pc dosent have `glu` installed. Please install it. (only necessary if error in the cmake compilation accoure)
