#include "Item.h"

#include "EntityHandler.h"
#include "../../gl/Texture.h"

world::Tool::Tool()
{
    type = ToolType::HAND;
    tier = ToolTier::NONE;
    damage = 1.0f;
}

world::Tool::Tool(ItemType t)
{
    switch (t)
    {
    case WOODEN_PICKAXE:
        type = ToolType::PICKAXE;
        tier = ToolTier::WOOD;
        damage = 2.0f;
        break;
    case STONE_PICKAXE:
        type = ToolType::PICKAXE;
        tier = ToolTier::STONE;
        damage = 3.0f;
        break;
    case IRON_PICKAXE:
        type = ToolType::PICKAXE;
        tier = ToolTier::IRON;
        damage = 4.0f;
        break;
    case GOLD_PICKAXE:
        type = ToolType::PICKAXE;
        tier = ToolTier::GOLD;
        damage = 2.0f;
        break;
    case DIAMOND_PICKAXE:
        type = ToolType::PICKAXE;
        tier = ToolTier::DIAMOND;
        damage = 5.0f;
        break;

    case WOODEN_SWORD:
        type = ToolType::SWORD;
        tier = ToolTier::WOOD;
        damage = 4.0f;
        break;
    case STONE_SWORD:
        type = ToolType::SWORD;
        tier = ToolTier::STONE;
        damage = 5.0f;
        break;
    case IRON_SWORD:
        type = ToolType::SWORD;
        tier = ToolTier::IRON;
        damage = 6.0f;
        break;
    case GOLDEN_SWORD:
        type = ToolType::SWORD;
        tier = ToolTier::GOLD;
        damage = 4.0f;
        break;
    case DIAMOND_SWORD:
        type = ToolType::SWORD;
        tier = ToolTier::DIAMOND;
        damage = 7.0f;
        break;

    case WOODEN_SHOVEL:
        type = ToolType::SHOVEL;
        tier = ToolTier::WOOD;
        damage = 2.5f;
        break;
    case STONE_SHOVEL:
        type = ToolType::SHOVEL;
        tier = ToolTier::STONE;
        damage = 3.5f;
        break;
    case IRON_SHOVEL:
        type = ToolType::SHOVEL;
        tier = ToolTier::IRON;
        damage = 4.5f;
        break;
    case GOLDEN_SHOVEL:
        type = ToolType::SHOVEL;
        tier = ToolTier::GOLD;
        damage = 2.5f;
        break;
    case DIAMOND_SHOVEL:
        type = ToolType::SHOVEL;
        tier = ToolTier::DIAMOND;
        damage = 5.5f;
        break;

    case WOODEN_AXE:
        type = ToolType::AXE;
        tier = ToolTier::WOOD;
        damage = 7.0f;
        break;
    case STONE_AXE:
        type = ToolType::AXE;
        tier = ToolTier::STONE;
        damage = 9.0f;
        break;
    case IRON_AXE:
        type = ToolType::AXE;
        tier = ToolTier::IRON;
        damage = 9.0f;
        break;
    case GOLDEN_AXE:
        type = ToolType::AXE;
        tier = ToolTier::GOLD;
        damage = 7.0f;
        break;
    case DIAMOND_AXE:
        type = ToolType::AXE;
        tier = ToolTier::DIAMOND;
        damage = 9.0f;
        break;

    default:
        type = ToolType::HAND;
        tier = ToolTier::NONE;
        damage = 1.0f;
        break;
    }
}

world::ToolType world::Tool::getType() const
{
    return type;
}

world::ToolTier world::Tool::getTier() const
{
    return tier;
}

float world::Tool::getDamage() const
{
    return damage;
}

float world::Tool::getToolBonus(BlockType t) const
{
    float bonus;
    if (isApplicable(t))
    {
        switch (tier)
        {
        case ToolTier::WOOD:
            bonus = 8.0f;
            break;
        case ToolTier::STONE:
            bonus = 12.0f;
            break;
        case ToolTier::IRON:
            bonus = 18.0f;
            break;
        case ToolTier::GOLD:
            bonus = 17.5f;
            break;
        case ToolTier::DIAMOND:
            bonus = 24.0f;
            break;
        default:
            bonus = 5.0f;
            break;
        }
    } else bonus = 5.0f;

    bonus -= GetToughness(t);
    if (bonus < 0.0f) bonus = 0.0f;

    return bonus;
}

bool world::Tool::isApplicable(const BlockType t) const
{
    switch (type)
    {
    case ToolType::PICKAXE:
        switch (t)
        {
            case STONE:
            case IRON_ORE:
            case COAL_ORE:
            case DIAMOND_ORE:
            case GOLD_ORE:
            case COPPER_ORE:
            case LAPIS_ORE:
            case EMERALD_ORE:
            case REDSTONE_ORE:
            case STONE_WALL:
                    return true;
            default:
                    return false;
        }

    case ToolType::AXE:
        switch (t)
        {
            case OAK_LOG:
                    return true;
            default:
                    return false;
        }

    case ToolType::SHOVEL:
        switch (t)
        {
            case DIRT:
            case GRASS_BLOCK:
            case GRASS:
                    return true;
            default:
                    return false;
        }

    case ToolType::SWORD:
        switch (t)
        {
            case OAK_LEAVES:
                    return true;
            default:
                    return false;
        }

    case ToolType::HAND:
    default:
        return false;
    }
}

