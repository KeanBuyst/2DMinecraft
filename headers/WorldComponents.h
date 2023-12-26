#pragma once

#include <World.h>

class WorldComponent : public Component
{
    public:
    WorldComponent(World* world) : world(world){};
    protected:
    World* world;
};

struct CollisonResult
{
    bool collison = false;
    bool onGround = false;

    Vector2f shift;

    std::vector<GameObject*> entities;
    std::vector<Block> blocks;
};

class HitBox : public WorldComponent 
{
    public:
    HitBox(World* world, sf::FloatRect bounds);

    void Awake(){};
    void Render(sf::RenderWindow* window,sf::Shader* shader);
    void Update(float delta);
    void Event(sf::Event* event){};

    bool intersects(sf::FloatRect other);
    sf::FloatRect getRect();

    bool debug = false;
    CollisonResult result;
    private:
    sf::FloatRect bounds;
};

class PhysicsObject : public Component 
{
    public:
    sf::Vector2f force;
    sf::Vector2f velocity;
    bool moving = false;
    bool onGround = false;
    PhysicsObject();
    void Awake(){};
    void Update(float delta);
    void Render(sf::RenderWindow* window,sf::Shader* shader){};
    static constexpr float GRAVITY = .8f;
    static constexpr float FRICTION = .1f;
};

class PlayerController : public PhysicsObject
{
    public:
    PlayerController(World* world);
    void Awake(){};
    void Render(sf::RenderWindow* window,sf::Shader* shader){};
    void Update(float delta);
    void Event(sf::Event* event);

    INPUT in;
    private:
    void Input(int key,bool down);
    World* world;
    const float SPEED = 1;
    const float JUMP = 3.5f;
};

struct BreakData
{
    Block* block = nullptr;
    Vector2i current;
    unsigned int durationLeft = 0;
    unsigned int durationRequired = 0;
    bool breaking = false;
};
/*
class PlayerWorldInteract : public WorldComponent
{
    public:
    PlayerWorldInteract(World* world) : WorldComponent(world) {};
    void Render(sf::RenderWindow* window,sf::Shader* shader);
    void Update(float delta);
    void Event(sf::Event* event);
    private:
    BreakData breakData;
    Vector2i blockPos;
};*/