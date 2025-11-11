#pragma once

#include "Entity.h"

#include <cstdint>

namespace world
{
    enum ItemType : uint8_t
    {
        WOODEN_PICKAXE,
        STONE_PICKAXE,
        IRON_PICKAXE,
        GOLD_PICKAXE,
        DIAMOND_PICKAXE,
        WOODEN_SWORD,
        STONE_SWORD,
        IRON_SWORD,
        GOLDEN_SWORD,
        DIAMOND_SWORD,
        WOODEN_SHOVEL,
        STONE_SHOVEL,
        IRON_SHOVEL,
        GOLDEN_SHOVEL,
        DIAMOND_SHOVEL,
        STICK,
        IRON_INGOT,
        GOLD_INGOT,
        DIAMOND,
        WOODEN_AXE,
        STONE_AXE,
        IRON_AXE,
        GOLDEN_AXE,
        DIAMOND_AXE
    };

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
        int amount;
    public:
        const Material material;

        explicit Item(Material material);
        Item(glm::vec2 position,Material material);
        Item(const Item& other);

        Tool getTool() const;
        int getStackLimit() const;

        void toComponent();
        void toEntity();

        int getAmount() const;
        void setAmount(int amount);
        bool operator==(const Item& item) const;
    };

    void RemoveAmount(Item*& item,int amount);
    Item* AddAmount(Item*& item,int amount);
}
