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
    MATERIAL main_type = getType();
    if (main_type == EMPTY) main_type = getWall();
    switch (main_type)
    {
    case OAK_LEAVES:
    case GRASS:
        return true;
    }
    return false;
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

void Block::setWall(const MATERIAL wall)
{
    data = data & 0xFFFF00FF | wall << 8;
}

void Block::setType(const MATERIAL type)
{
    data = data & 0xFFFFFF00 | type;
}

uint32_t Block::getRaw() const
{
    return data;
}
