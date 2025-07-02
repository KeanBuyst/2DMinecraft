#pragma once

#include <memory>
#include <SDL.h>

#include "gl/Shader.h"
#include "world/World.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern glm::vec4 VIEW_PORT;

constexpr int VIEW_SIZE = world::CHUNK_SIZE * 16 + world::CHUNK_SIZE / 2;

class Application {
public:
  ~Application();

  static Application& GetInstance();
  void run();

  static glm::vec2 GetWorldMouse();

private:
  Application();

  void events(bool& running);
  void update();
  void render();

  void KeyDown(SDL_Keycode key);
  void updateViewPort() const;

  world::World world;
  bool fullscreen = false;
  SDL_Window* window;
  std::unique_ptr<gl::ShaderProgram> terrain_shader;
  std::unique_ptr<gl::ShaderProgram> entity_shader;
  SDL_GLContext context;
  Uint32 last_frame_time;
  float frame_rate = -1.0f;
};
