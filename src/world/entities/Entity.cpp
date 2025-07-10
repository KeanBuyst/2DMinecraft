#include "Entity.h"

std::vector<world::EntityRenderData> world::entity_render_batch;

float world::GetMaxHealth(EntityType type)
{
    switch (type)
    {
    case PLAYER:
        return 100.0f;
    default:
        return -1.0f;
    }
}

world::Entity::Entity(const glm::vec2 position,const EntityType type, const int numOfComponents) :
    VectorTransform(position),type(type), numOfComponents(numOfComponents)
{
    max_health = GetMaxHealth(type);
    health = max_health;
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
    velocity += acceleration * delta_time;
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->update(delta_time);
    }
    // removes jitter
    position += glm::floor(velocity * 1000.0f * delta_time) / 1000.0f;
}

void world::Entity::render()
{
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->render();
    }
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