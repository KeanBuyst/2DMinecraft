#include "World.h"

#include <iostream>
#include <fstream>
#include <PerlinNoise.h>
#include <sstream>

Block::Block(Material mat) : material(mat) {}

void Block::setPosition(BLOCK x,BLOCK y){
    Sprite::setPosition(x * Block::SIZE,y * Block::SIZE);
}

Chunk::Chunk(CHUNK x,Textures* textures, Biome* biome) : x(x) {
    // TODO if exsists load
        // create Noise
        siv::PerlinNoise Noise { World::seed };
        int pos = x * WIDTH;
        for (BLOCK X = pos; X < pos + WIDTH; X++){
            BLOCK noise = Noise.noise1D(X / biome->smoothness) * biome->amplitude - biome->height;
            Vector2i vect(X,noise - 1);
            for (Block& block : biome->getPlant(vect))
            {
                block.setTexture(*textures->getTexture(block.material));
                blocks.push_back(block);
            }
            for (BLOCK Y = noise; Y <= HEIGHT; Y++){
                int layer = Y - noise;
                Material mat = biome->getMaterial(layer);
                Block block(mat);
                block.setTexture(*textures->getTexture(mat));
                block.setPosition(X,Y);
                blocks.push_back(block);
            }
        }
}

Chunk::~Chunk(){
    upload();
}

void Chunk::upload(){
    // std::ofstream stream;
    // std::stringstream name;
    // name << x << ".chunk";
    // char* n;
    // name >> n;
    // stream.open(n, std::ios::binary | std::ios::out);
    // char blockData[Chunk::WIDTH*Chunk::HEIGHT] = {0u};
    // for (Block block : blocks){
    //     Vector2 pos = block.getPosition();
    //     int index = pos.y * Chunk::HEIGHT + pos.x;
    //     blockData[index] = (char) ((unsigned char) block.material);
    // }
    // stream.write(blockData,sizeof(blockData));
    // stream.flush();
    // stream.close();
}

std::vector<Block>& Chunk::getBlocks(){
    return blocks;
}

void World::draw(sf::RenderWindow* window){
    shader->setUniform("camera",camera);
    for (unsigned short i = 0; i <= SIZE*2; i++){
        for (Sprite sprite : chunks[i].getBlocks()){
            Vector2f vector = sprite.getPosition() - camera;
            if (vector.x + Block::SIZE <= 0 || vector.x >= 600) continue;
            if (vector.y <= 0 || vector.y - Block::SIZE >= 400) continue;
            window->draw(sprite,shader);
        }
    }
    // stop laggy player movement
    shader->setUniform("camera",camera);
    for (GameObject* entity : entities)
    {
        entity->Render(window,shader);
    }
}

Block* World::getBlock(int x,int y)
{
    Chunk* chunk = getChunk(Chunk::getChunkCord(x));
    for (Block& block : chunk->blocks)
    {
        Vector2f pos = block.getPosition();
        if ((int) (pos.x) == x)
            return &block;
    }
    return nullptr;
}

Material Biome::getMaterial(int layer){
    if (layer == 0)
    {
        return Material::GRASS_BLOCK;
    }
    else if (layer > 4)
    {
        return Material::STONE_BLOCK;
    }
    else 
    {
        return Material::DIRT_BLOCK;
    }
}

World::World(Textures* textures,Shader* shader) : shader(shader) {
    srand(seed);

    biomes = new Biome*[1] 
    {
        new PlanesBiome()
    };
    currentBiome = (unsigned short) (((float) rand() / RAND_MAX) * (sizeof(biomes) / sizeof(Biome)));
    CHUNK pos = Chunk::worldToChunk(300);
    chunks = new Chunk[5]
    {
        Chunk(pos - 2,textures,biomes[currentBiome]),
        Chunk(pos - 1,textures,biomes[currentBiome]),
        Chunk(pos,textures,biomes[currentBiome]),
        Chunk(pos + 1,textures,biomes[currentBiome]),
        Chunk(pos + 2,textures,biomes[currentBiome])
    };
}

Chunk* World::getChunk(CHUNK pos){
    for (int i = 0; i <= SIZE*2; i++)
        if (chunks[i].x == pos) return &chunks[i];
    return nullptr;
}

World::~World(){
    delete[] biomes;
    delete[] chunks;
}

void World::update(float delta,Textures* textures){
    CHUNK cord = Chunk::worldToChunk(entities[0]->position.x);
    
    unsigned short index;
    if (chunks[3].x <= cord){
        index = 0;
        for (CHUNK pos = cord - SIZE; pos <= cord + SIZE; pos++){
            rechunk(index,pos,textures);
            index++;
        }
    }
    else {
        index = SIZE * 2;
        for (CHUNK pos = cord + SIZE; pos >= cord - SIZE; pos--){
            rechunk(index,pos,textures);
            index--;
        }
    }
    // entities
    for (GameObject* entity : entities)
        entity->Update(delta);
}

void World::rechunk(unsigned short index,CHUNK pos, Textures* textures){
    for (int i = 0; i <= SIZE * 2; i++){
        if (chunks[i].x == pos){
            chunks[index] = chunks[i];
            return;
        }
    }
    // biome change?
    if (RANDOM() < 0.1f)
        currentBiome = RANDOM() * (sizeof(biomes) / sizeof(Biome));
    std::cout << "Generating chunk at " << pos << std::endl;
    chunks[index] = Chunk(pos,textures,biomes[currentBiome]);;
}

CHUNK Chunk::getChunkCord(int x){
    return (x / (Block::SIZE * Chunk::WIDTH)) - (x < 0);
}
Vector2<BLOCK> Block::getBlockCord(int x,int y){
    return Vector2<BLOCK>((x / Block::SIZE) - (x < 0),(y / Block::SIZE) - (y < 0));
}
Vector2<BLOCK> Block::getBlockCord(Vector2f pos){
    return Block::getBlockCord(pos.x,pos.y);
}

float World::RANDOM(){
    return (float) rand() / RAND_MAX;
}

CHUNK Chunk::worldToChunk(int x){
    return x / (Block::SIZE * Chunk::WIDTH);
}

Biome::Biome(float s,int a,int h) : smoothness(s), amplitude(a), height(h)
{}

PlanesBiome::PlanesBiome() : Biome(10,5,-20)
{}

std::vector<Block> PlanesBiome::getPlant(Vector2<BLOCK> pos){
    std::vector<Block> blocks;
    float rand = World::RANDOM();
    if (rand < 0.1)
    {
        BLOCK height = pos.y - (World::RANDOM() * 2 + 1);
        for (BLOCK h = pos.y; h > height; h--){
            Block block(Material::OAK_LOG);
            block.setPosition(pos.x,h);
            block.setColor(Color::Green);
            blocks.push_back(block);
        }
        for (int y = height; y >= height-2; y--){
            for (int x = pos.x - 2; x <= pos.x + 2; x++){
                Block block(Material::OAK_LEAVES);
                block.setPosition(x,y);
                block.setColor(Color::Green);
                blocks.push_back(block);
            }
        }
    }
    else if (rand < 0.6)
    {
        Block block(Material::GRASS);
        block.setPosition(pos.x,pos.y);
        block.setColor(Color::Green);
        blocks.push_back(block);
    }
    return blocks;
}

int chunkToWorld(CHUNK x){
    return (x * Chunk::WIDTH * Block::SIZE) * -1;
}