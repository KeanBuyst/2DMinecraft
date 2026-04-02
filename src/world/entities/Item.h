#pragma once

#include "Entity.h"

#include <cstdint>

#include "Sprite.h"

namespace world
{
    enum class ToolType
    {
        HAND,
        PICKAXE,
        SWORD,
        SHOVEL,
        AXE
    };

    enum class ToolTier
    {
        NONE,
        WOOD,
        STONE,
        IRON,
        GOLD,
        DIAMOND
    };

    struct Tool
    {
        explicit Tool(ItemType type);
        Tool();

        ToolType getType() const;
        ToolTier getTier() const;
        float getDamage() const;

        float getToolBonus(BlockType) const;
        bool isApplicable(BlockType) const;
    private:
        ToolType type;
        ToolTier tier;
        float damage;
    };

    struct Material
    {
    private:
        uint8_t mat;
        bool block;
    public:
        static Material EMPTY;

        Material();
        Material(const ItemType& item);
        Material(const BlockType& block);

        Material& operator=(const ItemType& item);
        Material& operator=(const BlockType& item);

        operator ItemType() const;
        operator BlockType() const;

        bool operator==(const ItemType& item) const;
        bool operator==(const BlockType& item) const;
        bool operator==(const Material& material) const;

        bool operator!=(const ItemType& item) const;
        bool operator!=(const BlockType& item) const;
        bool operator!=(const Material& material) const;

        bool isBlock() const;
        bool isItem() const;
        bool isEmpty() const;

        int getRenderData() const;
        uint8_t getRaw() const;
    };

    class Item : public Entity
    {
    private:
        Sprite sprite;
        int amount;
    public:
        Material material;

        explicit Item(Util::ByteStream& stream);
        explicit Item(Material material);
        Item(glm::vec2 position,Material material);
        Item(const Item& other);

        void serialize(Util::ByteStream& stream) override;
        void render() override;
        void onCollision(Entity* other) override;
        Sprite& getSprite(int) override;

        Tool getTool() const;
        int getStackLimit() const;
        int getAmount() const;
        bool operator==(const Item& item) const;

        void setAmount(int amount);
        // returns true if other has been completely combined with the item
        // and thus its amount is 0
        bool combine(Item* other);
    };

    void RemoveAmount(Item*& item,int amount);
    Item* AddAmount(Item*& item,int amount);
}
