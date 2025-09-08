#include "Item.h"

#include "EntityHandler.h"
#include "../../gl/Texture.h"

world::Item::Item(glm::vec2 position, BlockType material): Entity(position,ITEM), material(material), isBlock(true), amount(1)
{
    auto* sprite = new Sprite({0,0,8,8},gl::AtlasTexture::getTexel(material - 1),1.0f);
    auto** components = new Component*[]
    {
        sprite,
        nullptr,
        nullptr
    };
    addComponents(components,3);
}

world::Item::Item(glm::vec2 position, ItemType material): Entity(position, ITEM), material(material), isBlock(false), amount(1)
{
    auto* sprite = new Sprite({0,0,8,8},gl::AtlasTexture::getTexel(material),1.0f);
    auto** components = new Component*[]
    {
        sprite,
        nullptr,
        nullptr
    };
    addComponents(components,3);
}

world::Item::Item(const Item& other)
    : Entity(other), material(other.material), isBlock(other.isBlock), amount(other.amount)
{}

void world::Item::toComponent()
{
    // remove unnecessary components
    for (auto i = 1; i < numOfComponents; ++i)
    {
        delete components[i];
        components[i] = nullptr;
    }
    health = 0;
}

void world::Item::toEntity()
{
    if (components[1] != nullptr || components[2] != nullptr) return;
    // add necessary components
    auto* hitbox = new HitBox({-5,-5,5,5});
    hitbox->entity = this;
    components[1] = hitbox;
    auto* rigid = new RigidBody(hitbox);
    rigid->entity = this;
    components[2] = rigid;
    health = 1;
}

bool world::Item::isMaterial(const BlockType mat) const
{
    return isBlock && material == mat;
}

bool world::Item::isMaterial(const ItemType mat) const
{
    return !isBlock && material == mat;
}

int world::Item::getAmount() const
{
    return amount;
}

void world::Item::setAmount(const int amount)
{
    this->amount = amount;
}
