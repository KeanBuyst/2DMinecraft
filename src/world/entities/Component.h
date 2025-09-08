#pragma once

#include <SDL_events.h>

#include "../World.h"
#include "../../Util.h"
#include "../../math/Transform.h"

namespace world
{
    // forward declaration
    struct Entity;
    struct Item;

    enum ComponentType : uint8_t
    {
        SPRITE,
        RIGID_BODY,
        HITBOX,
        ANIMATION,
        ITEM_CONTAINER
    };

    struct Component : Transform
    {
        ComponentType type;
        Entity* entity = nullptr;

        Component(glm::vec2 position,ComponentType type);

        glm::vec2 getNetPosition() const;
        float getNetRotation() const;

        virtual ~Component() = default;
        [[nodiscard]] virtual Component* clone() const = 0;

        virtual void event(SDL_Event* event){}
        virtual void update(const float& delta_time){}
        virtual void render(){}
    };

    struct Sprite : Component
    {
        glm::vec4 texel{};
        float width,height;
        float lightLevel;

        bool flipped;

        Sprite(glm::vec4 dimensions,glm::vec4 textRect,float scale);

        [[nodiscard]] Component* clone() const override;
        void render() override;
        void update(const float& delta_time) override;
    };

    struct HitBox : Component
    {
        glm::vec4 offsets;

        glm::vec2 offset;

        bool top;
        bool bottom;
        bool left;
        bool right;
        bool collidedWithEntity;

        bool debug;

        HitBox(glm::vec4 offsets);

        [[nodiscard]] Component* clone() const override;
        void update(const float& delta_time) override;
        void render() override;
        [[nodiscard]] bool inBounds(const glm::vec2& point) const;
    };

    struct RigidBody : Component
    {
        static constexpr float gravity = 48.0f;
        static constexpr float drag = 2.4f;
        static constexpr float friction = 10.6f;
        static constexpr float max_speed = 30.0f;

        const HitBox* hitbox;

        bool moving;

        explicit RigidBody(const HitBox* hitbox);

        [[nodiscard]] Component* clone() const override;
        void update(const float& delta_time) override;
    };

    struct Animation : Component
    {
        Animation();

        void update(const float& delta_time) override;

        // Fixed rate at 30 frames per second
        virtual void nextFrame(int& current_frame) = 0;

    private:
        int current_frame;
        float accumulator;
    };

    struct BipedalAnimation : Animation
    {
        Sprite* l1;
        Sprite* l2;

        static constexpr float MAX_ANGLE = Util::DegToRad(35.0f); // 30 degrees

        float rot_dir;

        BipedalAnimation(Sprite* l1, Sprite* l2);
        [[nodiscard]] Component* clone() const override;

        void nextFrame(int& current_frame) override;
    };

    struct PlayerHeadAnimation : Animation
    {
        Sprite* head;

        [[nodiscard]] Component* clone() const override;
        PlayerHeadAnimation(Sprite* head);

        void nextFrame(int& current_frame) override;
    };

    struct ItemContainer : Component
    {
        // inventory handles item pointer, therefore don't delete this!
        Item* item;

        // item can be NULL
        ItemContainer(Sprite* arm,Item* item);
        [[nodiscard]] Component* clone() const override;
        void render() override;
    };
}
