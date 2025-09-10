#include "Component.h"

#include "Entity.h"
#include "EntityHandler.h"
#include "Item.h"
#include "../../Application.h"
#include "../../Constants.h"
#include "../../Util.h"

world::Component::Component(const glm::vec2 position,const ComponentType type) : Transform(position), type(type)
{}

glm::vec2 world::Component::getNetPosition() const
{
    return entity->position + position;
}

float world::Component::getNetRotation() const
{
    return entity->rotation + rotation;
}

world::Component* world::Sprite::clone() const
{
    return new Sprite(*this);
}

void world::Sprite::render()
{
    const float p1x = position.x - width / 2 - pivot_point.x;
    const float p1y = position.y - height / 2 - pivot_point.y;
    const float p2x = p1x + width;
    const float p2y = p1y + height;

    const float rot = flipped ? -getNetRotation() : getNetRotation();

    glm::vec2 bottomLeft = entity->position + pivot_point + Util::rotate({p1x, p1y}, rot);
    glm::vec2 topRight = entity->position + pivot_point + Util::rotate({p2x, p2y}, rot);
    glm::vec2 topLeft = entity->position + pivot_point + Util::rotate({p1x, p2y}, rot);
    glm::vec2 bottomRight = entity->position + pivot_point + Util::rotate({p2x, p1y}, rot);

    // light calculation
    float
    light_BL, light_TL,
    light_TR, light_BR;

    const glm::vec2 b_BL = glm::floor(bottomLeft);
    const glm::vec2 b_TR = glm::floor(topRight);
    const glm::vec2 b_TL = glm::floor(topLeft);
    const glm::vec2 b_BR = glm::floor(bottomRight);

    // bottom left
    light_BL = static_cast<float>(m_world.getBlock(b_BL).getLightLevel());

    // top right
    if (b_TR == b_BL) light_TR = light_BL;
    else light_TR = static_cast<float>(m_world.getBlock(b_TR).getLightLevel());

    // top left
    if (b_TL == b_TR) light_TL = light_TR;
    else if (b_TL == b_BL) light_TL = light_BL;
    else light_TL = static_cast<float>(m_world.getBlock(b_TL).getLightLevel());

    // bottom right
    if (b_BR == b_BL) light_BR = light_BL;
    else if (b_BR == b_TR) light_BR = light_TR;
    else if (b_BR == b_TL) light_BR = light_TL;
    else light_BR = static_cast<float>(m_world.getBlock(b_BR).getLightLevel());


    const glm::vec2 texBottomLeft = {flipped ? texel.z : texel.x, texel.w};
    const glm::vec2 texBottomRight = {flipped ? texel.x : texel.z, texel.w};
    const glm::vec2 texTopRight = {flipped ? texel.x : texel.z, texel.y};
    const glm::vec2 texTopLeft = {flipped ? texel.z : texel.x, texel.y};

    bottomLeft = (bottomLeft - origin) * PIXEL_SCALE;
    topRight = (topRight - origin) * PIXEL_SCALE;
    topLeft = (topLeft - origin) * PIXEL_SCALE;
    bottomRight = (bottomRight - origin) * PIXEL_SCALE;

    // Insert two triangles (6 vertices)
    std::initializer_list<EntityHandler::EntityRenderData> data = {
        // First triangle
        {bottomLeft,  texBottomLeft,    light_BL},
        {bottomRight, texBottomRight,   light_BR},
        {topRight,    texTopRight,      light_TR},

        // Second triangle
        {topRight,    texTopRight,      light_TR},
        {topLeft,     texTopLeft,       light_TL},
        {bottomLeft,  texBottomLeft,    light_BL}
    };

    if (entity->type == ITEM)
    {
        auto item = reinterpret_cast<Item*>(entity);
        if (item->isBlock)
            render_batch.insert<BlockType>(data);
        else
            render_batch.insert<ItemType>(data);
    }
    else
        render_batch.insert<EntityType>(data);
}

world::Sprite::Sprite(const glm::vec4 dimensions,glm::vec4 textRect,const float scale)
    : Component({dimensions.x / PIXEL_SCALE,dimensions.y / PIXEL_SCALE},SPRITE), flipped(false)
{
    width = (dimensions.z / PIXEL_SCALE) * scale;
    height = (dimensions.w / PIXEL_SCALE) * scale;

    textRect.z += textRect.x;
    textRect.w += textRect.y;

    texel = textRect;
}

void world::Sprite::update(const float& delta_time)
{
    if (entity->velocity.x < 0)
    {
        flipped = true;
    }
    else if (entity->velocity.x != 0)
    {
        flipped = false;
    }
}

