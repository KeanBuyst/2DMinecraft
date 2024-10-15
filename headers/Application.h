#pragma once

#include <memory>
#include <SDL.h>

#include "World.h"
#include "Shader.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern glm::vec4 VIEW_PORT;

class Application {
public:
  ~Application();

  static Application& GetInstance();
  void run();

private:
  Application();

  void KeyDown(SDL_Keycode key);
  void updateViewPort() const;

  world::World world;
  bool fullscreen = false;
  SDL_Window* window;
  std::unique_ptr<ShaderProgram> shader;
  SDL_GLContext context;
};
