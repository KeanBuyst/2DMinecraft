#include "EntityHandler.h"

#include <functional>
#include <SDL3/SDL.h>

#include "../../gl/Shader.h"

#include "../Region.h"
#include "../World.h"
#include "../../Application.h"
#include "../../gl/Texture.h"
#include "../../resources/Resources.h"

using namespace world;

static Entity* head = nullptr;

static SDL_GPUBuffer* vertexBuffer = nullptr;
static SDL_GPUGraphicsPipeline* pipeline = nullptr;
static uint32_t batch_max_size = MAX_SPRITES_INIT * 6;

static constexpr int MAX_ENTITY_TYPES = static_cast<int>(EntityType::COUNT);
static std::array<EntityHandler::EntityCreator, MAX_ENTITY_TYPES> registry = { nullptr };

void EntityHandler::Init()
{
    SDL_GPUBufferCreateInfo bufferInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = batch_max_size * static_cast<uint32_t>(sizeof(EntityRenderData))
    };
    vertexBuffer = SDL_CreateGPUBuffer(Application::GPU_DEVICE,&bufferInfo);

    gl::Shader vertex("entity_vert", gl::VERTEX);
    gl::Shader fragment("entity_frag",gl::FRAGMENT);

    constexpr int num_attributes = 3;
    SDL_GPUVertexAttribute attributes[num_attributes] = {
        {0,0,SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(EntityRenderData,position)},
        {1,0,SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(EntityRenderData,uv)},
        {2,0,SDL_GPU_VERTEXELEMENTFORMAT_FLOAT, offsetof(EntityRenderData,light)}
    };

    SDL_GPUVertexBufferDescription vertexBindingDesc = {
        0,
        sizeof(EntityRenderData),
        SDL_GPU_VERTEXINPUTRATE_VERTEX,
        0
    };

    SDL_GPUTextureFormat format = SDL_GetGPUSwapchainTextureFormat(Application::GPU_DEVICE, Application::WINDOW);
    if (format == SDL_GPU_TEXTUREFORMAT_INVALID)
    {
        SDL_Log("Error: Swapchain format is INVALID (0). Window not claimed?");
        throw "Error: Swapchain format is INVALID (0). Window not claimed?";
    }

    SDL_GPUColorTargetDescription targetDesc = {
        format,
         {
             .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
             .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
             .color_blend_op = SDL_GPU_BLENDOP_ADD,
             .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
             .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
             .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
             .enable_blend = true,
         }
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {
        .vertex_shader = vertex,
        .fragment_shader = fragment,
        .vertex_input_state = {
            .vertex_buffer_descriptions = &vertexBindingDesc,
            .num_vertex_buffers = 1,
            .vertex_attributes = attributes,
            .num_vertex_attributes = num_attributes
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .depth_stencil_state = {
            .compare_op = SDL_GPU_COMPAREOP_ALWAYS,
            .enable_depth_test = false,
            .enable_depth_write = false
        },
        .target_info = {
            .color_target_descriptions = &targetDesc,
            .num_color_targets = 1,
        }
    };

    SDL_GPUGraphicsPipeline* terrainPipline = SDL_CreateGPUGraphicsPipeline(Application::GPU_DEVICE,&pipelineInfo);
    if (!terrainPipline)
    {
        SDL_Log("Error: SDL_CreateGPUGraphicsPipeline -> %s", SDL_GetError());
        throw "Error: SDL_CreateGPUGraphicsPipeline";
    }
    pipeline = terrainPipline;
}

void EntityHandler::Render(SDL_GPUCommandBuffer* cmd,SDL_GPUTexture* swapChain, uint32_t width, uint32_t height)
{
    Entity* entity = GetHead();
    while (entity)
    {
        if (!OutOfBounds(entity->position))
        {
            entity->render();
        }
        entity = entity->getNext();
    }

    // dynamically scale vertex buffer if needed
    size_t count = render_batch.size();
    if (count == 0) return;

    if (count > batch_max_size)
    {
        batch_max_size *= (count / MAX_SPRITES_INIT) + 1;

        SDL_ReleaseGPUBuffer(Application::GPU_DEVICE,vertexBuffer);

        SDL_GPUBufferCreateInfo info = {
            SDL_GPU_BUFFERUSAGE_VERTEX,
            batch_max_size * static_cast<uint32_t>(sizeof(EntityRenderData)),
            0
        };
        vertexBuffer = SDL_CreateGPUBuffer(Application::GPU_DEVICE,&info);
    }
    uint32_t size = count * sizeof(EntityRenderData);

    SDL_GPUTransferBufferCreateInfo transferInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = size
    };
    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(Application::GPU_DEVICE, &transferInfo);

    uint8_t* mapPtr = reinterpret_cast<uint8_t*>(SDL_MapGPUTransferBuffer(Application::GPU_DEVICE,transferBuffer,false));
    memcpy(mapPtr, render_batch.data(), size);
    SDL_UnmapGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer);

    SDL_GPUTransferBufferLocation src = {
        transferBuffer,
        0
    };

    SDL_GPUBufferRegion dest = {
        vertexBuffer,
        0,
        size
    };

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);

    SDL_UploadToGPUBuffer(copy_pass,&src,&dest,true);

    SDL_EndGPUCopyPass(copy_pass);
    SDL_ReleaseGPUTransferBuffer(Application::GPU_DEVICE, transferBuffer);

    SDL_GPUColorTargetInfo color_target_info = {
        .texture = swapChain,
        .mip_level = 0,
        .layer_or_depth_plane = 0,
        .clear_color = {0.529f,0.8078f,0.9215686f,1.0f},
        .load_op = SDL_GPU_LOADOP_LOAD,
        .store_op = SDL_GPU_STOREOP_STORE,
        .cycle = false
    };

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd,&color_target_info, 1, NULL);
    SDL_BindGPUGraphicsPipeline(render_pass,pipeline);

    SDL_GPUViewport viewport = {
        0, 0,
         (float)width, (float)height,
        -1.0f,1.0f
    };

    SDL_SetGPUViewport(render_pass,&viewport);

    SDL_Rect scissor = {
        0,0,(int)width,(int)height
    };
    SDL_SetGPUScissor(render_pass,&scissor);

    SDL_GPUBufferBinding vertexBinding = {
        .buffer = vertexBuffer,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(render_pass, 0, &vertexBinding, 1);

    SDL_GPUTextureSamplerBinding texture_binding = {
        .texture = *res::atlas::tiles,
        .sampler = gl::Texture::GetSampler()
    };
    uint32_t index = 0;

    if (!render_batch.tileBatch.empty())
    {
        SDL_BindGPUFragmentSamplers(render_pass,0,&texture_binding,1);
        SDL_DrawGPUPrimitives(render_pass,render_batch.tileBatch.size(),1,index,0);
        index += render_batch.tileBatch.size();
    }

    if (!render_batch.entityBatch.empty())
    {
        texture_binding.texture = *res::atlas::entities;
        SDL_BindGPUFragmentSamplers(render_pass,0,&texture_binding,1);
        SDL_DrawGPUPrimitives(render_pass,render_batch.entityBatch.size(),1,index,0);
        index += render_batch.entityBatch.size();
    }

    if (!render_batch.itemBatch.empty())
    {
        texture_binding.texture = *res::atlas::items;
        SDL_BindGPUFragmentSamplers(render_pass,0,&texture_binding,1);
        SDL_DrawGPUPrimitives(render_pass,render_batch.itemBatch.size(),1,index,0);
    }

    SDL_EndGPURenderPass(render_pass);

    render_batch.clear();
}

