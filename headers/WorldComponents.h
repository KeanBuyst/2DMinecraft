#pragma once

#include <World.h>

struct CollisonResult
{
    bool top = false;
    bool right = false;
    bool bottom = false;
    bool left = false;

    std::vector<GameObject*> entities;
    std::vector<Block> blocks;
};

class HitBox : public Component 
{
    public:
    HitBox(World* world, sf::FloatRect bounds);

    void Render(sf::RenderWindow* window);
    void Update(float delta);
    void Event(sf::Event* event){};

    bool intersects(sf::FloatRect other);
    sf::FloatRect getRect();

    bool debug = false;
    CollisonResult result;
    private:
    sf::FloatRect bounds;
    World* world;
};

class PhysicsObject : public Component 
{
    public:
    sf::Vector2f force;
    sf::Vector2f velocity;
    PhysicsObject();
    void Update(float delta);
    void Render(sf::RenderWindow* window){};
    static constexpr float GRAVITY = .8f;
};

class PlayerController : public PhysicsObject
{
    public:
    PlayerController(World* world);
    void Render(sf::RenderWindow* window){};
    void Update(float delta);
    void Event(sf::Event* event);
    private:
    World* world;
    const float SPEED = 5;
};