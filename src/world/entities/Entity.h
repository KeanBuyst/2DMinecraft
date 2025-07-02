#pragma once

#include <iostream>
#include <SDL_events.h>
#include <vector>

#include "Component.h"
#include "../../math/Transform.h"

namespace world
{
    enum EntityType : uint8_t
    {
        PLAYER
    };

    struct EntityRenderData
    {
        glm::vec2 vertex;
        glm::vec2 texel;
    };

    // Render Batch
    extern std::vector<EntityRenderData> entity_render_batch;

    struct Entity : Transform
    {
        EntityType type;
        Component** components;
        uint16_t id;
        int numOfComponents;
        float health = 1;

        Entity(glm::vec2 position,EntityType type,int numOfComponents);
        ~Entity();

        void event(SDL_Event* event) const;
        void update(const float& delta_time);
        void render();

        [[nodiscard]] Component* getComponent(ComponentType compType) const;

        void addComponents(Component** comps,int size = -1);
    };
}
