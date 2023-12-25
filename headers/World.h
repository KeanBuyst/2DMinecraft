#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <MetaData.h>

#include <Components.h>

typedef int CHUNK;
typedef int BLOCK;

class Block : public Sprite {
    public:
    static const int SIZE = 16;
    Block(Material mat);
    void setPosition(BLOCK x,BLOCK y);
    Material material;
};

class Biome {
    public:
    const float smoothness;
    const int amplitude;
    const int height;
    Biome(float smoothness,int amplitude,int height);
    virtual Material getMaterial(int layer);
    virtual std::vector<Block> getPlant(Vector2<BLOCK> position) = 0;
};

class Chunk {
    public:
    static const int WIDTH = 16,HEIGHT = 50;
    static CHUNK worldToChunk(int x);
    CHUNK x;

    Chunk(CHUNK x,Textures* textures, Biome* biome);
    ~Chunk();
    void upload();
    std::vector<Block>& getBlocks();
    private:
    std::vector<Block> blocks;
};

class World {
    public:
    static const unsigned long seed = 1234435436u;

    World(Textures* textures);
    ~World();

    static const CHUNK SIZE = 2;
    Vector2f offset;
    unsigned short currentBiome;
    void update(Textures* textures);
    void draw(sf::RenderWindow* window,sf::Shader* shader);
    CHUNK getChunkCord(int x);
    Chunk* getChunk(CHUNK pos);
    static float RANDOM();
    Biome** biomes;
    Chunk* chunks;
    std::vector<GameObject*> entities;
    private:
    void rechunk(unsigned short index,CHUNK pos,Textures* textures);
};

class PlanesBiome : public Biome {
    public:
    PlanesBiome();
    std::vector<Block> getPlant(Vector2i position);
};

int chunkToWorld(CHUNK x);