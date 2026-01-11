#include "Sprite.h"

#include "../../Constants.h"
#include "../World.h"
#include "EntityHandler.h"
#include "item.h"

world::Sprite::Sprite() : Transform({0.0f,0.0f})
{}

world::Sprite::Sprite(gl::Frame dimensions,gl::TextureMap texture_map,float scale)
: Transform({(dimensions.left + dimensions.right) / 2.0f, (dimensions.top + dimensions.bottom) / 2.0f}),
    texture_map(texture_map), dimensions(dimensions), flipped(false)
{
    this->dimensions *= scale;
}

void world::Sprite::render(const Entity* entity)
{
    const float p1x = dimensions.left - pivot_point.x;
    const float p1y = dimensions.bottom - pivot_point.y;
    const float p2x = dimensions.right - pivot_point.x;
    const float p2y = dimensions.top - pivot_point.y;

    if (entity->velocity.x < 0)
    {
        flipped = true;
    }
    else if (entity->velocity.x != 0)
    {
        flipped = false;
    }

    const float netRot = entity->rotation + rotation;
    const float rot = flipped ? -netRot : netRot;

    glm::vec2 bottomLeft = entity->position + pivot_point + Util::rotate({p1x, p1y}, rot);
    glm::vec2 topRight = entity->position + pivot_point + Util::rotate({p2x, p2y}, rot);
    glm::vec2 topLeft = entity->position + pivot_point + Util::rotate({p1x, p2y}, rot);
    glm::vec2 bottomRight = entity->position + pivot_point + Util::rotate({p2x, p1y}, rot);

    // light calculation
    float
    light_BL, light_TL,
    light_TR, light_BR;

    const glm::vec2 b_BL = glm::floor(bottomLeft);
    const glm::vec2 b_TR = glm::floor(topRight);
    const glm::vec2 b_TL = glm::floor(topLeft);
    const glm::vec2 b_BR = glm::floor(bottomRight);

    // bottom left
    light_BL = static_cast<float>(world_ptr->getBlock(b_BL).getLightLevel());

    // top right
    if (b_TR == b_BL) light_TR = light_BL;
    else light_TR = static_cast<float>(world_ptr->getBlock(b_TR).getLightLevel());

    // top left
    if (b_TL == b_TR) light_TL = light_TR;
    else if (b_TL == b_BL) light_TL = light_BL;
    else light_TL = static_cast<float>(world_ptr->getBlock(b_TL).getLightLevel());

    // bottom right
    if (b_BR == b_BL) light_BR = light_BL;
    else if (b_BR == b_TR) light_BR = light_TR;
    else if (b_BR == b_TL) light_BR = light_TL;
    else light_BR = static_cast<float>(world_ptr->getBlock(b_BR).getLightLevel());


    const glm::vec2 texBottomLeft = {flipped ? texture_map.right : texture_map.left, texture_map.bottom};
    const glm::vec2 texBottomRight = {flipped ? texture_map.left : texture_map.right, texture_map.bottom};
    const glm::vec2 texTopRight = {flipped ? texture_map.left : texture_map.right, texture_map.top};
    const glm::vec2 texTopLeft = {flipped ? texture_map.right : texture_map.left, texture_map.top};

    bottomLeft = (bottomLeft - origin) * BLOCK_SIZE;
    topRight = (topRight - origin) * BLOCK_SIZE;
    topLeft = (topLeft - origin) * BLOCK_SIZE;
    bottomRight = (bottomRight - origin) * BLOCK_SIZE;

    // Insert two triangles (6 vertices)
    std::initializer_list<EntityHandler::EntityRenderData> data = {
        // First triangle
        {bottomLeft,  texBottomLeft,    light_BL},
        {bottomRight, texBottomRight,   light_BR},
        {topRight,    texTopRight,      light_TR},

        // Second triangle
        {topRight,    texTopRight,      light_TR},
        {topLeft,     texTopLeft,       light_TL},
        {bottomLeft,  texBottomLeft,    light_BL}
    };

    if (entity->type == EntityType::ITEM)
    {
        auto item = reinterpret_cast<const Item*>(entity);
        if (item->material.isBlock())
            render_batch.insert<BlockType>(data);
        else
            render_batch.insert<ItemType>(data);
    }
    else
        render_batch.insert<EntityType>(data);
}

bool world::Sprite::isFlipped()
{
    return flipped;
}

void world::Sprite::flip()
{
    flipped = !flipped;
}

void world::Sprite::flip(bool f)
{
    flipped = f;
}

void world::Sprite::setTextureMap(gl::TextureMap map)
{
    texture_map = map;
}

void world::Sprite::setDimensions(gl::Frame dim)
{
    dimensions = dim;
}
