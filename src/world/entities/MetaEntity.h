#pragma once
#include "Entity.h"
#include "../../ui/Inventory.h"

namespace world
{
    class MetaEntity : public Entity
    {
    protected:
        UI::Inventory** inventories;
        int numOfInventories;
    public:
        MetaEntity(EntityType type,gl::Frame dimensions,Util::ByteStream& stream);
        MetaEntity(glm::vec2 position,gl::Frame dimensions,EntityType type);
        ~MetaEntity() override;

        void serialize(Util::ByteStream& stream) override;

        UI::Inventory& getInventory(int index);
        int getNumberOfInventories();
    };
}