world::HitBox::HitBox(const glm::vec4 offsets)
    : Component({0,0},HITBOX), offsets(offsets / PIXEL_SCALE), top(false),
    bottom(false), left(false), right(false), debug(false)
{}

void world::HitBox::render()
{
    if (debug)
    {
        const glm::vec2 p1(entity->position.x + offsets.x, entity->position.y + offsets.y); // Top Left
        const glm::vec2 p2(entity->position.x + offsets.z, entity->position.y + offsets.y); // Top Right
        const glm::vec2 p3(entity->position.x + offsets.x, entity->position.y + offsets.w); // Bottom Left
        const glm::vec2 p4(entity->position.x + offsets.z, entity->position.y + offsets.w); // Bottom Right

        const glm::vec2 points[] = {p1,p3,p4,p2};

        Util::drawDebugLines(points,4);
    }
}

world::Component* world::BipedalAnimation::clone() const
{
    return new BipedalAnimation(*this);
}

world::Component* world::PlayerHeadAnimation::clone() const
{
    return new PlayerHeadAnimation(*this);
}

world::Component* world::HitBox::clone() const
{
    return new HitBox(*this);
}

void world::HitBox::update(const float& delta_time)
{
    // reset all booleans
    top = false;
    bottom = false;
    left = false;
    right = false;

    const glm::vec2 pos = entity->position + entity->velocity * delta_time;

    const glm::vec2 top_p(pos.x, pos.y + offsets.y); // Top
    const glm::vec2 bottom_p(pos.x, pos.y + offsets.w); // Bottom
    const glm::vec2 left_p(pos.x + offsets.x, pos.y - 0.5f); // Left
    const glm::vec2 right_p(pos.x + offsets.z, pos.y - 0.5f); // Right

    // Tile/Block Collision
    // for point 1
    const Block b1 = m_world.getBlock(top_p);
    if (b1.isCollidable())
    {
        offset.y = b1.position.y - top_p.y;
        top = true;
    }
    const Block b2 = m_world.getBlock(right_p);
    if (b2.isCollidable())
    {
        // using pos instead of right_p prevents kick back from colliding with walls
        offset.x = b2.position.x - right_p.x;
        right = true;
    }
    const Block b3 = m_world.getBlock(left_p);
    if (b3.isCollidable())
    {
        offset.x = (b3.position.x + 1.0f) - left_p.x;
        left = true;
    }
    const Block b4 = m_world.getBlock(bottom_p);
    if (b4.isCollidable())
    {
        offset.y = (b4.position.y + 1.0f) - bottom_p.y;
        bottom = true;
    }

    // TODO entity collision
}

bool world::HitBox::inBounds(const glm::vec2& point) const
{
    const float top = entity->position.y + offsets.y;
    const float bottom = entity->position.y + offsets.w;
    const float left = entity->position.x + offsets.x;
    const float right = entity->position.x + offsets.z;

    return point.x >= left && point.x <= right && point.y >= bottom && point.y <= top;
}

world::RigidBody::RigidBody(const HitBox* hitbox) : Component({0,0},RIGID_BODY), hitbox(hitbox), moving(false)
{}

world::Component* world::RigidBody::clone() const
{
    return new RigidBody(*this);
}

void world::RigidBody::update(const float& delta_time)
{
    if (!moving) Util::decreaseMagnitude(entity->velocity,(hitbox->bottom ? friction : drag) * delta_time);
    if (hitbox->bottom && hitbox->top && hitbox->left && hitbox->right)
    {
        entity->velocity = {0,0};
    }
    if (hitbox->bottom)
    {
        if (entity->velocity.y < 0.0f) entity->velocity.y = hitbox->offset.y;
    } else
    {
        entity->acceleration.y = -gravity;
    }
    if (hitbox->top)
    {
        if (entity->velocity.y > 0.0f) entity->velocity.y = hitbox->offset.y;
    }
    if (hitbox->right)
    {
        if (entity->velocity.x > 0.0f)
        {
            entity->position.x += hitbox->offset.x;
            entity->velocity.x = 0;
        }
        if (entity->acceleration.x > 0.0f)
        {
            entity->acceleration.x = 0;
        }
    }
    if (hitbox->left)
    {
        if (entity->velocity.x < 0.0f)
        {
            entity->position.x += hitbox->offset.x;
            entity->velocity.x = 0;
        }
        if (entity->acceleration.x < 0.0f)
        {
            entity->acceleration.x = 0;
        }
    }
    // check speed limit
    Util::clamp(entity->velocity,max_speed);
}

