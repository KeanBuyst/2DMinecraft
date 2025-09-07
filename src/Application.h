#pragma once

#include <memory>
#include <SDL.h>

#include "gl/Shader.h"
#include "world/World.h"
#include "world/entities/Entity.h"

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

  static bool isKeyDown(SDL_Scancode key);
  static bool isKeyPressed(SDL_Scancode key);
  static bool isKeyUp(SDL_Scancode key);

private:
  Application();

  void events(bool& running);
  void update();
  void render();

  void updateViewPort() const;

  void computePlayer();

  static Uint8 prev_keystate[SDL_NUM_SCANCODES];
  static const Uint8* curr_keystate;

  bool fullscreen = false;
  bool keypressed = false;
  SDL_Window* window;
  std::unique_ptr<gl::ShaderProgram> terrain_shader;
  std::unique_ptr<gl::ShaderProgram> entity_shader;
  std::unique_ptr<gl::ShaderProgram> ui_shader;
  SDL_GLContext context;
  Uint32 last_frame_time;
  float frame_rate = -1.0f;

  world::Entity* player;
  world::HitBox* player_hitbox;
  world::RigidBody* player_rigid_body;
};
