#include "Component.h"

#include "Entity.h"
#include "../../Util.h"

void world::SpriteComponent::event(SDL_Event* event)
{}

void world::SpriteComponent::update(const float& delta_time)
{}

void world::SpriteComponent::render()
{
    const float p1x = (parent->position.x + position.x) - (width / 2);
    const float p1y = (parent->position.y + position.y) - (height / 2);
    const float p2x = p1x + width;
    const float p2y = p1y + height;

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

world::SpriteComponent::SpriteComponent(const glm::vec4 dimensions,glm::vec4 textRect,const float scale) : Component(dimensions,SPRITE)
{
    width = dimensions.z * scale;
    height = dimensions.w * scale;

    textRect.z += textRect.x;
    textRect.w += textRect.y;

    texel = textRect;
}