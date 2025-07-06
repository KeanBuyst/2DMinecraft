#include "Component.h"

#include "Entity.h"
#include "EntityHandler.h"
#include "../../Constants.h"
#include "../../Util.h"

void world::Sprite::render()
{
    const float p1x = ((parent->position.x + position.x - width / 2) - origin.x) * PIXEL_SCALE;
    const float p1y = ((parent->position.y + position.y - height / 2) - origin.y) * PIXEL_SCALE;
    const float p2x = p1x + width * PIXEL_SCALE;
    const float p2y = p1y + height * PIXEL_SCALE;

    float rot = parent->rotation + rotation;

    glm::vec2 bottomLeft = Util::rotate({p1x, p1y}, rot);
    glm::vec2 topRight = Util::rotate({p2x, p2y}, rot);
    glm::vec2 topLeft = Util::rotate({p1x, p2y}, rot);
    glm::vec2 bottomRight = Util::rotate({p2x, p1y}, rot);

    // Insert two triangles (6 vertices) with proper texture coordinates
    entity_render_batch.insert(entity_render_batch.end(), {
        // First triangle
        {bottomLeft,  {texel.x, texel.w}},  // Bottom-left corner
        {bottomRight, {texel.z, texel.w}}, // Bottom-right corner
        {topRight,    {texel.z, texel.y}}, // Top-right corner

        // Second triangle
        {topRight,    {texel.z, texel.y}}, // Top-right corner
        {topLeft,     {texel.x, texel.y}}, // Top-left corner
        {bottomLeft,  {texel.x, texel.w}}   // Bottom-left corner
    });
}

world::Component::Component(const glm::vec2 position,const ComponentType type) : Transform(position), type(type)
{}

world::Sprite::Sprite(const glm::vec4 dimensions,glm::vec4 textRect,const float scale)
    : Component({dimensions.x / PIXEL_SCALE,dimensions.y / PIXEL_SCALE},SPRITE)
{
    width = (dimensions.z / PIXEL_SCALE) * scale;
    height = (dimensions.w / PIXEL_SCALE) * scale;

    textRect.z += textRect.x;
    textRect.w += textRect.y;

    texel = textRect;
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

    const float mid_x = (offsets.x - offsets.z) / 2.0f;
    const float mid_y = (offsets.w - offsets.y) / 2.0f;

    const glm::vec2 pos = parent->position + parent->velocity * delta_time;

    const glm::vec2 top_p(pos.x + mid_x, pos.y + offsets.y); // Top
    const glm::vec2 bottom_p(pos.x + mid_x, pos.y + offsets.w); // Bottom
    const glm::vec2 left_p(pos.x + offsets.x, pos.y + mid_y); // Left
    const glm::vec2 right_p(pos.x + offsets.z, pos.y + mid_y); // Left

    // Tile/Block Collision
    // for point 1
    const Block b1 = world->getBlock(top_p);
    if (b1.getType() != EMPTY)
    {
        target.y = parent->position.y - floorf(top_p.y);
        top = true;
    }
    const Block b2 = world->getBlock(right_p);
    if (b2.getType() != EMPTY)
    {
        // using pos instead of right_p prevents kick back from colliding with walls
        target.x = parent->position.x - floorf(right_p.x);
        right = true;
    }
    const Block b3 = world->getBlock(left_p);
    if (b3.getType() != EMPTY)
    {
        target.x = parent->position.x - (floorf(left_p.x) - 0.2f);
        left = true;
    }
    const Block b4 = world->getBlock(bottom_p);
    if (b4.getType() != EMPTY)
    {
        target.y = parent->position.y - (floorf(bottom_p.y) + 1.0f);
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

world::RigidBody::RigidBody(const HitBox* hitbox) : Component({0,0},RIGID_BODY), hitbox(hitbox)
{}

void world::RigidBody::update(const float& delta_time)
{
    Util::decreaseMagnitude(parent->velocity,drag * delta_time);
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
        if (parent->velocity.x > 0.0f) parent->velocity.x = hitbox->target.x;
    }
    if (hitbox->left)
    {
        if (parent->velocity.x < 0.0f) parent->velocity.x = hitbox->target.x;
    }
}