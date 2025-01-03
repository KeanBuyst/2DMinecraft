#include "Block.h"

using namespace world;

Block::Block(const MATERIAL type, const glm::ivec2 position,const Layer layer,const uint8_t luminance) : type(type),position(position),layer(layer),luminance(luminance)
{}