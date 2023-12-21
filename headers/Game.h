#pragma once

#include <WorldComponents.h>

#include <vector>

class Game {
    public:
    Textures TEXTURES;
    sf::RenderWindow* window;
    World* world;

    Game(sf::RenderWindow* w);
    ~Game();

    void draw();
    void update(float delta);
    void event(sf::Event* event);
    GameObject* build(EntityType type);
};