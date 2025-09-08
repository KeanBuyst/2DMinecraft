#include "Application.h"

#include <glm.hpp>
#include <filesystem>
#include <iostream>
#include <cstring>

#include "glew.h"
#include "ext/matrix_clip_space.hpp"
#include "resources/Storage.h"
#include "world/generation/Generation.h"
#include "gl/Texture.h"
#include "ui/Inventory.h"
#include "ui/UI.h"
#include "world/entities/EntityHandler.h"

using namespace world;

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

const Uint8* Application::curr_keystate = nullptr;
Uint8 Application::prev_keystate[SDL_NUM_SCANCODES] = {};
int Application::scrollDir = 0;

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

    m_world.init();

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

    // vsync
    SDL_GL_SetSwapInterval(1);
}

Application& Application::GetInstance() {
    static Application application;
    return application;
}

void Application::run() {
    // Create texture altas
    const gl::Texture tileAtlas("../assets/tiles.png");
    const gl::Texture itemAtlas("../assets/items.png");
    const gl::AtlasTexture entityAtlas("../assets/entities.png");
    const gl::AtlasTexture uiAtlas("../assets/ui.png");
    tileAtlas.bind(0);
    entityAtlas.bind(1);
    itemAtlas.bind(2);
    uiAtlas.bind(3);

    // Create player entity
    player = new Entity({0,10},world::PLAYER);
    player_hitbox = new HitBox({-2.5f,10,2.5f,-18});
    player_rigid_body = new RigidBody(player_hitbox);
    auto* leg_1 = new Sprite({0,-12,4,12},entityAtlas.getTexel({8,8,4,12}),1.0f);
    auto* leg_2 = new Sprite({0,-12,4,12},entityAtlas.getTexel({8,8,4,12}),1.0f);
    auto* head = new Sprite({0,10,9,8},entityAtlas.getTexel({0,0,8,7}),1.0f);
    auto** components = new Component*[]
    {
        head, // HEAD
        new Sprite({0,0,4,12},entityAtlas.getTexel({4,8,4,12}),1.0f), // BODY
        new Sprite({0,0,4,12},entityAtlas.getTexel({0,8,4,12}),1.0f), // ARM
        leg_1,
        leg_2,
        player_rigid_body,
        player_hitbox,
        new BipedalAnimation(leg_1,leg_2),
        new PlayerHeadAnimation(head)
    };
    player->addComponents(components,9);
    EntityHandler::Add(player);

    // Create Player inventory
    auto* inventory = new UI::Hotbar();
    auto* item = new Item({0,0}, GRASS_BLOCK);
    item->setAmount(8);
    inventory->addItem(item);
    inventory->setVisible(true);
    UI::Renderer::Add(inventory);


    glClearColor(0.529f,0.8078f,0.9215686f,1.0f);

    // creating loop
    bool running = true;
    while (running)
    {
        // update key state
        memcpy(prev_keystate, curr_keystate, SDL_NUM_SCANCODES);
        curr_keystate = SDL_GetKeyboardState(nullptr);

        events(running);
        update();
        render();
    }
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
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
// Vector returned has not been floored, thus remember to floor when converting to block positions
glm::vec2 Application::GetWorldMouse()
{
    // Get window local mouse position
    int window_x, window_y;
    SDL_GetMouseState(&window_x,&window_y);
    // change to view port local position
    const float x = (((static_cast<float>(window_x) / (static_cast<float>(SCREEN_WIDTH) / 2.0)) - 1) * VIEW_PORT.y) / world::PIXEL_SCALE;
    const float y = (( -((static_cast<float>(window_y) / (static_cast<float>(SCREEN_HEIGHT) / 2.0)) - 1) * VIEW_SIZE) / world::PIXEL_SCALE);
    return glm::vec2(x,y) + world::origin;
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
                    case SDL_BUTTON_LEFT:
                        {
                            //world::Block block(world::EMPTY,mouse,world::EMPTY);
                            //world.setBlock(block);
                            world::Generate::Lighting(&m_world,chunk);
                        }
                        break;
                    case SDL_BUTTON_RIGHT:
                        m_world.setBlock({world::TORCH, mouse, world::STONE_WALL},true);
                        break;
                    case SDL_BUTTON_MIDDLE:
                        {
                            Block block = m_world.getBlock(mouse);
                            std::cout << "\n-------------------------\n"
                                      << "Block data at (" << mouse.x << ", " << mouse.y << ") in chunk (" << chunk.x << ", " << chunk.y << ")"
                                      << "\nType: " << std::to_string(block.getType())
                                      << "\nWall: " << std::to_string(block.getWall())
                                      << "\nLight Level: " << std::to_string(block.getLightLevel())
                                      << "\n-------------------------\n"
                                      << std::endl;
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
    const float delta_time = static_cast<float>(SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    const float current_frame_rate = 1.0f / delta_time;
    if (frame_rate == -1.0f) frame_rate = current_frame_rate;
    else frame_rate = (frame_rate + current_frame_rate) / 2;

    if (static_cast<int>(last_frame_time) % 30 == 1)
    {
        const std::string title = "2DMinecraft - " + std::to_string(static_cast<int>(frame_rate));
        SDL_SetWindowTitle(window,title.c_str());
    }
    computePlayer();
    EntityHandler::Update(delta_time);
    UI::Renderer::Update(delta_time);

    world::origin = player->position;
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
    ui_shader->use();

    updateViewPort();
    UI::Renderer::Render(ui_shader.get());
    // end UI rendering

    SDL_GL_SwapWindow(window);
}

void Application::computePlayer()
{
    constexpr float SPEED = 8.0f;

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
    if (isKeyDown(SDL_SCANCODE_A))
    {
        if (player->acceleration.x == 0) player->velocity.x = -SPEED / 2.0f;

        player->acceleration.x = -SPEED;
        player_rigid_body->moving = true;
    }
    if (isKeyDown(SDL_SCANCODE_D))
    {
        // creates more instantaneous movement
        if (player->acceleration.x == 0) player->velocity.x = SPEED / 2.0f;

        player->acceleration.x = SPEED;
        player_rigid_body->moving = true;
    }
    if (isKeyPressed(SDL_SCANCODE_W) && player_hitbox->bottom)
    {
        player->velocity.y = 20.0f;
    }
    if (isKeyUp(SDL_SCANCODE_A) || isKeyUp(SDL_SCANCODE_D))
    {
        player->acceleration.x = 0;
        player_rigid_body->moving = false;
    }
}

void Application::updateViewPort() const {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    const float aspect = roundf((static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)) * VIEW_SIZE); // round to help against artifcates

    glm::mat4 mat = glm::ortho<float>(-aspect, aspect, -VIEW_SIZE, VIEW_SIZE,-5.0f,5.0f);
    VIEW_PORT = glm::vec4(-aspect,aspect,-VIEW_SIZE,VIEW_SIZE);

    terrain_shader->sendMatrix("ortho", mat);
}
