#include "Item.h"

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
    numOfComponents = 1;
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
    numOfComponents = 1;
}

void world::Item::toComponent()
{
    // remove unnecessary components
    for (auto i = 1; i < numOfComponents; ++i)
    {
        delete components[i];
        components[i] = nullptr;
    }
    numOfComponents = 1;
}

void world::Item::toEntity()
{
    // add necessary components
    auto* hitbox = new HitBox({-4,-4,4,4});
    components[1] = hitbox;
    components[2] = new RigidBody(hitbox);
    numOfComponents = 3;
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
