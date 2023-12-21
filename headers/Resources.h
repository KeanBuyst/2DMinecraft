#pragma once

#include <SFML/Graphics.hpp>

typedef unsigned char byte;

using namespace sf;

enum Material {
    AIR_BLOCK = 0,
    GRASS_BLOCK = 1,
    STONE_BLOCK = 2,
    DIRT_BLOCK = 3,
    GRASS = 4,
    OAK_LEAVES = 5,
    OAK_LOG = 6
};

enum EntityType {
    PLAYER = 0
};

class Textures {
    private:
    Texture* materials;
    Texture** entities;
    public:
    Texture* getTexture(Material mat);
    Texture* getTexture(EntityType type);

    Textures();
    ~Textures();
};