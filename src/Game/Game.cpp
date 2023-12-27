#include "Game.h"

#include<iostream>

Game::Game(sf::RenderWindow* w,sf::Shader* s) : window(w), shader(s) {
    shader->setUniform("texture", sf::Shader::CurrentTexture);
    world = new World(&TEXTURES,shader);

    GameObject* player = build(PLAYER);
    player->position = Vector2f(300,200);
    world->entities.push_back(player);
}
Game::~Game(){
    delete world;
}

void Game::draw(){
    world->draw(window);
}

void Game::update(float delta){
    world->update(delta,&TEXTURES);
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
        head->addComponent(new PlayerWorldInteract(world));

        GameObject* body = new GameObject();
        body->setTexture(&textures[0]);

        GameObject* legs = new GameObject();
        legs->draw = false;

        GameObject* leftLeg = new GameObject(0,12);
        leftLeg->setTexture(&textures[2]);

        GameObject* rightLeg = new GameObject(0,12);
        rightLeg->setTexture(&textures[2]);

        legs->addComponent(leftLeg);
        legs->addComponent(rightLeg);

        GameObject* arm = new GameObject(0,0);
        arm->setTexture(&textures[1]);

        player->addComponent(body);
        player->addComponent(legs);
        player->addComponent(arm);

        player->addComponent(head);

        sf::FloatRect bounds(0,-8,4,31);
        HitBox* box = new HitBox(world,bounds);
        player->addComponent(box);

        PlayerController* controller = new PlayerController(world);
        player->addComponent(controller);
        legs->addComponent(new WalkingAnimation(&controller->in));
        

        return player;
    }

    throw "Failed to build component";
}