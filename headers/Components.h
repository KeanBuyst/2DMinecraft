#pragma once

#include <SFML/Graphics.hpp>

#define RAD2DEG 180 / M_PI

enum class ComponentType {
    NONE,
    GameObject,
    PhysicsObject,
    PlayerController,
    HitBox,
    HeadMove,
    Animation
};

class GameObject;

class Component
{
    public:
    void init(GameObject* parent);

    virtual void Awake() = 0;
    virtual void Update(float delta) = 0;
    virtual void Render(sf::RenderWindow* window,sf::Shader* shader) = 0;
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

    void Awake() {};
    void Update(float delta);
    void Render(sf::RenderWindow* window,sf::Shader* shader);
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
    bool hasComponent(ComponentType type);

    void flip(bool backwords);
    void setTexture(sf::Texture* texture);
    void setPivotPoint(sf::Vector2f point);
    void setRotation(float angle);
    void rotate(float angle);
    float getRotation();

    void setParent(sf::Vector2f parent);
    sf::FloatRect transform(sf::FloatRect rect);

    sf::FloatRect getBounds();
    sf::Vector2f getGlobalPosition();

    bool draw = true;
    sf::Vector2f position;
    private:
    sf::IntRect trect;
    std::vector<Component*> components;
    sf::Vector2f parent;
};

struct INPUT 
{
    bool RIGHT = false;
    bool LEFT = false;
    bool JUMP = false;
};

class Animation : public Component
{
    public:
    Animation(INPUT* in);
    virtual std::string getName() = 0;
    protected:
    INPUT* in;
    // time / step = frame
    unsigned int time = 0;
    unsigned int frame = 0;
    unsigned int duration;
    unsigned int step;
};

class WalkingAnimation : public Animation
{
    public:
    WalkingAnimation(INPUT* in);

    void Awake();
    void Update(float delta);
    void Render(sf::RenderWindow* window,sf::Shader* shader) {};
    void Event(sf::Event* event) {};
    
    std::string getName();
    private:
    std::vector<GameObject*> legs;

    float r1,r2;
    float change;
};