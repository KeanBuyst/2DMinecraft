#include "WorldComponents.h"

#include <iostream>
#include <cmath>

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

    if (!onGround) force.y += GRAVITY;
    if (std::abs(velocity.x) <= 1) velocity.x += force.x;
    velocity.y += force.y;


    force = sf::Vector2f(0,0);

    if (result.shift.x == 0){
        gameObject->position.x += velocity.x;
    } else {
        gameObject->position.x -= result.shift.x;
        velocity.x = 0;
    }

    if (result.shift.y == 0){
        gameObject->position.y += velocity.y;
    } else {
        gameObject->position.y -= result.shift.y;
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
        force.y -= JUMP;
    if (in.LEFT)
        force.x -= SPEED;
    if (in.RIGHT)
        force.x += SPEED;

    PhysicsObject::Update(delta);

    world->camera = gameObject->position - Vector2f(300,200);
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
            break;
        case Keyboard::D: // D
            in.RIGHT = down;
            break;
    }
}

HitBox::HitBox(World* world,FloatRect bounds) : WorldComponent(world), bounds(bounds)
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
    CHUNK c = Chunk::getChunkCord(gameObject->position.x);
    Chunk* chunk = world->getChunk(c);
    FloatRect rect = getRect();
    if (chunk)
    {
        for (int i = 0; i < Chunk::WIDTH; i++){
            for (Block& block : chunk->blocks[i]){
                if(!MetaData::GetStaticMeta(block.material).colliable) continue;
                Vector2i pos = Block::getBlockCord(block.getPosition());
                FloatRect bounds = block.getGlobalBounds();
                if (intersects(bounds)){
                    result.shift += getPenetration(rect,bounds);
                    result.collison = true;

                    result.blocks.push_back(block);
                }
                // do weird magic
                if (pos.x < 0) pos.x++;
                // increase precision
                Vector2i other[2];
                other[0] = Block::getBlockCord(rect.left + rect.width,rect.top + rect.height);
                other[1] = Block::getBlockCord(rect.left,rect.top + rect.height);
                unsigned short amount = other[0] == other[1] ? 1 : 2;
                for (unsigned short i = 0; i < amount; i++)
                {
                    if (pos.y == other[i].y)
                    {
                        if (pos.x == other[i].x){
                            result.onGround = true;
                            result.blocks.push_back(block);
                            break;
                        }
                    }
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

        window->draw(rect,shader);

        // draw blocks
        for (Block& block : result.blocks){
            FloatRect bounds = block.getGlobalBounds();
            RectangleShape r(bounds.getSize());
            r.setPosition(bounds.getPosition());

            r.setOutlineThickness(1);
            r.setOutlineColor(Color::White);

            r.setFillColor(Color::Transparent);

            window->draw(r,shader);
        }
    }
}

float headMove(float x, float y){
    float angle = atan2(y,x) * RAD2DEG;
    if (angle > 70)
        angle = 70;
    if (angle < -70)
        angle = -70;
    return angle;
}

void PlayerWorldInteract::Event(sf::Event* event)
{
    switch (event->type)
    {
        case sf::Event::MouseMoved:
            {
                Vector2f pos = mousePos - Vector2f(300,200);
                if (pos.x < 0)
                {
                    gameObject->flip(true);
                    gameObject->setRotation(headMove(-pos.x,-pos.y));
                }
                else
                {
                    gameObject->flip(false);
                    gameObject->setRotation(headMove(pos.x,pos.y));
                }
            }
            break;
        case sf::Event::MouseButtonPressed:
            {
                sf::Event::MouseButtonEvent e = event->mouseButton;
                switch (e.button)
                {
                    case Mouse::Left:
                        // break block over time
                        breakData.block = world->getBlock(mousePos + world->camera);
                        if (!breakData.block) return;
                        breakData.durationRequired = MetaData::GetStaticMeta(breakData.block->material).breakTime;
                        breakData.breaking = true;
                        break;
                    // case Mouse::Right:
                    //     // TODO place block
                    //     break;
                }
            }
            break;
        case sf::Event::MouseButtonReleased:
            {
                sf::Event::MouseButtonEvent e = event->mouseButton;
                switch (e.button)
                {
                    case Mouse::Left:
                        breakData.breaking = false;
                        break;
                }
            }
            break;
    }
}

void PlayerWorldInteract::Update(float delta)
{
    if (!breakData.breaking) return;
    Vector2i pos = Block::getBlockCord(breakData.block->getPosition());
    if (pos != Vector2i(mousePos))
    {
        breakData.breaking = false;
        return;
    }
    breakData.durationLeft += 1;
    if (breakData.durationLeft < breakData.durationRequired)
    {
        std::cout << "Block broken" << std::endl;
    }
};

void PlayerWorldInteract::Render(RenderWindow* window, Shader* shader)
{
    // get relative postition
    Vector2i pos = Mouse::getPosition(*window);
    Vector2u size = window->getSize();
    mousePos = Vector2f((float) pos.x / size.x,(float) pos.y / size.y);
    mousePos.x *= 600;
    mousePos.y *= 400;

    if(!breakData.block) return;
    FloatRect bounds = breakData.block->getGlobalBounds();
    RectangleShape shape(bounds.getSize());
    shape.setPosition(bounds.getPosition());

    shape.setOutlineThickness(-1);
    shape.setFillColor(Color::Transparent);

    window->draw(shape,shader);
}