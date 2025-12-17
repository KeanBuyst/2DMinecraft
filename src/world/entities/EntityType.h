#pragma once
#include <cstdint>

namespace world
{
    enum class EntityType : uint8_t
    {
        PLAYER,
        ITEM,

        COUNT
    };

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
}
