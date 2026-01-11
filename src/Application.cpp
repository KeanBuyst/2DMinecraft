#include "Application.h"

#include <activation.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <filesystem>
#include <iostream>
#include <cstring>

#include "Constants.h"
#include "Util.h"
#include "resources/Storage.h"
#include "world/generation/Generation.h"
#include "gl/Texture.h"
#include "libs/SDL_shadercross.h"
#include "resources/Resources.h"
#include "world/entities/EntityHandler.h"
/*#include "ui/Inventory.h"
#include "ui/UI.h"
#include "world/crafting/Crafting.h"
#include "world/entities/EntityHandler.h"
*/

using namespace world;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

const bool* Application::curr_keystate = nullptr;
bool Application::prev_keystate[SDL_SCANCODE_COUNT] = {};

Uint32 Application::currentMouseState = 0;
Uint32 Application::previousMouseState = 0;

int Application::scrollDir = 0;
glm::vec2 Application::mousePos;
//UI::MouseItemHolder Application::item_holder;

SDL_GPUDevice* Application::GPU_DEVICE = nullptr;
SDL_Window* Application::WINDOW = nullptr;

Application::Application() {
    // initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
    {
        SDL_Log("SDL could not initialize\n%s", SDL_GetError());
        throw "SDL could not initialize";
    }
    if (!SDL_ShaderCross_Init()) {
        SDL_Log("ShaderCross could not initialize\n%s", SDL_GetError());
        throw "ShaderCross could not initialize";
    }


    // create window
    WINDOW = SDL_CreateWindow("2DMinecraft - 0", SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!WINDOW)
    {
        SDL_Log("Window could not be created\n%s", SDL_GetError());
        throw "Window could not be created";
    }

   GPU_DEVICE = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL,
                            true, nullptr);
    if (!GPU_DEVICE)
    {
        SDL_Log("Error: SDL_CreateGPUDevice\n%s",SDL_GetError());
        throw "Error: SDL_CreateGPUDevice";
    }

    if (!SDL_ClaimWindowForGPUDevice(GPU_DEVICE,WINDOW))
    {
        SDL_Log( "Error: SDL_ClaimWindowForGPUDevice\n%s",SDL_GetError());
        throw "Error: SDL_ClaimWindowForGPUDevice";
    }

    std::cout << "Initialized SDL and window" << std::endl;

    // Init key state
    curr_keystate = SDL_GetKeyboardState(nullptr);

    // init data path
    std::filesystem::create_directories(res::basePath);

    World::Init();
    EntityHandler::Init();

    // load resources
    res::load();

    // load state data
    std::fstream file(res::basePath + "world.dat",std::ios::binary | std::ios::in);

    if (file.is_open())
    {
        Util::ByteStream stream(&file);
        uint8_t entityType;
        stream >> Util::seed_value;
        stream >> entityType;
        player = new Player(stream);
        origin = player->position;
        chunk_origin = ToChunkSpace(origin);
    } else
    {
        player = new Player(origin);
    }

    world_ptr = new World();

    // Crafting handler
    //Crafting::Init();

    // vsync
    SDL_GL_SetSwapInterval(1);
}

Application& Application::GetInstance() {
    static Application application;
    return application;
}

void Application::run() {
    // Registure player entity
    EntityHandler::Add(player);

    // Mouse item holder (always last in order)
    //UI::Renderer::Add(&item_holder);

    // reset clock
    last_frame_time = SDL_GetTicks();

    // creating loop
    bool running = true;
    while (running)
    {
        // update mouse sate
        previousMouseState = currentMouseState;
        currentMouseState = SDL_GetMouseState(&mousePos.x,&mousePos.y);

        // update key state
        memcpy(prev_keystate, curr_keystate, SDL_SCANCODE_COUNT);
        curr_keystate = SDL_GetKeyboardState(nullptr);

        events(running);
        update();
        render();
    }
}

bool Application::isMouseDown(const int button)
{
    return currentMouseState & SDL_BUTTON_MASK(button);
}

bool Application::isMousePressed(const int button)
{
    return (currentMouseState & SDL_BUTTON_MASK(button)) &&
              !(previousMouseState & SDL_BUTTON_MASK(button));
}

bool Application::isMouseReleased(const int button)
{
    return !(currentMouseState & SDL_BUTTON_MASK(button)) &&
               (previousMouseState & SDL_BUTTON_MASK(button));
}

bool Application::isKeyDown(const SDL_Scancode key)
{
    return curr_keystate[key];
}

bool Application::isKeyPressed(const SDL_Scancode key)
{
    return !prev_keystate[key] && curr_keystate[key];
}

bool Application::isKeyUp(const SDL_Scancode key)
{
    return prev_keystate[key] && !curr_keystate[key];
}

int Application::GetMouseScroll()
{
    return scrollDir;
}

