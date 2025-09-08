#pragma once

#include <iostream>
#include <SDL_events.h>
#include <vector>

#include "Component.h"
#include "../../math/Transform.h"

namespace world
{
    enum ItemType : uint8_t;

    enum EntityType : uint8_t
    {
        PLAYER,
        ITEM
    };
}

namespace EntityHandler
{
    struct EntityRenderData
    {
        glm::vec2 vertex;
        glm::vec2 texel;
        float lightLevel;
    };

    struct EntityRenderBatch
    {
        std::vector<EntityRenderData> tileBatch;
        std::vector<EntityRenderData> entityBatch;
        std::vector<EntityRenderData> itemBatch;

        template<typename type>
        void insert(std::initializer_list<EntityRenderData> data)
        {
            if (std::is_same_v<type,world::BlockType>)
            {
                tileBatch.insert(tileBatch.end(),data);
            } else if (std::is_same_v<type,world::EntityType>)
            {
                entityBatch.insert(entityBatch.end(),data);
            } else if (std::is_same_v<type,world::ItemType>)
            {
                itemBatch.insert(itemBatch.end(),data);
            } else
            {
                throw std::runtime_error("Invalid entity render batch type");
            }
        }

        size_t size() const;
        EntityRenderData* data();

        void clear();
    };
}

namespace world
{
    extern EntityHandler::EntityRenderBatch render_batch;

    float GetMaxHealth(EntityType type);

    struct Entity : VectorTransform
    {
        EntityType type;
        Component** components;
        int id = -1;
        int numOfComponents;
        float max_health;
        float health;

        Entity(glm::vec2 position,EntityType type);
        Entity(const Entity& other);
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

        void addComponents(Component** comps,int size);
    };
}