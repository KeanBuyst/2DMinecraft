#pragma once

#include "Entity.h"
#include "../../Util.h"

namespace EntityHandler
{
    extern Util::Buffer<world::Entity,128> buffer;

    void Init();
    void Destroy();

    void Event(SDL_Event* event);
    void Update(const float& delta_time);
    void Render();

    void Add(world::Entity* entity);
}
