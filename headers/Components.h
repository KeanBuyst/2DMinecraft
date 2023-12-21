#pragma once

#include <SFML/Graphics.hpp>

enum class ComponentType {
    NONE,
    GameObject,
    PhysicsObject,
    PlayerController,
    HitBox,
    HeadMove
};

class GameObject;

class Component
{
    public:
    void init(GameObject* parent);

    virtual void Update(float delta) = 0;
    virtual void Render(sf::RenderWindow* window) = 0;
    virtual void Event(sf::Event* event) = 0;
    ComponentType type;
    protected:
    GameObject* gameObject;
};

class GameObject : public Component,private sf::Sprite
{
    public:
    GameObject();
    GameObject(float x, float y);
    GameObject(sf::Vector2f& pos);

    void Update(float delta);
    void Render(sf::RenderWindow* window);
    void Event(sf::Event* event);

    void addComponent(Component* component);
    template<class T> std::vector<T*> getComponents(ComponentType type){
        std::vector<T*> comps;
        for (Component* comp : components)
            if (comp->type == type)
                comps.push_back(static_cast<T*>(comp));
        return comps;
    }
    template<class T> T* getComponent(ComponentType type){
        for (Component* comp : components)
            if (comp->type == type)
                return static_cast<T*>(comp);
        return nullptr;
    }

    void setTexture(sf::Texture* texture);
    void setPivotPoint(sf::Vector2f point);
    void setRotation(float angle);
    void setParent(sf::Vector2f parent);
    sf::FloatRect transform(sf::FloatRect rect);

    sf::FloatRect getBounds();
    sf::Vector2f getGlobalPosition();

    bool draw = true;
    sf::Vector2f position;
    private:
    std::vector<Component*> components;
    sf::Vector2f parent;
};

class HeadMove : public Component 
{
    public:
    HeadMove();
    void Render(sf::RenderWindow* window){};
    void Update(float delta){};
    void Event(sf::Event* event);
};