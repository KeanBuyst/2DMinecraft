#include "MetaEntity.h"

#include "Item.h"

world::MetaEntity::MetaEntity(EntityType type, gl::Frame dimensions, Util::ByteStream& stream) : Entity(type,dimensions,stream)
{}

world::MetaEntity::MetaEntity(glm::vec2 position,gl::Frame dimensions,EntityType type)
    : Entity(position,dimensions,type), inventories(nullptr), numOfInventories(0)
{}

UI::Inventory& world::MetaEntity::getInventory(int index)
{
    if (index < 0 || index >= numOfInventories)
        throw "MetaEntity: Invalid inventory index";
    if (!inventories)
        throw "MetaEntity: No inventories created";
    return *inventories[index];
}

void world::MetaEntity::serialize(Util::ByteStream& stream)
{
    Entity::serialize(stream);
    // TODO inventory serialization
}

int world::MetaEntity::getNumberOfInventories()
{
    return numOfInventories;
}

world::MetaEntity::~MetaEntity()
{
    // TODO add inventory removal by storing it capabilities | UI::Renderer::remove();
}