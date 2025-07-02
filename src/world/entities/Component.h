#pragma once

#include <SDL_events.h>

#include "../../gl/Texture.h"
#include "../../math/Transform.h"

namespace world
{
    enum ComponentType : uint8_t
    {
        SPRITE,
    };

    struct Component : Transform
    {
        ComponentType type;
        Transform* parent = nullptr;

        Component(glm::vec2 position,ComponentType type);

        virtual ~Component() {};

        virtual void event(SDL_Event* event) = 0;
        virtual void update(const float& delta_time) = 0;
        virtual void render() = 0;
    };

    struct SpriteComponent : Component
    {
        glm::vec4 texel;
        float width,height;

        SpriteComponent(glm::vec4 position,glm::vec4 textRect,float scale);

        void event(SDL_Event* event) override;
        void update(const float& delta_time) override;
        void render() override;
    };
}
