#pragma once

#include <World.h>

struct CollisonResult
{
    bool collison = false;
    bool onGround = false;

    Vector2f shift;

    std::vector<GameObject*> entities;
    std::vector<Block> blocks;
};

class HitBox : public Component 
{
    public:
    HitBox(World* world, sf::FloatRect bounds);

    void Render(sf::RenderWindow* window,sf::Shader* shader);
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

struct INPUT 
{
    bool RIGHT = false;
    bool LEFT = false;
    bool JUMP = false;
};

class PhysicsObject : public Component 
{
    public:
    sf::Vector2f force;
    sf::Vector2f velocity;
    bool moving = false;
    bool onGround = false;
    PhysicsObject();
    void Update(float delta);
    void Render(sf::RenderWindow* window,sf::Shader* shader){};
    static constexpr float GRAVITY = .8f;
    static constexpr float FRICTION = .1f;
};

class PlayerController : public PhysicsObject
{
    public:
    PlayerController(World* world);
    void Render(sf::RenderWindow* window,sf::Shader* shader){};
    void Update(float delta);
    void Event(sf::Event* event);
    private:
    INPUT in;
    void Input(int key,bool down);
    World* world;
    const float SPEED = 1;
    const float JUMP = 3.5f;
};