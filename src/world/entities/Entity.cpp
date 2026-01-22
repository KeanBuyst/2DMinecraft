#include "Entity.h"

#include "../../gl/Texture.h"
#include "Procedure.h"

#include <chrono>

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
    combined.insert(combined.end(), itemBatch.begin(), itemBatch.end());
    combined.insert(combined.end(), entityBatch.begin(), entityBatch.end());

    return combined.data();
}

float world::GetMaxHealth(EntityType type)
{
    switch (type)
    {
    case EntityType::PLAYER:
        return 100.0f;
    case EntityType::ITEM:
        return 1.0f;
    default:
        return -1.0f;
    }
}

world::Entity::Entity(EntityType type,gl::Frame dimensions,Util::ByteStream& stream) :
    Transform(stream), next(nullptr), dimensions(dimensions),
    appliedMovement(false), doDeque(false), type(type), acceleration(0.0f,0.0f)
{
    stream >> id;
    stream >> health;
    stream >> velocity.x;
    stream >> velocity.y;
}

world::Entity::Entity(const glm::vec2 position,gl::Frame dimensions,const EntityType type) :
    Transform(position), next(nullptr), id((Util::GetTimeBasedID() << 22) | Util::GetRadomNumber(22)),
    dimensions(dimensions), appliedMovement(false), doDeque(false), type(type), velocity(0.0f,0.0f), acceleration(0.0f,0.0f)
{
    max_health = GetMaxHealth(type);
    health = max_health;
}

void world::Entity::update()
{
    velocity += acceleration * delta_time;
    Procedure::HitBoxResult collision = Procedure::HitBox(this);
    Procedure::RigidBody(this,collision);
    position += velocity * delta_time;
}

void world::Entity::serialize(Util::ByteStream& stream)
{
    stream << static_cast<uint8_t>(type);
    Transform::serialize(stream);
    stream << id;
    stream << health;
    stream << velocity.x;
    stream << velocity.y;
    // not storing acceleration
}

bool world::Entity::hasAppliedMovement() const
{
    return appliedMovement;
}

gl::Frame world::Entity::getDimensions() const
{
    return dimensions;
}

uint64_t world::Entity::getID() const
{
    return id;
}

world::Entity* world::Entity::getNext()
{
    if (next)
    {
        if (next->dead())
        {
            Entity* temp = next->next;
            if (destroy()) delete next;
            else
            {
                next->setNext(nullptr);
                next->requeue();
            }
            next = temp;
        }
    }
    return next;
}

void world::Entity::setNext(Entity* entity)
{
    next = entity;
}

bool world::Entity::destroy()
{
    return doDeque == 1;
}

bool world::Entity::dead()
{
    return health <= 0 || doDeque;
}

// if soft deque we don't delete the memory
void world::Entity::deque(bool soft)
{
    doDeque = 1 + soft;
}

void world::Entity::requeue()
{
    doDeque = 0;
}
