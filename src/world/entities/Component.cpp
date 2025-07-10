#include "Component.h"

#include "Entity.h"
#include "EntityHandler.h"
#include "../../Application.h"
#include "../../Constants.h"
#include "../../Util.h"

void world::Sprite::render()
{
    const glm::vec2 base = parent->position - origin;

    const float p1x = position.x - width / 2 - pivot_point.x;
    const float p1y = position.y - height / 2 - pivot_point.y;
    const float p2x = p1x + width;
    const float p2y = p1y + height;

    const float rot = parent->rotation + rotation;

    const glm::vec2 bottomLeft = (base + pivot_point + Util::rotate({p1x, p1y}, rot)) * PIXEL_SCALE;
    const glm::vec2 topRight = (base + pivot_point + Util::rotate({p2x, p2y}, rot)) * PIXEL_SCALE;
    const glm::vec2 topLeft = (base + pivot_point + Util::rotate({p1x, p2y}, rot)) * PIXEL_SCALE;
    const glm::vec2 bottomRight = (base + pivot_point + Util::rotate({p2x, p1y}, rot)) * PIXEL_SCALE;

    const glm::vec2 texBottomLeft = {flipped ? texel.z : texel.x, texel.w};
    const glm::vec2 texBottomRight = {flipped ? texel.x : texel.z, texel.w};
    const glm::vec2 texTopRight = {flipped ? texel.x : texel.z, texel.y};
    const glm::vec2 texTopLeft = {flipped ? texel.z : texel.x, texel.y};

    // Insert two triangles (6 vertices)
    entity_render_batch.insert(entity_render_batch.end(), {
        // First triangle
        {bottomLeft,  texBottomLeft},
        {bottomRight, texBottomRight},
        {topRight,    texTopRight},

        // Second triangle
        {topRight,    texTopRight},
        {topLeft,     texTopLeft},
        {bottomLeft,  texBottomLeft}
    });
}

world::Component::Component(const glm::vec2 position,const ComponentType type) : Transform(position), type(type)
{}

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
    if (parent->velocity.x < 0)
    {
        flipped = true;
    }
    else if (parent->velocity.x != 0)
    {
        flipped = false;
    }
}

world::HitBox::HitBox(const World* world, const glm::vec4 offsets)
    : Component({0,0},HITBOX), offsets(offsets / PIXEL_SCALE), world(world), top(false),
    bottom(false), left(false), right(false), debug(false)
{}

void world::HitBox::render()
{
    if (debug)
    {
        const glm::vec2 p1(parent->position.x + offsets.x, parent->position.y + offsets.y); // Top Left
        const glm::vec2 p2(parent->position.x + offsets.z, parent->position.y + offsets.y); // Top Right
        const glm::vec2 p3(parent->position.x + offsets.x, parent->position.y + offsets.w); // Bottom Left
        const glm::vec2 p4(parent->position.x + offsets.z, parent->position.y + offsets.w); // Bottom Right

        const glm::vec2 points[] = {p1,p3,p4,p2};

        Util::drawDebugLines(points,4);
    }
}

void world::HitBox::update(const float& delta_time)
{
    // reset all booleans
    top = false;
    bottom = false;
    left = false;
    right = false;

    const glm::vec2 pos = parent->position + parent->velocity * delta_time;

    const glm::vec2 top_p(pos.x, pos.y + offsets.y); // Top
    const glm::vec2 bottom_p(pos.x, pos.y + offsets.w); // Bottom
    const glm::vec2 left_p(pos.x + offsets.x, pos.y - 0.5f); // Left
    const glm::vec2 right_p(pos.x + offsets.z, pos.y - 0.5f); // Right

    // Tile/Block Collision
    // for point 1
    const Block b1 = world->getBlock(top_p);
    if (b1.getType() != EMPTY)
    {
        target.y = b1.position.y - top_p.y;
        top = true;
    }
    const Block b2 = world->getBlock(right_p);
    if (b2.getType() != EMPTY)
    {
        // using pos instead of right_p prevents kick back from colliding with walls
        target.x = b2.position.x - right_p.x;
        right = true;
    }
    const Block b3 = world->getBlock(left_p);
    if (b3.getType() != EMPTY)
    {
        target.x = (b3.position.x + 1.0f) - left_p.x;
        left = true;
    }
    const Block b4 = world->getBlock(bottom_p);
    if (b4.getType() != EMPTY)
    {
        target.y = (b4.position.y + 1.0f) - bottom_p.y;
        bottom = true;
    }

    // TODO entity collision
}

bool world::HitBox::inBounds(const glm::vec2& point) const
{
    const float top = parent->position.y + offsets.y;
    const float bottom = parent->position.y + offsets.w;
    const float left = parent->position.x + offsets.x;
    const float right = parent->position.x + offsets.z;

    return point.x >= left && point.x <= right && point.y >= bottom && point.y <= top;
}

world::RigidBody::RigidBody(const HitBox* hitbox) : Component({0,0},RIGID_BODY), hitbox(hitbox), moving(false)
{}

void world::RigidBody::update(const float& delta_time)
{
    if (!moving) Util::decreaseMagnitude(parent->velocity,(hitbox->bottom ? friction : drag) * delta_time);
    if (hitbox->bottom && hitbox->top && hitbox->left && hitbox->right)
    {
        parent->velocity = {0,0};
    }
    if (hitbox->bottom)
    {
        if (parent->velocity.y < 0.0f) parent->velocity.y = hitbox->target.y;
    } else
    {
        parent->acceleration.y = -gravity;
    }
    if (hitbox->top)
    {
        if (parent->velocity.y > 0.0f) parent->velocity.y = hitbox->target.y;
    }
    if (hitbox->right)
    {
        if (parent->velocity.x > 0.0f)
        {
            parent->position.x += hitbox->target.x;
            parent->velocity.x = 0;
        }
        if (parent->acceleration.x > 0.0f)
        {
            parent->acceleration.x = 0;
        }
    }
    if (hitbox->left)
    {
        if (parent->velocity.x < 0.0f)
        {
            parent->position.x += hitbox->target.x;
            parent->velocity.x = 0;
        }
        if (parent->acceleration.x < 0.0f)
        {
            parent->acceleration.x = 0;
        }
    }
    // check speed limit
    Util::clamp(parent->velocity,max_speed);
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

world::LegAnimation::LegAnimation(Sprite* l1, Sprite* l2) : l1(l1), l2(l2)
{
    const glm::vec2 pivot(0, -0.4f);
    l1->pivot_point = pivot;
    l2->pivot_point = pivot;
    rot_dir = -1;
}

void world::LegAnimation::nextFrame(int& current_frame)
{
    float ratio = abs(parent->velocity.x) / RigidBody::max_speed;
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
    float rotation = atan2f(mousePos.y - (head->parent->position.y + head->position.y),
        mousePos.x - (head->parent->position.x + head->position.x));

    static constexpr float MAX_ANGLE = Util::DegToRad(40.0f);
    static constexpr float MID_ANGLE = Util::DegToRad(90.0f);

    if (rotation > MID_ANGLE) rotation = M_PI - rotation;
    if (rotation < -MID_ANGLE) rotation = -static_cast<float>(M_PI) - rotation;

    if (head->flipped)
    {
        rotation = -rotation;
    }

    Util::clamp(rotation,MAX_ANGLE);

    head->rotation = rotation;
}
