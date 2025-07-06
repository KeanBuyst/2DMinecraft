#include "EntityHandler.h"

#include "../../glew.h"

using namespace world;

Util::Buffer<Entity,128> EntityHandler::buffer;

GLuint VAO;
GLuint VBO;

void EntityHandler::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Position attribute (x,y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(EntityRenderData),
        reinterpret_cast<void*>(offsetof(EntityRenderData, vertex)));
    glEnableVertexAttribArray(0);

    // Texels attribute (vec4)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(EntityRenderData),
        reinterpret_cast<void*>(offsetof(EntityRenderData, texel)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void EntityHandler::Event(SDL_Event* event)
{
    Entity* entity;
    while (buffer.next(entity))
    {
        entity->event(event);
    }
}

void EntityHandler::Render()
{
    Entity* entity;
    while (buffer.next(entity))
    {
        entity->render();
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, entity_render_batch.size() * sizeof(EntityRenderData), entity_render_batch.data(), GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, entity_render_batch.size());

    entity_render_batch.clear();
}

void EntityHandler::Update(const float& delta_time)
{
    Entity* entity;
    while (buffer.next(entity))
    {
        if (entity->health <= 0)
        {
            buffer.destroy(entity->id);
            continue;
        }
        entity->update(delta_time);
    }
}

void EntityHandler::Destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void EntityHandler::Add(Entity* entity)
{
    entity->id = buffer.add(entity);
}
