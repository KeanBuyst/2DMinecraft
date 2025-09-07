#include "Block.h"

#include "Region.h"

using namespace world;

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

void Block::setWall(const BlockType wall)
{
    data = data & 0xFFFF00FF | wall << 8;
}

void Block::setType(const BlockType type)
{
    data = data & 0xFFFFFF00 | type;
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
    if (isTransparent()) return -1;
    if (solid != EMPTY) return -3;
    if (wall != EMPTY) return -1;
    return DAYLIGHT;
}
