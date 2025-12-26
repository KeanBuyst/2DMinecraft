#pragma once

#include <iostream>
#include <SDL_events.h>
#include <vector>

#include "Sprite.h"
#include "../../math/Transform.h"
#include "../Block.h"
#include "../../Util.h"
#include "../../gl/Frame.h"
#include "EntityType.h"

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

    class Entity : public Transform
    {
    private:
        Entity* next;

    protected:
        uint64_t id;
        gl::Frame dimensions;
        float health;

        bool appliedMovement;
        uint8_t doDeque;
        float max_health;

    public:
        const EntityType type;
        glm::vec2 velocity;
        glm::vec2 acceleration;

        Entity(EntityType type,gl::Frame dimensions,Util::ByteStream& stream);
        Entity(glm::vec2 position,gl::Frame dimensions,EntityType type);
        virtual ~Entity() = default;

        virtual void update();
        virtual void render() = 0;
        virtual void onCollision(Entity* other) = 0;
        virtual void serialize(Util::ByteStream& stream);

        virtual Sprite& getSprite(int index) = 0;

        bool hasAppliedMovement() const;
        gl::Frame getDimensions() const;
        uint64_t getID() const;

        Entity* getNext();
        void setNext(Entity* entity);
        bool destroy();
        bool dead();
        void deque(bool soft = false);
        void requeue();
    };
}