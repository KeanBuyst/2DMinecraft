#pragma once

#include <memory>
#include <SDL3/SDL.h>

#include "gl/Shader.h"
//#include "ui/Inventory.h"
#include "world/World.h"
#include "world/entities/Entity.h"
#include "world/entities/Player.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

class Application {
public:
  ~Application();

  static Application& GetInstance();
  void run();

  static bool isMouseDown(int button);
  static bool isMousePressed(int button);
  static bool isMouseReleased(int button);
  static glm::vec2 GetWorldMouse();
  static int GetMouseScroll();

  static UI::MouseItemHolder item_holder;
  static SDL_GPUDevice* GPU_DEVICE;
  static SDL_Window* WINDOW;

  static bool isKeyDown(SDL_Scancode key);
  static bool isKeyPressed(SDL_Scancode key);
  static bool isKeyUp(SDL_Scancode key);

private:
  Application();

  void events(bool& running);
  void update();
  void render();

  static bool prev_keystate[SDL_SCANCODE_COUNT];
  static const bool* curr_keystate;

  static Uint32 currentMouseState;
  static Uint32 previousMouseState;

  static glm::vec2 mousePos;

  bool fullscreen = false;
  bool keypressed = false;
  Uint32 last_frame_time;
  float frame_rate = -1.0f;

  world::Player* player;

  static int scrollDir;
};
