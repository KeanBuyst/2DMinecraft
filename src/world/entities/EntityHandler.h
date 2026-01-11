#pragma once

#include "Entity.h"
#include "../../Util.h"
#include "../../gl/Shader.h"

namespace EntityHandler
{
    using EntityCreator = world::Entity* (*)(Util::ByteStream&);

    void Init();
    void Cleanup();

    void Update();
    void Render(SDL_GPUCommandBuffer* cmd,SDL_GPUTexture* swapChain, uint32_t width, uint32_t height);

    world::Entity* GetHead();

    void Register(world::EntityType type, EntityCreator creator);

    void Add(world::Entity* entity);
    world::Entity* Add(Util::ByteStream& stream);
}

// some MACRO magic
#define STR_CONCAT_IMPL(x, y) x##y
#define STR_CONCAT(x, y) STR_CONCAT_IMPL(x, y)

#define REGISTER_ENTITY(ENUM_VAL, CLASS_NAME) \
namespace { \
    static bool STR_CONCAT(_registered_at_line_, __LINE__) = []() -> bool { \
        EntityHandler::Register(ENUM_VAL, [](Util::ByteStream& stream) -> world::Entity* { \
             return new CLASS_NAME(stream); \
        }); \
        return true; \
        }(); \
    }
