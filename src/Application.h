#pragma once

#include <memory>
#include <SDL3/SDL.h>

#include "gl/Shader.h"
#include "ui/Inventory.h"
#include "world/World.h"
#include "world/entities/Entity.h"
#include "world/entities/Player.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern glm::vec4 VIEW_PORT;

constexpr int VIEW_SIZE = world::CHUNK_SIZE * 16 + world::CHUNK_SIZE / 2;

class Application {
public:
  ~Application();

  static Application& GetInstance();
  void run();

  static bool isMouseDown(int button);
  static bool isMousePressed(int button);
  static bool isMouseReleased(int button);
  static glm::vec2 GetWorldMouse();
  static glm::vec2 GetUIMouse();
  static int GetMouseScroll();

  static UI::MouseItemHolder item_holder;

  static bool isKeyDown(SDL_Scancode key);
  static bool isKeyPressed(SDL_Scancode key);
  static bool isKeyUp(SDL_Scancode key);

private:
  Application();

  void events(bool& running);
  void update();
  void render();

  void updateViewPort() const;

  static bool prev_keystate[SDL_SCANCODE_COUNT];
  static const bool* curr_keystate;

  static Uint32 currentMouseState;
  static Uint32 previousMouseState;

  static glm::vec2 mousePos;

  bool fullscreen = false;
  bool keypressed = false;
  SDL_Window* window;
  std::unique_ptr<gl::ShaderProgram> terrain_shader;
  std::unique_ptr<gl::ShaderProgram> entity_shader;
  std::unique_ptr<gl::ShaderProgram> ui_shader;
  SDL_GLContext context;
  Uint32 last_frame_time;
  float frame_rate = -1.0f;

  world::Player* player;

  static int scrollDir;
};
