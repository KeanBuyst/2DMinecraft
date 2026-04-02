#include "UI.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include "../Application.h"
#include "../Util.h"
#include "../world/entities/Item.h"

std::vector<UI::UIComponent*> buffer;

static int RENDER_WIDTH = SCREEN_WIDTH;
static int RENDER_HEIGHT = SCREEN_HEIGHT;

struct RenderCell
{
    glm::vec2 position;
    uint32_t type;
    int item;
    int stack_size;
};

void UI::Renderer::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL3_InitForSDLGPU(Application::WINDOW);
    ImGui_ImplSDLGPU3_InitInfo init_info = {};
    init_info.Device = Application::GPU_DEVICE;
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(Application::GPU_DEVICE, Application::WINDOW);
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    ImGui_ImplSDLGPU3_Init(&init_info);
}

void UI::Renderer::Event(SDL_Event* event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}


void UI::Renderer::Update()
{
    for (UIComponent*& comp : buffer)
    {
        comp->update();
    }
}

void UI::Renderer::Cleanup()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();
}

void UI::Renderer::Add(UIComponent* component)
{
    buffer.push_back(component);
}

void UI::Renderer::Render(SDL_GPUCommandBuffer* cmd,SDL_GPUTexture* swapChain, uint32_t width, uint32_t height)
{
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // start render

    for (UIComponent*& component : buffer)
    {
        if (component->isVisible())
        {
            component->render();
        }
    }

    // end render

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    Imgui_ImplSDLGPU3_PrepareDrawData(draw_data,cmd);

    // Setup and start a render pass
    SDL_GPUColorTargetInfo target_info = {};
    target_info.texture = swapChain;
    target_info.clear_color = SDL_FColor { 0.0f, 0.0f, 0.0f, 0.0f };
    target_info.load_op = SDL_GPU_LOADOP_LOAD;
    target_info.store_op = SDL_GPU_STOREOP_STORE;
    target_info.mip_level = 0;
    target_info.layer_or_depth_plane = 0;
    target_info.cycle = false;
    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &target_info, 1, nullptr);

    // Render ImGui
    ImGui_ImplSDLGPU3_RenderDrawData(draw_data, cmd, render_pass);

    SDL_EndGPURenderPass(render_pass);
}