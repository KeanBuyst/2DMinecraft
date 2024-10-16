#include "Application.h"

#include <GL/glew.h>
#include <iostream>
#include <glm.hpp>
#include <filesystem>

#include "Storage.h"

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

glm::vec4 VIEW_PORT;

Application::Application() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }
    // set to use OpenGL core. Latest stuff
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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

    // init data path
    std::filesystem::create_directories(res::basePath);

    world.init();

    shader = std::make_unique<ShaderProgram>();
    shader->bind(GetShader("test.vert"));
    shader->bind(GetShader("test.geom"));
    shader->bind(GetShader("test.frag"));
    shader->build();

    // vsync
    SDL_GL_SetSwapInterval(1);
}

Application& Application::GetInstance() {
    static Application application;
    return application;
}

void Application::run() {
    // Create texture altas
    const res::Texture texture("2DMC.png");

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

        glClear(GL_COLOR_BUFFER_BIT);
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
        case SDLK_f:
            world::origin.x = -103;
        break;
    }
}

void Application::updateViewPort() const {
    constexpr int SIZE = world::CHUNK_SIZE * 16 + world::CHUNK_SIZE / 2;
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    const float aspect = roundf((static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)) * SIZE); // round to help against artifcates

    glm::mat4 mat = glm::ortho<float>(-aspect, aspect, -SIZE, SIZE);
    VIEW_PORT = glm::vec4(-aspect,aspect,-SIZE,SIZE);

    shader->sendMatrix("ortho", mat);
}
