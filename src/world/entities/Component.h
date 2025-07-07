#pragma once

#include <SDL_events.h>

#include "../World.h"
#include "../../gl/Texture.h"
#include "../../math/Transform.h"

namespace world
{
    enum ComponentType : uint8_t
    {
        SPRITE,
        RIGID_BODY,
        HITBOX,
    };

    struct Component : Transform
    {
        ComponentType type;
        Transform* parent = nullptr;

        Component(glm::vec2 position,ComponentType type);

        virtual ~Component() = default;

        virtual void event(SDL_Event* event){}
        virtual void update(const float& delta_time){}
        virtual void render(){}
    };

    struct Sprite : Component
    {
        glm::vec4 texel{};
        float width,height;

        Sprite(glm::vec4 position,glm::vec4 textRect,float scale);

        void render() override;
    };

    struct HitBox : Component
    {
        glm::vec4 offsets;
        const World* world;

        glm::vec2 target;

        bool top;
        bool bottom;
        bool left;
        bool right;
        bool collidedWithEntity;

        bool debug;

        HitBox(const World* world,glm::vec4 offsets);

        void update(const float& delta_time) override;
        void render() override;
        [[nodiscard]] bool inBounds(const glm::vec2& point) const;
    };

    struct RigidBody : Component
    {
        static constexpr float gravity = 48.0f;
        static constexpr float drag = 2.4f;
        static constexpr float friction = 10.6f;

        const HitBox* hitbox;

        bool moving;

        explicit RigidBody(const HitBox* hitbox);

        void update(const float& delta_time) override;
    };
}
