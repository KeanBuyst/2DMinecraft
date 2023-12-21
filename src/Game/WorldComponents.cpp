#include "WorldComponents.h"

#include <iostream>

PhysicsObject::PhysicsObject()
{
    type = ComponentType::PhysicsObject;
}

void PhysicsObject::Update(float delta)
{
    force.y -= GRAVITY;
    velocity += force;

    force = sf::Vector2f(0,0);

    CollisonResult result = gameObject->getComponent<HitBox>(ComponentType::HitBox)->result;
    // part 1: stop object
    if (result.bottom)
        velocity.y = velocity.y > 0 ? 0 : velocity.y;
    if (result.top)
        velocity.y = velocity.y < 0 ? 0 : velocity.y;
    if (result.right)
        velocity.x = velocity.x > 0 ? 0 : velocity.x;
    if (result.left)
        velocity.x = velocity.x < 0 ? 0 : velocity.x;
     
    gameObject->position += velocity;

    // part 2: shift object
    
}

PlayerController::PlayerController(World* world) : world(world) {
    type = ComponentType::PlayerController;
}

void PlayerController::Update(float delta){
    gameObject->position = world->offset;
    PhysicsObject::Update(delta);
    world->offset = gameObject->position;
    gameObject->position = Vector2f(300,200);
}

void PlayerController::Event(sf::Event* event){
    if (event->type == sf::Event::KeyPressed)
    {
        switch (event->key.code)
        {
            case Keyboard::W: // W
                force.y += SPEED;
                break;
            case Keyboard::A: // A
                force.x += SPEED;
                break;
            case Keyboard::S: // S
                force.y -= SPEED;
                break;
            case Keyboard::D: // D
                force.x -= SPEED;
                break;
        }
    }
    
}

HitBox::HitBox(World* world,FloatRect bounds) : world(world), bounds(bounds)
{type = ComponentType::HitBox;}

void HitBox::Update(float delta){
    CollisonResult result;
    for (GameObject* obj : world->entities){
        if (obj == gameObject) continue;
        std::vector<HitBox*> hitboxs = obj->getComponents<HitBox>(ComponentType::HitBox);
        if (hitboxs.empty()) continue;
        if(intersects(hitboxs[0]->getRect())){
            result.entities.push_back(obj);
            std::cout << "Entitiy collison\n";
        }
    }
    CHUNK pos = world->getChunkCord(gameObject->position.x);
    Chunk* chunk = world->getChunk(pos);
    if (chunk)
    {
        for (Block block : chunk->getBlocks()){
            block.move(world->offset);
            Vector2f pos = block.getPosition();
            FloatRect bounds = block.getGlobalBounds(); 
            if (intersects(bounds)){
                FloatRect rect = getRect();
                bool b = rect.top + rect.height - 10 > bounds.top; 
                result.right = (rect.left + rect.width) - bounds.left > 0 && b ? true : result.right;
                result.left = rect.left - (bounds.left + bounds.width) < 0 && b ? true : result.left;
                result.top = rect.top - (bounds.top + rect.height) < 0 ? true : result.top;
                result.bottom = (rect.top + rect.height) - bounds.top > 0 ? true : result.bottom;
                result.blocks.push_back(block);
            }
        }
    }
    this->result = result;
}

FloatRect HitBox::getRect(){
    Vector2f pos = gameObject->position;
    return FloatRect(pos.x + bounds.left,pos.y + bounds.top,bounds.width,bounds.height);
}

bool HitBox::intersects(FloatRect other)
{
    FloatRect rect = getRect();
    return rect.intersects(other);
}

void HitBox::Render(RenderWindow* window){
    if (debug)
    {
        FloatRect bounds = getRect();
        RectangleShape rect(bounds.getSize());
        rect.setPosition(bounds.getPosition());

        rect.setOutlineThickness(2);
        rect.setOutlineColor(Color::Red);

        rect.setFillColor(Color::Transparent);

        window->draw(rect);

        // draw blocks
        for (Block& block : result.blocks){
            FloatRect bounds = block.getGlobalBounds();
            RectangleShape r(bounds.getSize());
            r.setPosition(bounds.getPosition());

            r.setOutlineThickness(2);
            r.setOutlineColor(Color::White);

            r.setFillColor(Color::Transparent);

            window->draw(r);
        }
    }
}