Application::~Application()
{
    SDL_WaitForGPUIdle(GPU_DEVICE);
    World::Cleanup();
    delete world_ptr;
    EntityHandler::Cleanup();
    // UI::Renderer::Cleanup();
    // Crafting::Destroy();
    res::clear();

    // save player state
    std::fstream file(res::basePath + "world.dat",std::ios::binary | std::ios::out);
    Util::ByteStream stream(&file);

    // seed
    stream << Util::seed_value;
    // player
    player->serialize(stream);
    delete player;

    SDL_DestroyGPUDevice(GPU_DEVICE);
    SDL_DestroyWindow(WINDOW);
    SDL_ShaderCross_Quit();
    SDL_Quit();
}
// Vector returned has not been floored, thus remember to floor when converting to block positions
glm::vec2 Application::GetWorldMouse()
{
    // change to view port local position
    const float x = (((mousePos.x / (static_cast<float>(SCREEN_WIDTH) / 2.0)) - 1) * VIEW_SIZE.x) / BLOCK_SIZE;
    const float y = (( -((mousePos.y / (static_cast<float>(SCREEN_HEIGHT) / 2.0)) - 1) * VIEW_SIZE.y) / BLOCK_SIZE);
    return glm::vec2(x,y) + origin;
}

/*glm::vec2 Application::GetUIMouse()
{
    const float x = (((mousePos.x / (static_cast<float>(SCREEN_WIDTH) / 2.0)) - 1) * VIEW_PORT.y) / UI::CELL_SIZE;
    const float y = (( -((mousePos.y / (static_cast<float>(SCREEN_HEIGHT) / 2.0)) - 1) * VIEW_SIZE) / UI::CELL_SIZE);
    return {x,y};
}*/

void Application::events(bool& running)
{
    scrollDir = 0;

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type) {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                const glm::vec2 mouse = GetWorldMouse();
                    const glm::vec2 chunk = world::ToChunkSpace(mouse);
                switch (e.button.button)
                {
                    case SDL_BUTTON_MIDDLE:
                        {
                            Block block = world_ptr->getBlock(mouse);
                            std::cout << "\n-------------------------\n"
                                      << "Block data at (" << mouse.x << ", " << mouse.y << ") in chunk (" << chunk.x << ", " << chunk.y << ")"
                                      << "\nType: " << std::to_string(block.getType())
                                      << "\nWall: " << std::to_string(block.getWall())
                                      << "\nLight Level: " << std::to_string(block.getLightLevel())
                                      << "\nBreak State: " << std::to_string(block.getBreakState())
                                      << "\n-------------------------\n"
                                      << std::endl;
                        }
                        break;
                }
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL:
                scrollDir = e.wheel.y;
                break;
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                {
                    SCREEN_WIDTH = e.window.data1;
                    SCREEN_HEIGHT = e.window.data2;
                }
                break;
        }
    }
}

void Application::update()
{
    const Uint32 now = SDL_GetTicks();
    delta_time = static_cast<float>(now - last_frame_time) / 1000.0f;
    last_frame_time = now;

    // clamp delta time. Prevent physics explosions or huge movement jumps
    if (delta_time > 0.1f) // MIN FPS is 10
    {
        delta_time = 0.1f;
    }

    const float current_frame_rate = (delta_time > 0.0f) ? (1.0f / delta_time) : 0.0f;

    // exponential moving average for smoother FPS
    if (frame_rate < 0.0f) frame_rate = current_frame_rate;
    else frame_rate = frame_rate * 0.9f + current_frame_rate * 0.1f;

    // update title once per second
    static Uint32 last_title_update = 0;
    if (now - last_title_update >= 1000) {
        last_title_update = now;
        const std::string title = "2DMinecraft - " + std::to_string(static_cast<int>(frame_rate));
        SDL_SetWindowTitle(WINDOW,title.c_str());
    }
    // NB!! Order is important. UI then entities
    // update mouse item holder
    //item_holder.setPosition(GetUIMouse() - 0.4f);
    //UI::Renderer::Update();

    // full screen
    if (isKeyPressed(SDL_SCANCODE_F11))
    {
        fullscreen = !fullscreen;
        if (fullscreen)
        {
            SDL_DisplayID id = SDL_GetPrimaryDisplay();
            const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(id);
            SCREEN_WIDTH = mode->w;
            SCREEN_HEIGHT = mode->h;
            SDL_SetWindowFullscreen(WINDOW, true);
        }
        else
        {
            SDL_SetWindowFullscreen(WINDOW, false);
            SCREEN_WIDTH = 1280;
            SCREEN_HEIGHT = 720;
            SDL_SetWindowSize(WINDOW, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }

    // update entities
    EntityHandler::Update();

    origin = player->position;
}

struct Uniforms
{
    glm::mat4 ortho;
    glm::vec2 origin;
};

void Application::render()
{
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(GPU_DEVICE);

    // uniform matrix data
    glm::mat4 matrix = gl::GetOrthoMat();
    Uniforms uniforms = {matrix,origin * BLOCK_SIZE};
    SDL_PushGPUVertexUniformData(cmd,0,&uniforms,sizeof(Uniforms));

    // set up swapchain
    SDL_GPUTexture* swapChain;
    uint32_t width, height;

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd,WINDOW,&swapChain,&width,&height) || !swapChain)
    {
        SDL_Log("Render frame skipped");
        SDL_SubmitGPUCommandBuffer(cmd);
        return;
    }

    world_ptr->render(cmd,swapChain,width,height);
    EntityHandler::Render(cmd,swapChain,width,height);

    SDL_SubmitGPUCommandBuffer(cmd);
}