world::Material::Material() : mat(0), block(true)
{}

world::Material::Material(const ItemType& item) : mat(item), block(false)
{}

world::Material::Material(const BlockType& block) : mat(block), block(true)
{}

world::Material& world::Material::operator=(const ItemType& item)
{
    mat = item;
    block = false;
    return *this;
}

world::Material& world::Material::operator=(const BlockType& item)
{
    mat = item;
    block = true;
    return *this;
}

world::Material::operator ItemType() const
{
    return static_cast<ItemType>(mat);
}

world::Material::operator BlockType() const
{
    return static_cast<BlockType>(mat);
}

bool world::Material::operator==(const ItemType& item) const
{
    if (block) return false;
    return mat == item;
}

bool world::Material::operator==(const BlockType& item) const
{
    if (!block) return false;
    return mat == item;
}

bool world::Material::operator==(const Material& material) const
{
    return block == material.block && mat == material.mat;
}

bool world::Material::operator!=(const ItemType& item) const
{
    return !this->operator==(item);
}

bool world::Material::operator!=(const BlockType& item) const
{
    return !this->operator==(item);
}

bool world::Material::operator!=(const Material& material) const
{
    return !this->operator==(material);
}

bool world::Material::isBlock() const
{
    return block;
}

bool world::Material::isItem() const
{
    return !block;
}

bool world::Material::isEmpty() const
{
    return block && mat == EMPTY;
}

int world::Material::getRenderData() const
{
    int item = mat;
    if (block)
    {
        if (item == 0) item = -1;
        else item += 255;
    }
    return item;
}

uint8_t world::Material::getRaw() const
{
    return mat;
}

world::Tool world::Item::getTool() const
{
    if (material.isBlock())
        return {};
    return Tool(material);
}

world::Item::Item(Material material) : Entity({0.0f,0.0f},ITEM), material(material), amount(1)
{
    Sprite* sprite;
    if (material.isBlock())
        sprite = new Sprite({0,0,8,8},gl::AtlasTexture::getTexel(material.getRaw() - 1),1.0f);
    else
        sprite = new Sprite({0,0,8,8},gl::AtlasTexture::getTexel(material.getRaw()),2.0f);

    auto** components = new Component*[]
    {
        sprite,
        nullptr,
        nullptr
    };
    addComponents(components,3);
}

world::Item::Item(glm::vec2 position, Material material) : Entity(position,ITEM), material(material), amount(1)
{
    Sprite* sprite;
    if (material.isBlock())
        sprite = new Sprite({0,0,8,8},gl::AtlasTexture::getTexel(material.getRaw() - 1),1.0f);
    else
        sprite = new Sprite({0,0,8,8},gl::AtlasTexture::getTexel(material.getRaw()),2.0f);

    auto* hitbox = new HitBox({-4,4,4,-4});
    auto* rigid = new RigidBody(hitbox);
    auto** components = new Component*[]
    {
        sprite,
        hitbox,
        rigid
    };
    addComponents(components,3);
    health = 1;
}

int world::Item::getStackLimit() const
{
    if (material.isBlock()) return 99;
    if (getTool().getType() != ToolType::HAND) return 1;
    return 99;
}

world::Item::Item(const Item& other)
    : Entity(other), material(other.material), amount(other.amount)
{}

void world::Item::toComponent()
{
    // remove unnecessary components
    for (auto i = 1; i < numOfComponents; ++i)
    {
        delete components[i];
        components[i] = nullptr;
    }
    health = 0;
}

void world::Item::toEntity()
{
    if (components[1] != nullptr || components[2] != nullptr) return;
    // add necessary components
    auto* hitbox = new HitBox({-4,4,4,-4});
    hitbox->entity = this;
    components[1] = hitbox;
    auto* rigid = new RigidBody(hitbox);
    rigid->entity = this;
    components[2] = rigid;
    health = 1;
}

int world::Item::getAmount() const
{
    return amount;
}

void world::Item::setAmount(const int amount)
{
    this->amount = amount;
}

bool world::Item::operator==(const Item& item) const
{
    return material == item.material;
}

void world::RemoveAmount(Item*& item, const int amount)
{
    if (item == nullptr) return;
    const int sum = item->getAmount() - amount;
    if (sum <= 0)
    {
        delete item;
        item = nullptr;
    }
    else
    {
        item->setAmount(sum);
    }
}

// only produces 1 overflow item
world::Item* world::AddAmount(Item*& item, const int amount)
{
    if (item == nullptr) return nullptr;
    int sum = item->getAmount() + amount;
    const int limit = item->getStackLimit();
    if (sum > limit)
    {
        assert(sum <= limit * 2);
        item->setAmount(limit);
        sum -= limit;
        auto* newItem = new Item(*item);
        newItem->setAmount(sum);
        return newItem;
    }
    item->setAmount(sum);
    return nullptr;
}