Entity* EntityHandler::GetHead()
{
    // special case for if the head needs to be destroyed
    if (head && head->dead())
    {
        Entity* temp = head->getNext();
        if (head->destroy()) delete head;
        else
        {
            head->setNext(nullptr);
            head->requeue();
        }
        head = temp;
    }
    return head;
}

void EntityHandler::Register(EntityType type, EntityCreator creator)
{
    registry[static_cast<int>(type)] = creator;
}

void EntityHandler::Update()
{
    Entity* entity = GetHead();
    while (entity)
    {
        // check if entity is still in processing area (loaded chunks minus the post-processing buffer area)
        if (OutOfBounds(entity->position))
        {
            glm::ivec2 chunk_pos = ToChunkSpace(entity->position);
            if(glm::ivec2 a_pos = chunk_pos; !ChunkToArray(a_pos))
            {
                entity->deque();

                if (entity->type != EntityType::PLAYER)
                {
                    std::vector<uint8_t>& vector = handler.fetch(chunk_pos);
                    Util::ByteStream stream(&vector);
                    entity->serialize(stream);
                }
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
    Entity* entity = GetHead();
    while (entity)
    {
        Entity* next = entity->getNext();

        if (entity->type != EntityType::PLAYER)
        {
            std::vector<uint8_t>& vector = handler.fetch(ToChunkSpace(entity->position));
            Util::ByteStream stream(&vector);

            entity->serialize(stream);
            delete entity;
        }
        entity = next;
    }
    head = nullptr;

    SDL_ReleaseGPUGraphicsPipeline(Application::GPU_DEVICE,pipeline);
    SDL_ReleaseGPUBuffer(Application::GPU_DEVICE,vertexBuffer);
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
