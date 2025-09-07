#pragma once
#include "../gl/Shader.h"
#include "../world/entities/Item.h"

namespace UI
{
    constexpr float CELL_SIZE = 20.0f;

    enum CellType : uint32_t
    {
        EMPTY_CELL,
        INVENTORY_SLOT,
        CELL_SELECTED
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

        virtual void update(const float& delta_time) {}

        virtual glm::ivec2 getPosition() const = 0;
        virtual glm::ivec2 getSize() const = 0;
        virtual int getCount() const = 0;
        virtual const Cell* getCells() const = 0;

        virtual bool isVisible() const = 0;
    };

    namespace Renderer
    {
        void Init();
        void Render(gl::ShaderProgram* shader);
        void Update(const float& delta_time);
        void Cleanup();
        void Add(UIComponent* component);
    }

}
