#include "Components.h"

#include <iostream>
#include <math.h>

using namespace sf;

void Component::init(GameObject* parent){
    gameObject = parent;
    Awake();
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

bool GameObject::hasComponent(ComponentType type)
{
    for (Component* comp : components)
        if (comp->type == type) return true;
    return false;
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

void GameObject::rotate(float angle){
    Sprite::rotate(angle);
}

float GameObject::getRotation(){
    return Sprite::getRotation();
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

Animation::Animation(INPUT* in) : in(in)
{
    type = ComponentType::Animation;
}

WalkingAnimation::WalkingAnimation(INPUT* in) : Animation(in)
{
    step = 1;
}

void WalkingAnimation::Awake()
{
    legs = gameObject->getComponents<GameObject>(ComponentType::GameObject);
    r1 = 0;
    r2 = 0;
    change = 2.5f;
}

void WalkingAnimation::Update(float delta)
{
    if (in->LEFT || in->RIGHT)
    {
        if (r1 >= 35 || r1 <= -35)
            change *= -1;
        r1 += change;
        legs[0]->setRotation(r1);
        r2 -= change;
        legs[1]->setRotation(r2);
    }
    else
    {
        legs[0]->setRotation(0);
        legs[1]->setRotation(0);
    }
}

std::string WalkingAnimation::getName()
{
    return "walking";
};