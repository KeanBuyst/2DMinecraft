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

    struct Item : Entity
    {
        const uint8_t material;
        const bool isBlock;

        Item(glm::vec2 position,BlockType material);
        Item(glm::vec2 position,ItemType material);
        Item(const Item& other);

        void toComponent();
        void toEntity();

        bool isMaterial(BlockType mat) const;
        bool isMaterial(ItemType mat) const;

        int getAmount() const;
        void setAmount(int amount);

    private:
        int amount;
    };
}
