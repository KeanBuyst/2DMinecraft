#pragma once
#include "UI.h"

namespace UI
{
    struct Inventory : UIComponent
    {
        glm::ivec2 position;
        const int width,height;
        bool visible;
        Cell* cells;

        Inventory(glm::ivec2 position, int width, int height);
        ~Inventory();

        bool addItem(world::Item* item) const;

        glm::ivec2 getPosition() const override;
        glm::ivec2 getSize() const override;
        int getCount() const override;
        const Cell* getCells() const override;

        bool isVisible() const override;
    };
}
