# 2D Minecraft
Simple 2D Minecraft clone written in C++ with SDL2 and glew.

### Current feature's
* 2D chunks (thus "infinite" x and y generation)
* Perlin Noise chunk generation
* Efficient GPU rendering using byte grouping
* Region based chunk storage
* 2D Perlin Noise cave generation
* Background and foreground tiles
* Entities rendering
* Tile Collision
* Physics elements (e.g. Gravity)
* Lighting
* Entity Lighting
* Entity Animation
* Inventory and items

### Planned feature's
* Entity Collision
* Metadata Tiles
* Multiple biomes
* Items
* Block breaking and placing

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
