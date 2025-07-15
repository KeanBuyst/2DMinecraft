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
        float lightLevel;
    };

    // Render Batch
    extern std::vector<EntityRenderData> entity_render_batch;

    float GetMaxHealth(EntityType type);

    struct Entity : VectorTransform
    {
        EntityType type;
        Component** components;
        uint16_t id;
        int numOfComponents;
        float max_health;
        float health;
        float lightLevel;

        Entity(glm::vec2 position,EntityType type,int numOfComponents);
        ~Entity();

        void event(SDL_Event* event) const;
        void update(const float& delta_time);
        void render();

        template<typename T>
        [[nodiscard]] std::vector<T*> getComponents(const ComponentType compType) const
        {
            std::vector<T*> list;
            for (auto i = 0; i < numOfComponents; ++i)
            {
                if (components[i]->type == compType)
                    list.push_back(static_cast<T*>(components[i]));
            }
            return list;
        }

        void addComponents(Component** comps,int size = -1);
    };
}
