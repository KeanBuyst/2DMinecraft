#include "Block.h"

#include "Region.h"
#include "../Application.h"
#include "entities/Item.h"

using namespace world;

float world::GetToughness(const BlockType type)
{
    switch (type)
    {
    case GRASS_BLOCK:
    case DIRT:
        return 1.0f;

    case OAK_LOG:
        return 2.0f;

    case OAK_LEAVES:
        return 0.5f;

    case STONE:
    case STONE_WALL:
        return 5.0f;

    case COAL_ORE:
    case GOLD_ORE:
    case COPPER_ORE:
        return 5.5f;

    case IRON_ORE:
    case LAPIS_ORE:
        return 10.0f;

    case DIAMOND_ORE:
    case EMERALD_ORE:
    case REDSTONE_ORE:
        return 17.0f;

    default:
        return 0.0f;
    }
}

BlockType world::GetWallOf(const BlockType type)
{
    switch (type)
    {
        case GRASS_BLOCK:
        case DIRT:
            return DIRT;
        case STONE:
        case IRON_ORE:
        case GOLD_ORE:
        case COPPER_ORE:
        case COAL_ORE:
        case LAPIS_ORE:
        case DIAMOND_ORE:
        case EMERALD_ORE:
        case REDSTONE_ORE:
            return STONE_WALL;
        default:
            return EMPTY;
    }
}

Block::Block(const BlockType type, const glm::vec2 position,const BlockType wall) : position(position)
{
    data = type | wall << 8;
}

Block::Block(const glm::vec2 position,const uint32_t data) : position(position), data(data)
{}


bool Block::isTransparent() const
{
    BlockType main_type = getType();
    if (main_type == EMPTY) main_type = getWall();
    switch (main_type)
    {
    case OAK_LEAVES:
    case GRASS:
    case TORCH:
        return true;
    }
    return false;
}

bool Block::isCollidable() const
{
    if (isEmpty()) return false;
    const BlockType mat = getType();
    if (mat == EMPTY) return false;
    // exceptions
    switch (mat)
    {
    case TORCH:
        return false;
    }
    return true;
}

BlockType Block::getType() const
{
    return static_cast<BlockType>(data & 0xFFu);
}

BlockType Block::getWall() const
{
    return static_cast<BlockType>(data >> 8 & 0xFFu);
}

bool Block::isEmpty() const
{
    return (data & 0xFFFFu) == 0;
}

int Block::getLightLevel() const
{
    return static_cast<int>((data >> 16) & 0xFu);
}

int Block::getBreakState() const
{
    return static_cast<int>((data >> 24) & 0xFu);
}

void Block::setWall(const BlockType wall)
{
    data = data & 0xFFFF00FF | wall << 8;
}

void Block::setType(const BlockType type)
{
    data = data & 0xFFFFFF00 | type;
}

void Block::setBreakState(int state)
{
    assert(state >= 0 && state <= 10);
    data = data & 0x00FFFFFF | state << 24;
}

uint32_t Block::getRaw() const
{
    return data;
}

short EmitsLight(const BlockType mat)
{
    switch (mat)
    {
    case TORCH:
        return 15;
    default:
        return 0;
    }
}

short Block::getInterference() const
{
    const BlockType solid = getType();
    const short s_result = EmitsLight(solid);
    const BlockType wall = getWall();
    const short w_result = EmitsLight(wall);
    if (s_result != 0 || w_result != 0)
    {
        if (s_result > w_result)
            return s_result;
        return w_result;
    }
    if (isTransparent()) return 0;
    if (solid != EMPTY) return -3;
    if (wall != EMPTY) return -1;
    return DAYLIGHT;
}
