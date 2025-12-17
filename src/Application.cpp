#include "Application.h"

#include <glm.hpp>
#include <filesystem>
#include <iostream>
#include <cstring>

#include "Constants.h"
#include "glew.h"
#include "ext/matrix_clip_space.hpp"
#include "resources/Storage.h"
#include "world/generation/Generation.h"
#include "gl/Texture.h"
#include "resources/Resources.h"
#include "ui/Inventory.h"
#include "ui/UI.h"
#include "world/crafting/Crafting.h"
#include "world/entities/EntityHandler.h"
#include "world/entities/Item.h"
#include "world/entities/Player.h"

using namespace world;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

const Uint8* Application::curr_keystate = nullptr;
Uint8 Application::prev_keystate[SDL_NUM_SCANCODES] = {};

Uint32 Application::currentMouseState = 0;
Uint32 Application::previousMouseState = 0;

int Application::scrollDir = 0;
glm::ivec2 Application::mousePos;
UI::MouseItemHolder* Application::item_holder = nullptr;

glm::vec4 VIEW_PORT;

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,
    const GLchar* message,const void* userParam)
{
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") << " type = "<< type
    << ", severity = " << severity << ", message = " << message << std::endl;
}

Application::Application() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    // create window
    window = SDL_CreateWindow("2DMinecraft - 0",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }
    // OpenGL context
    context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        printf("OpenGL context could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        printf("GLEW could not initialize!");
        exit(-1);
    }

    std::cout << "Initialized libraries" << std::endl;

    // error messages
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback,nullptr);
    // enable Alpha blending (allows textures with empty pixels to be transparent)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_ALPHA_TEST);
    glDepthFunc(GL_LESS);

    // Init key state
    curr_keystate = SDL_GetKeyboardState(nullptr);

    // init data path
    std::filesystem::create_directories(res::basePath);

    terrain_shader = std::make_unique<gl::ShaderProgram>();
    terrain_shader->bind(gl::GetShader("terrain.vert"));
    terrain_shader->bind(gl::GetShader("terrain.geom"));
    terrain_shader->bind(gl::GetShader("terrain.frag"));
    terrain_shader->build();

    entity_shader = std::make_unique<gl::ShaderProgram>();
    entity_shader->bind(gl::GetShader("entity.vert"));
    entity_shader->bind(gl::GetShader("entity.frag"));
    entity_shader->build();

    EntityHandler::Init();

    ui_shader = std::make_unique<gl::ShaderProgram>();
    ui_shader->bind(gl::GetShader("ui.vert"));
    ui_shader->bind(gl::GetShader("ui.geom"));
    ui_shader->bind(gl::GetShader("ui.frag"));
    ui_shader->build();

    UI::Renderer::Init();

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
    } else
    {
        player = new Player(origin);
    }

    m_world.init();

    // Crafting handler
    Crafting::Init();

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
    item_holder = new UI::MouseItemHolder();
    UI::Renderer::Add(item_holder);

    glClearColor(0.529f,0.8078f,0.9215686f,1.0f);


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
        memcpy(prev_keystate, curr_keystate, SDL_NUM_SCANCODES);
        curr_keystate = SDL_GetKeyboardState(nullptr);

        events(running);
        update();
        render();
    }
}

bool Application::isMouseDown(const int button)
{
    return currentMouseState & SDL_BUTTON(button);
}

bool Application::isMousePressed(const int button)
{
    return (currentMouseState & SDL_BUTTON(button)) &&
              !(previousMouseState & SDL_BUTTON(button));
}

