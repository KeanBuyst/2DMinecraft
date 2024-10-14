#include "Block.h"

using namespace world;

Block::Block(MATERIAL type, glm::ivec2 postion) : type(type), pos(postion)
{

}

MetaBlock::MetaBlock(MATERIAL type, glm::ivec2 postion) : Block(type,postion)
{

}

void MetaBlock::addMeta(Metadata data)
{

}