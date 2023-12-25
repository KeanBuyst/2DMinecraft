#include "Components.h"

#include <iostream>
#include <math.h>

#define RAD2DEG 180 / M_PI

using namespace sf;

void Component::init(GameObject* parent){
    gameObject = parent;
}

GameObject::GameObject() {
    type = ComponentType::GameObject;
}

GameObject::GameObject(float x, float y){
    type = ComponentType::GameObject;
    position = sf::Vector2f(x,y);
}
GameObject::GameObject(sf::Vector2f& pos){
    type = ComponentType::GameObject;
    position = pos;
}

void GameObject::setTexture(sf::Texture* texture)
{
    Sprite::setTexture(*texture);
    trect = Sprite::getTextureRect();
}
void GameObject::setRotation(float angle)
{Sprite::setRotation(angle);}

void GameObject::flip(bool backwords){
    if (backwords) Sprite::setTextureRect(sf::IntRect(trect.width, 0, -trect.width, trect.height));
    else Sprite::setTextureRect(trect);
}

void GameObject::Render(sf::RenderWindow* window,sf::Shader* shader){
    if (draw)
        window->draw(*this,shader);
    for (Component* component : components){
        component->Render(window,shader);
    }
}

void GameObject::Update(float delta){
    sf::Vector2f truePos = parent + position;
    for (Component* component : components){
        if (component->type == ComponentType::GameObject){
            static_cast<GameObject*>(component)->setParent(truePos);
        }
        component->Update(delta);
    }
    Sprite::setPosition(truePos);
}

void GameObject::Event(sf::Event* event){
    for (Component* component : components){
        component->Event(event);
    }
}

void GameObject::setParent(sf::Vector2f p){
    parent = p;
}

FloatRect GameObject::transform(FloatRect rect){
    return getTransform().transformRect(rect);
}

void GameObject::setPivotPoint(sf::Vector2f point){
    Sprite::setOrigin(point);
    position += point;
};

FloatRect GameObject::getBounds(){
    return Sprite::getGlobalBounds();
}

Vector2f GameObject::getGlobalPosition(){
    return parent + position;
}

void GameObject::addComponent(Component* component){
    component->init(this);
    components.push_back(component);
}

HeadMove::HeadMove()
{type = ComponentType::HeadMove;}

void HeadMove::Event(sf::Event* event){
    if (event->type == sf::Event::MouseMoved)
    {
        sf::Event::MouseMoveEvent e = event->mouseMove;
        int y = e.y - gameObject->position.y;
        int x = e.x - gameObject->position.x;
        gameObject->setRotation(atan2(y,x) * RAD2DEG);
    }
}