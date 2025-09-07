#pragma once

#include "Entity.h"
#include "../../Util.h"
#include "../../gl/Shader.h"

namespace EntityHandler
{

    extern Util::Buffer<world::Entity,128> buffer;

    void Init();
    void Cleanup();

    void Event(SDL_Event* event);
    void Update(const float& delta_time);
    void Render(gl::ShaderProgram* shader);

    void Add(world::Entity* entity);
}