world::Animation::Animation() : Component({0,0},ANIMATION), current_frame(0), accumulator(0.0f)
{}

void world::Animation::update(const float& delta_time)
{
    // 20 fps
    static constexpr float interval = 1.0f / 20.0f;

    accumulator += delta_time;

    if (accumulator >= interval)
    {
        nextFrame(++current_frame);
        accumulator -= interval;
    }
}

world::BipedalAnimation::BipedalAnimation(Sprite* l1, Sprite* l2) : l1(l1), l2(l2)
{
    const glm::vec2 pivot(0, -0.4f);
    l1->pivot_point = pivot;
    l2->pivot_point = pivot;
    rot_dir = -1;
}

void world::BipedalAnimation::nextFrame(int& current_frame)
{
    float ratio = abs(entity->velocity.x) / RigidBody::max_speed;
    float angle_ratio = ratio * 3.0f;
    float speed_ratio = ratio / 4.0f;
    float velocity = rot_dir * speed_ratio;

    if (velocity == 0.0f)
    {
        l1->rotation = 0.0f;
        l2->rotation = 0.0f;
    } else
    {
        l1->rotation += velocity;
        l2->rotation -= velocity;
    }
    float angle = MAX_ANGLE * angle_ratio;
    if (l1->rotation > angle)
    {
        rot_dir = -1;
        l1->rotation = angle;
        l2->rotation = -angle;
    } else if (l1->rotation < -angle)
    {
        rot_dir = 1;
        l1->rotation = -angle;
        l2->rotation = angle;
    }
}

world::PlayerHeadAnimation::PlayerHeadAnimation(Sprite* head) : head(head)
{
    head->pivot_point = {0,head->position.y};
}

void world::PlayerHeadAnimation::nextFrame(int& current_frame)
{
    const glm::vec2 mousePos = Application::GetWorldMouse();
    float rotation = atan2f(mousePos.y - (head->entity->position.y + head->position.y),
        mousePos.x - (head->entity->position.x + head->position.x));

    static constexpr float MAX_ANGLE = Util::DegToRad(40.0f);
    static constexpr float MID_ANGLE = Util::DegToRad(90.0f);

    if (rotation > MID_ANGLE) rotation = M_PI - rotation;
    if (rotation < -MID_ANGLE) rotation = -static_cast<float>(M_PI) - rotation;

    Util::clamp(rotation,MAX_ANGLE);

    head->rotation = rotation;
}

world::ItemContainer::ItemContainer(Sprite* arm, UI::Hotbar* inv)
    : Component(arm->position,ITEM_CONTAINER), inv(inv), ARM_ANGLE(Util::DegToRad(30.0f)), arm(arm)
{
    arm->pivot_point = {0,0.25};
}

world::Component* world::ItemContainer::clone() const
{
    return new ItemContainer(*this);
}

void world::ItemContainer::render()
{

    Item*& item = inv->getSelectedItem();
    if (item == nullptr)
    {
        arm->rotation = 0.0f;
        return;
    }
    arm->rotation = ARM_ANGLE;
    rotation = ARM_ANGLE;
    float x = 0.5f;
    float y = -0.3f;
    if (!item->isBlock)
    {
        rotation -= Util::DegToRad(30.0f);
        x = 0.6f;
        y = 0.15f;
    }
    position = Util::rotate(arm->position + pivot_point + glm::vec2(arm->flipped ? -x : x,y), arm->flipped ? -rotation : rotation);

    item->position = getNetPosition();
    item->rotation = getNetRotation();
    item->getComponent<Sprite>(SPRITE)->flipped = arm->flipped;
    item->render();
}

void world::ItemContainer::update(const float& delta_time)
{
    // dropping item
    if (Application::isKeyPressed(SDL_SCANCODE_Q))
    {
        Item*& item = inv->getSelectedItem();
        if (item != nullptr)
        {
            if (item->getAmount() > 1)
            {
                item->setAmount(item->getAmount() - 1);
                auto* newItem = new Item(*item);
                newItem->setAmount(1);
                newItem->position = getNetPosition();
                newItem->velocity = position * 10.0f;
                newItem->toEntity();
                EntityHandler::Add(newItem);
            } else
            {
                // pass ownership over to the entity handler
                item->position = getNetPosition();
                item->rotation = 0;
                item->velocity = position * 10.0f;
                item->toEntity();
                EntityHandler::Add(item);
                item = nullptr;
            }
        }
    }
}
