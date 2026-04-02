#pragma once

#include <SDL3/SDL_events.h>
#include "../gl/Shader.h"

namespace UI
{
    class UIComponent
    {
    public:
        virtual ~UIComponent() = default;

        virtual void render() = 0;
        virtual void update() {}

        virtual bool isVisible() const = 0;
        virtual void setVisible(bool visible) = 0;
    };

    namespace Renderer
    {
        void Init();
        void Render(SDL_GPUCommandBuffer* cmd,SDL_GPUTexture* swapChain, uint32_t width, uint32_t height);
        void Event(SDL_Event* event);
        void Update();
        void Cleanup();
        void Add(UIComponent* component);
    }
}