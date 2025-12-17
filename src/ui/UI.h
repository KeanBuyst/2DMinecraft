#pragma once
#include "../gl/Shader.h"

// forward declaration
namespace world
{
    struct Item;
}

namespace UI
{
    constexpr float CELL_SIZE = 20.0f;

    enum CellType : uint32_t
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

    struct UIComponent
    {
        uint16_t id = 0;

        virtual ~UIComponent() = default;

        virtual void update() {}

        virtual glm::vec2 getPosition() const = 0;
        virtual glm::ivec2 getSize() const = 0;
        virtual int getCount() const = 0;
        virtual const Cell* getCells() const = 0;

        virtual bool isVisible() const = 0;
        virtual void setVisible(bool visible) = 0;
    };

    namespace Renderer
    {
        void Init();
        void Render(gl::ShaderProgram* shader);
        void Update();
        void Cleanup();
        void Add(UIComponent* component);
    }

    bool isCell(const CellType& type);
    bool isSlot(const CellType& type);
}
