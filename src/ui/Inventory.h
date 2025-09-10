#pragma once
#include "UI.h"

namespace UI
{
    struct Inventory : UIComponent
    {
    public:
        Inventory(glm::ivec2 position, int width, int height);
        ~Inventory();

        bool addItem(world::Item* item) const;

        glm::ivec2 getPosition() const override;
        glm::ivec2 getSize() const override;
        int getCount() const override;
        const Cell* getCells() const override;

        bool isVisible() const override;
        void setVisible(bool visible) override;
    protected:
        glm::ivec2 position;
        const int width,height;
        bool visible;
        Cell* cells;
    };

    struct Hotbar : Inventory
    {
        Hotbar();

        void update(const float& delta_time) override;
        void setSelectedSlot(int slot);
        world::Item*& getSelectedItem() const;

    private:
        int selected_slot;
    };
}
