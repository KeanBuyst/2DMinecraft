#include "Block.h"

using namespace world;

Block::Block(const MATERIAL type, const glm::vec2 position,const MATERIAL wall,const uint8_t luminance) : type(type),position(position),wall(wall),luminance(luminance)
{}

bool Block::isTransparent()
{
    return true;
}
MATERIAL Block::getWall() const
{
    return wall;
}
