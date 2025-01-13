#include "Block.h"

using namespace world;

Block::Block(const MATERIAL type, const glm::vec2 position,const MATERIAL wall, const int luminance) : type(type),position(position),luminance(luminance),wall(wall)
{}

bool Block::isTransparent() const
{
    // TODO add exceptions (e.g. if type is GLASS)
    return wall != 0;
}
MATERIAL Block::getWall() const
{
    return wall;
}

bool Block::isEmpty() const
{
    return type == EMPTY && wall == EMPTY;
}
