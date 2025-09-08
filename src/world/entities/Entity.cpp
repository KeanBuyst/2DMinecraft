#include "Entity.h"

#include "../../gl/Texture.h"

EntityHandler::EntityRenderBatch world::render_batch;

size_t EntityHandler::EntityRenderBatch::size() const
{
    return tileBatch.size() + itemBatch.size() + entityBatch.size();
}

EntityHandler::EntityRenderData* EntityHandler::EntityRenderBatch::data()
{
    thread_local std::vector<EntityRenderData> combined;
    combined.clear();

    combined.reserve(size());

    combined.insert(combined.end(), tileBatch.begin(), tileBatch.end());
    combined.insert(combined.end(), entityBatch.begin(), entityBatch.end());
    combined.insert(combined.end(), itemBatch.begin(), itemBatch.end());

    return combined.data();
}

float world::GetMaxHealth(EntityType type)
{
    switch (type)
    {
    case PLAYER:
        return 100.0f;
    case ITEM:
        return 1.0f;
    default:
        return -1.0f;
    }
}

world::Entity::Entity(const glm::vec2 position,const EntityType type) :
    VectorTransform(position),type(type), components(nullptr), numOfComponents(0)
{
    max_health = GetMaxHealth(type);
    health = max_health;
}

world::Entity::Entity(const Entity& other) : VectorTransform(other.position), type(other.type)
{
    components = new Component*[other.numOfComponents];
    numOfComponents = other.numOfComponents;

    for (auto i = 0; i < numOfComponents; ++i)
    {
        if (other.components[i] != nullptr)
        {
            components[i] = other.components[i]->clone();
            components[i]->entity = this;
        } else
        {
            components[i] = nullptr;
        }
    }
    max_health = other.max_health;
    // not copying over health
    health = max_health;
}

world::Entity::~Entity()
{
    if (components == nullptr) return;
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
    if (components == nullptr) return;
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->event(event);
    }
}

void world::Entity::update(const float& delta_time)
{
    if (components == nullptr) return;
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
    if (components == nullptr) return;
    for (auto i = 0; i < numOfComponents; ++i)
    {
        components[i]->render();
    }
}

void world::Entity::addComponents(Component** comps, const int size)
{
    for (auto i = 0; i < size; ++i)
    {
        if (comps[i] == nullptr) continue;
        comps[i]->entity = this;
    }
    numOfComponents = size;
    components = comps;
}