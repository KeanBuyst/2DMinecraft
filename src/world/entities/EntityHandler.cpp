#include "EntityHandler.h"

#include "../../glew.h"
#include "../../gl/Shader.h"

#include <functional>

#include "../Region.h"
#include "../World.h"

using namespace world;

Entity* EntityHandler::head = nullptr;

static GLuint VAO;
static GLuint VBO;

static constexpr int MAX_ENTITY_TYPES = static_cast<int>(EntityType::COUNT);
static std::array<EntityHandler::EntityCreator, MAX_ENTITY_TYPES> registry = { nullptr };

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

    // Light Level attribute
    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE, sizeof(EntityRenderData),
        reinterpret_cast<void*>(offsetof(EntityRenderData, lightLevel)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void EntityHandler::Event(SDL_Event* event)
{
    Entity* entity = head;
    while (entity)
    {
        entity->event(event);

        entity = entity->getNext();
    }
}

void EntityHandler::Render(gl::ShaderProgram* shader)
{
    Entity* entity = head;
    while (entity)
    {
        if (!OutOfBounds(entity->position))
        {
            entity->render();
        }
        entity = entity->getNext();
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, render_batch.size() * sizeof(EntityRenderData), render_batch.data(), GL_STATIC_DRAW);

    int size = render_batch.tileBatch.size();

    shader->useTexture("atlas",0);
    glDrawArrays(GL_TRIANGLES, 0, size);

    shader->useTexture("atlas",1);
    glDrawArrays(GL_TRIANGLES, size, render_batch.entityBatch.size());

    size += render_batch.entityBatch.size();
    shader->useTexture("atlas",2);
    glDrawArrays(GL_TRIANGLES, size, render_batch.itemBatch.size());

    render_batch.clear();
}

void EntityHandler::Register(EntityType type, EntityCreator creator)
{
    registry[static_cast<int>(type)] = creator;
}

void EntityHandler::Update()
{
    // special case for if the head needs to be destroyed
    if (head && head->dead())
    {
        Entity* temp = head->getNext();
        delete head;
        head = temp;
    }

    Entity* entity = head;
    while (entity)
    {
        // check if entity is still in processing area (loaded chunks minus the post-processing buffer area)
        if (OutOfBounds(entity->position))
        {
            glm::ivec2 chunk_pos = ToChunkSpace(entity->position);
            if(glm::ivec2 a_pos = chunk_pos; !ChunkToArray(a_pos))
            {
                entity->deque();

                std::vector<uint8_t>& vector = handler.fetch(chunk_pos);
                Util::ByteStream stream(&vector);

                if (entity->type != EntityType::PLAYER)
                    entity->serialize(stream);
            }
        }
        else
        {
            entity->update();
        }
        entity = entity->getNext();
    }
}

void EntityHandler::Cleanup()
{
    Entity* entity = head;
    while (entity)
    {
        std::vector<uint8_t>& vector = handler.fetch(ToChunkSpace(entity->position));
        Util::ByteStream stream(&vector);

        Entity* next = entity->getNext();

        if (entity->type != EntityType::PLAYER)
        {
            entity->serialize(stream);
            delete entity;
        }
        entity = next;
    }
    head = nullptr;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void EntityHandler::Add(Entity* entity)
{
    entity->setNext(head);
    head = entity;
}

Entity* EntityHandler::Add(Util::ByteStream& stream)
{
    uint8_t nil = static_cast<uint8_t>(EntityType::COUNT);
    uint8_t type = nil;
    Entity* entity = nullptr;
    stream >> type;
    if (type != nil)
    {
        entity = registry[type](stream);
        Add(entity);
    }
    return entity;
}

void EntityHandler::EntityRenderBatch::clear()
{
    tileBatch.clear();
    entityBatch.clear();
    itemBatch.clear();
}
