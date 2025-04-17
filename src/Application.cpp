#include "Application.h"

#include <glm.hpp>
#include <filesystem>
#include <iostream>

#include "glew.h"
#include "ext/matrix_clip_space.hpp"
#include "resources/Storage.h"
#include "world/generation/Generation.h"

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

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
    window = SDL_CreateWindow("2DMinecraft",
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

    // init data path
    std::filesystem::create_directories(res::basePath);

    shader = std::make_unique<ShaderProgram>();
    shader->bind(GetShader("terrain.vert"));
    shader->bind(GetShader("terrain.geom"));
    shader->bind(GetShader("terrain.frag"));
    shader->build();

    world.init();

    // vsync
    SDL_GL_SetSwapInterval(1);
}

Application& Application::GetInstance() {
    static Application application;
    return application;
}

void Application::run() {
    // Create texture altas
    const res::Texture texture("../assets/tiles.png");

    shader->use();
    updateViewPort();

    texture.bind(0);
    shader->useTexture("atlas",0);

    glClearColor(0.529f,0.8078f,0.9215686f,1);

    // creating loop
    SDL_Event e;
    bool running = true;
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            switch (e.type) {
                case SDL_KEYDOWN:
                    KeyDown(e.key.keysym.sym);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                {
                    const glm::vec2 mouse = GetWorldMouse();
                    switch (e.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                        {
                            world::Block block(world::EMPTY,mouse,world::EMPTY);
                            world.setBlock(block);
                        }
                        break;
                        case SDL_BUTTON_RIGHT:
                        break;
                    }
                    break;
                }
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // draw

        world.render();

        SDL_GL_SwapWindow(window);
    }
}

Application::~Application() {
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
    const float x = (((static_cast<float>(window_x) / (static_cast<float>(SCREEN_WIDTH) / 2.0)) - 1) * VIEW_PORT.y) / world::PIXEL_SIZE;
    const float y = (( -((static_cast<float>(window_y) / (static_cast<float>(SCREEN_HEIGHT) / 2.0)) - 1) * VIEW_SIZE) / world::PIXEL_SIZE);
    return glm::vec2(x,y) + world::origin;
}

void Application::KeyDown(const SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_F11:
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
        updateViewPort();
        break;
        case SDLK_a:
            world::origin.x -= 1;
        break;
        case SDLK_d:
            world::origin.x += 1;
        break;
        case SDLK_w:
            world::origin.y += 1;
        break;
        case SDLK_s:
            world::origin.y -= 1;
        break;
    }
}

void Application::updateViewPort() const {
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    const float aspect = roundf((static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)) * VIEW_SIZE); // round to help against artifcates

    glm::mat4 mat = glm::ortho<float>(-aspect, aspect, -VIEW_SIZE, VIEW_SIZE,-2.0f,2.0f);
    VIEW_PORT = glm::vec4(-aspect,aspect,-VIEW_SIZE,VIEW_SIZE);

    shader->sendMatrix("ortho", mat);
}
