#pragma once

#include <WorldComponents.h>

#include <vector>

class Game {
    public:
    Textures TEXTURES;
    sf::RenderWindow* window;
    sf::Shader* shader;
    World* world;
    bool running = true;

    Game(sf::RenderWindow* w, sf::Shader* shader);
    ~Game();

    void draw();
    void update(float delta);
    void event(sf::Event* event);
    GameObject* build(EntityType type);
};