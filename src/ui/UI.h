#pragma once
#include <SDL3/SDL_events.h>

#include "../gl/Shader.h"

// forward declaration
namespace world
{
    struct Item;
}

namespace UI
{
    constexpr float CELL_SIZE = 20.0f;

    enum CellType : uint8_t
    {
        EMPTY_CELL,
        INVENTORY_SLOT,
        SELECTED_SLOT,
        ARROW_CELL,
        BLANK_CELL,
        HELMET_SLOT,
        CHESTPLATE_SLOT,
        LEGGINGS_SLOT,
        BOOTS_SLOT,
        HAT_SLOT,
        NECKLACE_SLOT,
        RING_SLOT,
        SHOES_SLOT
    };

    struct Cell
    {
        CellType type;
        world::Item* item;
    };

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

    bool isCell(const CellType& type);
    bool isSlot(const CellType& type);
}
