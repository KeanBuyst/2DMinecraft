#include "Resources.h"

#include <iostream>

void LoadTexture(Texture& texture, std::string filename){
    filename = "assets/" + filename;
    if(!texture.loadFromFile(filename)){
        std::cout << "Failed to load file from path\n";
    }
}

Textures::Textures(){
    Texture GRASS_BLOCK;
    LoadTexture(GRASS_BLOCK,"blocks/grass_block.png");
    Texture DIRT_BLOCK;
    LoadTexture(DIRT_BLOCK,"blocks/dirt_block.png");
    Texture STONE_BLOCK;
    LoadTexture(STONE_BLOCK,"blocks/stone_block.png");
    Texture GRASS;
    LoadTexture(GRASS,"blocks/grass.png");
    Texture OAK_LEAVES;
    LoadTexture(OAK_LEAVES, "blocks/oak_leaves.png");
    Texture OAK_LOG;
    LoadTexture(OAK_LOG,"blocks/oak_log.png");

    materials = new Texture[6]
    {
        GRASS_BLOCK,
        STONE_BLOCK,
        DIRT_BLOCK,
        GRASS,
        OAK_LEAVES,
        OAK_LOG
    };

    Texture PLAYER_BODY, PLAYER_ARM, PLAYER_LEG, PLAYER_HEAD;
    LoadTexture(PLAYER_BODY, "entities/player/body.png");
    LoadTexture(PLAYER_ARM, "entities/player/arm.png");
    LoadTexture(PLAYER_LEG, "entities/player/leg.png");
    LoadTexture(PLAYER_HEAD, "entities/player/head.png");
    Texture* PLAYER = new Texture[4]
    {PLAYER_BODY,PLAYER_ARM,PLAYER_LEG,PLAYER_HEAD};

    entities = new Texture*[1]
    {
       PLAYER
    };
}

Textures::~Textures(){
    delete[] materials;
    delete[] entities;
}

Texture* Textures::getTexture(Material mat){
    if (mat == Material::AIR_BLOCK)
    {
        std::cout << "ERROR: Attempted to access AIR_BLOCK texture in Resources.cpp" << std::endl;
        return nullptr;
    }
    return &materials[mat - 1];
}

Texture* Textures::getTexture(EntityType type){
    return entities[type];
}