bool Application::isMouseReleased(const int button)
{
    return !(currentMouseState & SDL_BUTTON(button)) &&
               (previousMouseState & SDL_BUTTON(button));
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

Application::~Application() {
    EntityHandler::Cleanup();
    UI::Renderer::Cleanup();
    Crafting::Destroy();
    res::clear();

    // save player state
    std::fstream file(res::basePath + "world.dat",std::ios::binary | std::ios::out);
    Util::ByteStream stream(&file);

    // seed
    stream << Util::seed_value;
    // player
    player->serialize(stream);
    delete player;

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
// Vector returned has not been floored, thus remember to floor when converting to block positions
glm::vec2 Application::GetWorldMouse()
{
    // change to view port local position
    const float x = (((static_cast<float>(mousePos.x) / (static_cast<float>(SCREEN_WIDTH) / 2.0)) - 1) * VIEW_PORT.y) / PIXEL_SCALE;
    const float y = (( -((static_cast<float>(mousePos.y) / (static_cast<float>(SCREEN_HEIGHT) / 2.0)) - 1) * VIEW_SIZE) / PIXEL_SCALE);
    return glm::vec2(x,y) + world::origin;
}

glm::vec2 Application::GetUIMouse()
{
    const float x = (((static_cast<float>(mousePos.x) / (static_cast<float>(SCREEN_WIDTH) / 2.0)) - 1) * VIEW_PORT.y) / UI::CELL_SIZE;
    const float y = (( -((static_cast<float>(mousePos.y) / (static_cast<float>(SCREEN_HEIGHT) / 2.0)) - 1) * VIEW_SIZE) / UI::CELL_SIZE);
    return {x,y};
}

void Application::events(bool& running)
{
    scrollDir = 0;

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type) {
            case SDL_MOUSEBUTTONDOWN:
            {
                const glm::vec2 mouse = GetWorldMouse();
                    const glm::vec2 chunk = world::ToChunkSpace(mouse);
                switch (e.button.button)
                {
                    case SDL_BUTTON_MIDDLE:
                        {
                            Block block = m_world.getBlock(mouse);
                            block.setBreakState(10);
                            std::cout << "\n-------------------------\n"
                                      << "Block data at (" << mouse.x << ", " << mouse.y << ") in chunk (" << chunk.x << ", " << chunk.y << ")"
                                      << "\nType: " << std::to_string(block.getType())
                                      << "\nWall: " << std::to_string(block.getWall())
                                      << "\nLight Level: " << std::to_string(block.getLightLevel())
                                      << "\nBreak State: " << std::to_string(block.getBreakState())
                                      << "\n-------------------------\n"
                                      << std::endl;
                            m_world.setBlock(block);
                        }
                        break;
                }
                break;
            }
            case SDL_MOUSEWHEEL:
                scrollDir = e.wheel.y;
                break;
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SCREEN_WIDTH = e.window.data1;
                    SCREEN_HEIGHT = e.window.data2;
                    updateViewPort();
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
        SDL_SetWindowTitle(window, title.c_str());
    }
    // NB!! Order is important. UI then entities
    // update mouse item holder
    item_holder->setPosition(GetUIMouse() - 0.4f);
    UI::Renderer::Update();

    // full screen
    if (isKeyPressed(SDL_SCANCODE_F11))
    {
        fullscreen = !fullscreen;
        if (fullscreen)
        {
            SDL_DisplayMode displayMode;
            SDL_GetCurrentDisplayMode(0, &displayMode);
            SCREEN_WIDTH = displayMode.w;
            SCREEN_HEIGHT = displayMode.h;
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else
        {
            SDL_SetWindowFullscreen(window, 0);
            SCREEN_WIDTH = 1280;
            SCREEN_HEIGHT = 720;
            SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }

    // update entities
    EntityHandler::Update();

    origin = player->position;
}

void Application::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw
    // Tile shader to draw world
    terrain_shader->use();

    updateViewPort();
    terrain_shader->useTexture("atlas",0);

    m_world.render();
    // end tile drawing

    // Entity drawing
    entity_shader->use();

    updateViewPort();
    EntityHandler::Render(entity_shader.get());
    // end entity drawing

    // UI rendering
    glDisable(GL_DEPTH_TEST);
    ui_shader->use();

    updateViewPort();
    UI::Renderer::Render(ui_shader.get());
    glEnable(GL_DEPTH_TEST);
    // end UI rendering

    SDL_GL_SwapWindow(window);
}

void Application::updateViewPort() const {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    const float aspect = roundf((static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)) * VIEW_SIZE); // round to help against artifcates

    glm::mat4 mat = glm::ortho<float>(-aspect, aspect, -VIEW_SIZE, VIEW_SIZE,-5.0f,5.0f);
    VIEW_PORT = glm::vec4(-aspect,aspect,-VIEW_SIZE,VIEW_SIZE);

    terrain_shader->sendMatrix("ortho", mat);
}