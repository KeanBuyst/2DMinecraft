#include "Inventory.h"

#include "../Application.h"
#include "../world/entities/EntityHandler.h"
#include "../world/entities/Item.h"

UI::Inventory::Inventory(glm::ivec2 position,const int width, const int height) : position(position), width(width), height(height), visible(false)
{
    cells = new Cell[width * height];
    for (auto i = 0; i < height * width; ++i)
    {
        cells[i] = {INVENTORY_SLOT,nullptr};
    }
}

UI::Inventory::~Inventory()
{
    // delete all items as well. Since they only exist in the inventory
    // Thus die with the inventory
    // TODO implement a way to store the inventory to be reloaded if necessary
    for (auto i = 0; i < getCount(); ++i)
    {
        delete cells[i].item;
    }
    delete[] cells;
}

bool UI::Inventory::addItem(world::Item* item) const
{
    for (auto i = 0; i < getCount(); ++i)
    {
        if (cells[i].item == nullptr)
        {
            cells[i].item = item;
            return true;
        }
    }
    return false;
}

glm::ivec2 UI::Inventory::getPosition() const
{
    return position;
}

glm::ivec2 UI::Inventory::getSize() const
{
    return {width,height};
}

int UI::Inventory::getCount() const
{
    return width * height;
}

const UI::Cell* UI::Inventory::getCells() const
{
    return cells;
}

bool UI::Inventory::isVisible() const
{
    return visible;
}

void UI::Inventory::setVisible(const bool visible)
{
    this->visible = visible;
}

// HOTBAR

UI::Hotbar::Hotbar() : Inventory({-4.5f,-13},9,1)
{
    selected_slot = 0;
    cells[0].type = CELL_SELECTED;
}

void UI::Hotbar::update(const float& delta_time)
{
    // Key slot selection
    for (auto i = 0; i < 9; ++i)
    {
        if (Application::isKeyPressed(static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i)))
        {
            setSelectedSlot(i);
            break;
        }
    }
    // Scroll selection
    if (Application::GetMouseScroll() != 0)
    {
        setSelectedSlot(selected_slot - Application::GetMouseScroll());
    }
}

void UI::Hotbar::setSelectedSlot(const int slot)
{
    cells[selected_slot].type = INVENTORY_SLOT;
    selected_slot = slot;
    if (selected_slot < 0) selected_slot += 9;
    if (selected_slot > 8) selected_slot -= 9;
    cells[selected_slot].type = CELL_SELECTED;
}

world::Item*& UI::Hotbar::getSelectedItem() const
{
    return cells[selected_slot].item;
}
