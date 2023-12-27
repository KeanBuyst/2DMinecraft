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
    void setBlockPos(Vector2<BLOCK> block);
    Vector2<BLOCK> getBlockPos();
    Material material;

    static Vector2<BLOCK> getBlockCord(int x, int y);
    static Vector2<BLOCK> getBlockCord(Vector2f pos);
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
    std::vector<Block> blocks[WIDTH];
    Block* getBlock(Vector2<BLOCK> position);
    void addBlock(Block& block);

    static CHUNK getChunkCord(int x);
};

class World {
    public:
    static const unsigned long seed = 1234435436u;

    World(Textures* textures, Shader* shader);
    ~World();

    static const CHUNK SIZE = 2;
    unsigned short currentBiome;
    void update(float delta,Textures* textures);
    void draw(sf::RenderWindow* window);
    Chunk* getChunk(CHUNK pos);
    Block* getBlock(Vector2f pos);
    static float RANDOM();
    Biome** biomes;
    Chunk* chunks;

    sf::Vector2f camera;

    std::vector<GameObject*> entities;
    private:
    Shader* shader;
    void rechunk(unsigned short index,CHUNK pos,Textures* textures);
};

class PlanesBiome : public Biome {
    public:
    PlanesBiome();
    std::vector<Block> getPlant(Vector2i position);
};

int chunkToWorld(CHUNK x);