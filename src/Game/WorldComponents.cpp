#include "WorldComponents.h"

#include <iostream>

void apply(float& x, float friction);

PhysicsObject::PhysicsObject()
{
    type = ComponentType::PhysicsObject;
}

void PhysicsObject::Update(float delta)
{
    CollisonResult result = gameObject->getComponent<HitBox>(ComponentType::HitBox)->result;
    onGround = result.onGround;

    apply(velocity.x, FRICTION);

    if (!onGround) force.y -= GRAVITY;
    if (std::abs(velocity.x) <= 1) velocity.x += force.x;
    velocity.y += force.y;


    force = sf::Vector2f(0,0);

    if (result.shift.x == 0){
        gameObject->position.x += velocity.x;
    } else {
        gameObject->position.x += result.shift.x;
        velocity.x = 0;
    }

    if (result.shift.y == 0){
        gameObject->position.y += velocity.y;
    } else {
        gameObject->position.y += result.shift.y;
        velocity.y = 0;
    }
}

void apply(float& x, float friction){
    if (x == 0) return;
    if (x < 0){
        x += friction;
        if (x > 0) x = 0;
    }
    else {
        x -= friction;
        if (x < 0) x = 0;
    }
}

Vector2f getPenetration(const FloatRect& entityBounds, const FloatRect& blockBounds);

PlayerController::PlayerController(World* world) : world(world) {
    type = ComponentType::PlayerController;
}

void PlayerController::Update(float delta){
    if (in.JUMP && onGround)
        force.y += JUMP;
    if (in.LEFT)
        force.x += SPEED;
    if (in.RIGHT)
        force.x -= SPEED;

    gameObject->position = world->offset;
    PhysicsObject::Update(delta);
    world->offset = gameObject->position;
    gameObject->position = Vector2f(300,200);
}

void PlayerController::Event(sf::Event* event){
    if (event->type == sf::Event::KeyPressed)
    {
        Input(event->key.code,true);
    }
    if (event->type == sf::Event::KeyReleased)
    {
        Input(event->key.code,false);
    }
}

void PlayerController::Input(int key,bool down){
    switch (key)
        {
        case Keyboard::W: // W
            in.JUMP = down;
            break;
        case Keyboard::A: // A
            in.LEFT = down;
            if (down){
                for (GameObject* obj : gameObject->getComponents<GameObject>(ComponentType::GameObject))
                    obj->flip(true);
            }
            break;
        case Keyboard::D: // D
            in.RIGHT = down;
            if (down){
                for (GameObject* obj : gameObject->getComponents<GameObject>(ComponentType::GameObject))
                    obj->flip(false);
            }
            break;
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
    FloatRect rect = getRect();
    if (chunk)
    {
        for (Block block : chunk->getBlocks()){
            if(!MetaData::GetStaticMeta(block.material).colliable) continue;
            block.move(world->offset);
            Vector2f pos = block.getPosition();
            FloatRect bounds = block.getGlobalBounds();
            if (intersects(bounds)){
                result.shift += getPenetration(rect,bounds);
                result.collison = true;

                result.blocks.push_back(block);
            }
            if (((int) pos.y / Block::SIZE) == (int) ( rect.top + rect.height) / Block::SIZE)
            {
                if (((int) pos.x / Block::SIZE) == (int) (rect.left / Block::SIZE)){
                    result.onGround = true;
                    result.blocks.push_back(block);
                }
            }
        }
    }

    this->result = result;
}

Vector2f getPenetration(const FloatRect& entityBounds, const FloatRect& blockBounds){
    // Calculate the overlap in both x and y directions
    float xOverlap = std::min(entityBounds.left + entityBounds.width, blockBounds.left + blockBounds.width) -
                     std::max(entityBounds.left, blockBounds.left);
    float yOverlap = std::min(entityBounds.top + entityBounds.height, blockBounds.top + blockBounds.height) -
                     std::max(entityBounds.top, blockBounds.top);

    // Determine the direction of penetration
    float xPenetration = 0.0f;
    float yPenetration = 0.0f;

    if (entityBounds.top + entityBounds.height > blockBounds.top + blockBounds.width) {
        // Penetration is in the x-direction
        xPenetration = (entityBounds.left - blockBounds.left < 0) ? xOverlap : -xOverlap;
    } else {
        // Penetration is in the y-direction
        yPenetration = (entityBounds.top - blockBounds.top < 0) ? yOverlap : -yOverlap;
    }

    return Vector2f(xPenetration, yPenetration);
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

void HitBox::Render(RenderWindow* window,sf::Shader* shader){
    if (debug)
    {
        FloatRect bounds = getRect();
        RectangleShape rect(bounds.getSize());
        rect.setPosition(bounds.getPosition());

        rect.setOutlineThickness(1);
        rect.setOutlineColor(Color::Red);

        rect.setFillColor(Color::Transparent);

        window->draw(rect);

        // draw blocks
        for (Block& block : result.blocks){
            FloatRect bounds = block.getGlobalBounds();
            RectangleShape r(bounds.getSize());
            r.setPosition(bounds.getPosition());

            r.setOutlineThickness(1);
            r.setOutlineColor(Color::White);

            r.setFillColor(Color::Transparent);

            window->draw(r);
        }
    }
}