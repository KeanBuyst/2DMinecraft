#include "Game.h"

#include<iostream>

GameObject GeneratePlayer(Vector2f position,Texture* textures);

Game::Game(sf::RenderWindow* w,sf::Shader* s) : window(w), shader(s) {
    world = new World(&TEXTURES);
    shader->setUniform("texture", sf::Shader::CurrentTexture);

    GameObject* player = build(PLAYER);
    player->position = Vector2f(600/2,400/2);
    world->entities.push_back(player);
}
Game::~Game(){
    delete world;
}

void Game::draw(){
    world->draw(window,shader);
    for (GameObject* entity : world->entities)
        entity->Render(window,shader);
}

void Game::update(float delta){
    world->update(&TEXTURES);
    for (GameObject* entity : world->entities)
        entity->Update(delta);
}

void Game::event(sf::Event* event){
    for (GameObject* entity : world->entities)
        entity->Event(event);
}

GameObject* Game::build(EntityType type){
    Texture* textures = TEXTURES.getTexture(type);

    switch (type)
    {
    case PLAYER:
        GameObject* player = new GameObject();
        player->draw = false;

        GameObject* head = new GameObject(-2,-8);
        head->setTexture(&textures[3]);
        head->setPivotPoint(Vector2f(4,8));
        head->addComponent(new HeadMove());

        GameObject* body = new GameObject();
        body->setTexture(&textures[0]);

        GameObject* leftLeg = new GameObject(0,12);
        leftLeg->setTexture(&textures[2]);

        GameObject* rightLeg = new GameObject(0,12);
        rightLeg->setTexture(&textures[2]);

        GameObject* arm = new GameObject(0,0);
        arm->setTexture(&textures[1]);

        player->addComponent(body);
        player->addComponent(leftLeg);
        player->addComponent(rightLeg);
        player->addComponent(arm);
        player->addComponent(head);

        sf::FloatRect bounds(0,-8,4,31);
        HitBox* box = new HitBox(world,bounds);
        player->addComponent(box);

        player->addComponent(new PlayerController(world));

        return player;
    
    }

    throw "Failed to build component";
}