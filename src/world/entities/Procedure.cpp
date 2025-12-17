#include "Procedure.h"

#include "../World.h"
#include "../../Util.h"

using namespace world;

Procedure::HitBoxResult Procedure::HitBox(const Entity* entity, const gl::Frame& dimensions)
{
    HitBoxResult result = {
        false,
        false,
        false,
        false,
        {0.0f,0.0f}
    };

    const glm::vec2 pos = entity->position + entity->velocity * delta_time;

    // xy zw
    // left top | right bottom
    const float shift = (dimensions.right - dimensions.left + dimensions.top - dimensions.bottom) / 10.0f;
    // Check Vertical line left
    for (float y = dimensions.bottom + shift; y < dimensions.top; y += 1.0f)
    {
        glm::vec2 left_p(pos.x + dimensions.left,pos.y + y);
        const Block block = m_world.getBlock(left_p);
        if (block.isCollidable())
        {
            result.translation.x = (block.position.x + 1.0f) - left_p.x - (1.0f / PIXEL_SCALE);
            result.left = true;
        }
    }

    // Check Vertical line right
    for (float y = dimensions.bottom + shift; y < dimensions.top; y += 1.0f)
    {
        glm::vec2 right_p(pos.x + dimensions.right,pos.y + y);
        const Block block = m_world.getBlock(right_p);
        if (block.isCollidable())
        {
            // using pos instead of right_p prevents kick back from colliding with walls
            result.translation.x = block.position.x - right_p.x + (1.0f / PIXEL_SCALE);
            result.right = true;
        }
    }

    // Check Horizontal line top
    for (float x = dimensions.left + shift; x < dimensions.right; x += 1.0f)
    {
        glm::vec2 top_p(pos.x + x,pos.y + dimensions.top);
        const Block block = m_world.getBlock(top_p);
        if (block.isCollidable())
        {
            result.translation.y = block.position.y - top_p.y + (1.0f / PIXEL_SCALE);
            result.top = true;
        }
    }

    // Check Horizontal line bottom
    for (float x = dimensions.left + shift; x < dimensions.right; x += 1.0f)
    {
        glm::vec2 bottom_p(pos.x + x,pos.y + dimensions.bottom);
        const Block block = m_world.getBlock(bottom_p);
        if (block.isCollidable())
        {
            // apply pixel shift 1 / 16 = 0.0625 = 1 pixel
            result.translation.y = ((block.position.y + 1.0f) - bottom_p.y) - (1.0f / PIXEL_SCALE);
            result.bottom = true;
        }
    }

    return result;
}

void Procedure::RigidBody(Entity* entity, const HitBoxResult& result)
{
    // Under low speeds offsetting is not required and prevents weird behaviour due to offsetting.
    // But if speeds are higher than then pixel perfect checking the entity will enter blocks
    // Thus if higher speeds are required introduce a hybrid system.
    // Under low speeds no offsets. High speeds. Enable offsets
    if (!entity->hasAppliedMovement()) Util::decreaseMagnitude(entity->velocity,(result.bottom ? FRICTION : DRAG) * delta_time);
    if (result.bottom && result.top && result.left && result.right)
    {
        entity->velocity = {0,0};
    }
    if (result.bottom)
    {
        if (entity->velocity.y < 0.0f)
        {
            entity->position.y += result.translation.y;
            entity->velocity.y = 0;
        }
    } else
    {
        entity->acceleration.y = -GRAVITY;
    }
    if (result.top)
    {
        if (entity->velocity.y > 0.0f)
        {
            entity->position.y += result.translation.y;
            entity->velocity.y = 0;
        }
    }
    if (result.right)
    {
        if (entity->velocity.x > 0.0f)
        {
            entity->position.x += result.translation.x;
            entity->velocity.x = 0;
        }
        if (entity->acceleration.x > 0.0f)
        {
            entity->acceleration.x = 0;
        }
    }
    if (result.left)
    {
        if (entity->velocity.x < 0.0f)
        {
            entity->position.x += result.translation.x;
            entity->velocity.x = 0;
        }
        if (entity->acceleration.x < 0.0f)
        {
            entity->acceleration.x = 0;
        }
    }
    // check speed limit
    Util::clamp(entity->velocity,MAX_SPEED);
}