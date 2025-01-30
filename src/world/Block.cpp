#include "Block.h"

using namespace world;

Block::Block(const MATERIAL type, const glm::vec2 position,const MATERIAL wall) : position(position)
{
    data = type | wall << 8;
}

Block::Block(const glm::vec2 position,const uint32_t data) : position(position), data(data)
{}


bool Block::isTransparent() const
{
    // TODO add exceptions (e.g. if type is GLASS)
    return getType() == 0 && getWall() != 0;
}

MATERIAL Block::getType() const
{
    return static_cast<MATERIAL>(data & 0xFFu);
}

MATERIAL Block::getWall() const
{
    return static_cast<MATERIAL>(data >> 8 & 0xFFu);
}

bool Block::isEmpty() const
{
    return (data & 0xFFFFu) == 0;
}

int Block::getLuminance() const
{
    return static_cast<int>((data >> 16) & 0xFFu);
}

uint32_t Block::getRaw() const
{
    return data;
}
