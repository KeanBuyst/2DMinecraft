#pragma once

#include "Entity.h"
#include "../../gl/Frame.h"

namespace Procedure
{
    struct HitBoxResult
    {
        bool top;
        bool bottom;
        bool left;
        bool right;
        // translation vector to alter the next position so that it is not within the block
        glm::vec2 translation;
    };

    HitBoxResult HitBox(world::Entity* entity);
    void RigidBody(world::Entity* entity, const HitBoxResult& result);
}
