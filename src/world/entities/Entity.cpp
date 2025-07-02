#include "Entity.h"

std::vector<world::EntityRenderData> world::entity_render_batch;

world::Entity::Entity(const glm::vec2 position,const EntityType type, const int numOfComponents) :
    Transform(position),type(type), numOfComponents(numOfComponents)
{
    components = new Component*[numOfComponents];
}

world::Entity::~Entity()
{
    for (auto i = 0; i < numOfComponents; ++i)
    {
        delete components[i];
        components[i] = nullptr;
    }
    delete[] components;
    components = nullptr;
}

void world::Entity::event(SDL_Event* event) const
{
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->event(event);
    }
}

void world::Entity::update(const float& delta_time)
{
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->update(delta_time);
    }
}

void world::Entity::render()
{
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->render();
    }
}

world::Component* world::Entity::getComponent(const ComponentType compType) const
{
    for (auto i = 0; i < numOfComponents; ++i)
    {
        if (components[i]->type == compType)
            return components[i];
    }
    return nullptr;
}

void world::Entity::addComponents(Component** comps, int size)
{
    if (size > numOfComponents)
        throw std::runtime_error("Attempted to add more components then the entity allows");
    if (size == -1) size = numOfComponents;
    for (auto i = 0; i < size; ++i)
    {
        comps[i]->parent = this;
    }
    components = comps;
}