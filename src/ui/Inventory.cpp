#include "Inventory.h"

#include "../Application.h"
#include "../world/crafting/Crafting.h"
#include "../world/entities/EntityHandler.h"
#include "../world/entities/Item.h"

UI::Inventory::Inventory(glm::vec2 position,const int width, const int height)
    : position(position), width(width), height(height), updated(false), visible(false)
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

bool UI::Inventory::addItem(world::Item* item)
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

void UI::Inventory::setItem(const int index, world::Item* item)
{
    if (index < getCount() && index >= 0)
    {
        cells[index].item = item;
    } else
    {
        std::cerr << "ERROR: (setItem) invalid slot index" << std::endl;
    }
}

world::Item* UI::Inventory::getItem(int index)
{
    if (index < getCount() && index >= 0)
    {
        return cells[index].item;
    }
    std::cerr << "ERROR: (getItem) invalid slot index" << std::endl;
    return nullptr;
}

glm::vec2 UI::Inventory::getPosition() const
{
    return position;
}

void UI::Inventory::setPosition(glm::vec2 position)
{
    this->position = position;
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

void UI::Inventory::update()
{
    updated = false;
    if (!visible) return;
    int index;
    if (Application::item_holder->getItem() == nullptr)
    {
        if (Application::isMousePressed(SDL_BUTTON_LEFT))
        {
            if (Cell* cell = GetMouseSlot(index))
            {
                Application::item_holder->setItem(cell->item);
                Application::item_holder->saveLastSlot(this,index);
                cell->item = nullptr;
                updated = true;
            }
        }
    }
    else
    {
        if  (Application::isMouseReleased(SDL_BUTTON_LEFT))
        {
            if (Cell* cell = GetMouseSlot(index))
            {
                world::Item* item = Application::item_holder->getItem();
                if (cell->item != nullptr)
                {
                    if (*item == *cell->item)
                    {
                        int amount = item->getAmount() + cell->item->getAmount();
                        const int limit = item->getStackLimit();
                        int overflow = 0;
                        if (amount > limit)
                        {
                            overflow = amount - limit;
                            amount = limit;
                        }
                        if (overflow == 0)
                        {
                            delete item;
                            Application::item_holder->setItem(nullptr);
                            cell->item->setAmount(amount);
                            return;
                        }
                        item->setAmount(overflow);
                        cell->item->setAmount(amount);
                    }
                    Application::item_holder->setLastSlot(item);
                    Application::item_holder->setItem(nullptr);
                }
                else
                {
                    cell->item = item;
                    Application::item_holder->setItem(nullptr);
                }
                updated = true;
            }
        }
        else if (Application::isMouseDown(SDL_BUTTON_RIGHT))
        {
            if (Cell* cell = GetMouseSlot(index))
            {
                if (cell->item == nullptr)
                {
                    world::Item& item = *Application::item_holder->getItem();
                    if (item.getAmount() > 1)
                    {
                        auto* newItem = new world::Item(item);
                        newItem->setAmount(1);
                        cell->item = newItem;
                        item.setAmount(item.getAmount() - 1);
                        updated = true;
                    }
                }
            }
        }
    }
}

bool UI::Inventory::isVisible() const
{
    return visible;
}

void UI::Inventory::setVisible(const bool visible)
{
    this->visible = visible;
}

UI::Cell& UI::Inventory::at(int x, int y)
{
    return cells[y * width + x];
}

UI::Cell* UI::Inventory::GetMouseSlot(int& index)
{
    glm::vec2 pos = Application::GetUIMouse() - position;
    index = static_cast<int>(floorf(pos.y)) * width + static_cast<int>(floorf(pos.x));
    if (index < 0 || index >= getCount()) return nullptr;
    if (isCell(cells[index].type)) return nullptr;
    return &cells[index];
}

// HOTBAR

UI::Hotbar::Hotbar() : Inventory({-4.5f,-13.0f},9,1)
{
    selected_slot = 0;
    cells[0].type = SELECTED_SLOT;
}

void UI::Hotbar::update()
{
    Inventory::update();
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
    cells[selected_slot].type = SELECTED_SLOT;
}

world::Item*& UI::Hotbar::getSelectedItem() const
{
    return cells[selected_slot].item;
}

UI::MouseItemHolder::MouseItemHolder() : Inventory({0,0},1,1)
{
    cells[0].type = EMPTY_CELL;
    visible = true;
    lastSlot = 0;
    lastInv = nullptr;
}

void UI::MouseItemHolder::setItem(world::Item* item)
{
    cells[0].item = item;
}

void UI::MouseItemHolder::saveLastSlot(Inventory* last,const int slot)
{
    lastSlot = slot;
    lastInv = last;
}

world::Item* UI::MouseItemHolder::getItem() const
{
    return cells[0].item;
}

void UI::MouseItemHolder::setLastSlot(world::Item*& item) const
{
    if (lastInv == nullptr) return;
    world::Item* current = lastInv->getItem(lastSlot);
    if (current != nullptr)
    {
        current->setAmount(current->getAmount() + item->getAmount());
        delete item;
        item = nullptr;
    }
    else lastInv->setItem(lastSlot,item);
}

void UI::MouseItemHolder::update()
{}

UI::PlayerInventory::PlayerInventory(): Inventory({-4.5f,-11.5f},9,7)
{
    // set blank cells
    for (auto y = 3; y < 7; ++y)
    {
        for (auto x = 0; x < 6; ++x)
        {
            at(x,y).type = BLANK_CELL;
        }
    }
    // set armour slots
    at(8,6).type = HELMET_SLOT;
    at(8,5).type = CHESTPLATE_SLOT;
    at(8,4).type = LEGGINGS_SLOT;
    at(8,3).type = BOOTS_SLOT;
    // set trinket slots
    for (auto i = 0; i < 2; ++i)
    {
        at(6 + i,6).type = HAT_SLOT;
        at(6 + i,5).type = NECKLACE_SLOT;
        at(6 + i,4).type = RING_SLOT;
        at(6 + i,3).type = SHOES_SLOT;
    }

    // set crafting
    at(1,5).type = INVENTORY_SLOT;
    at(1,4).type = INVENTORY_SLOT;
    at(2,5).type = INVENTORY_SLOT;
    at(2,4).type = INVENTORY_SLOT;
    at(3,5).type = ARROW_CELL;
    at(4,5).type = INVENTORY_SLOT;
}

void UI::PlayerInventory::update()
{
    bool previous;
    world::Item*& c5 = at(4,5).item;
    if (c5) previous = true;

    Inventory::update();

    if (updated)
    {
        world::Item*& c1 = at(1,5).item;
        world::Item*& c2 = at(1,4).item;
        world::Item*& c3 = at(2,5).item;
        world::Item*& c4 = at(2,4).item;

        if (previous && !c5)
        {
            world::RemoveAmount(c1,1);
            world::RemoveAmount(c2,1);
            world::RemoveAmount(c3,1);
            world::RemoveAmount(c4,1);
        }
        else if (!previous && c5)
        {
            // prevents placing items in crafting output slot
            Application::item_holder->addItem(c5);
            c5 = nullptr;
        }

        int size = (c1 ? 1 : 0) + (c2 ? 1 : 0) + (c3 ? 1 : 0) + (c4 ? 1 : 0);
        if (size)
        {
            world::Crafting::Recipe recipe(size == 1 ? 1 : 2);
            if (c1) recipe.at({0,0}) = c1->material;
            if (c2) recipe.at({0,1}) = c2->material;
            if (c3) recipe.at({1,0}) = c3->material;
            if (c4) recipe.at({1,1}) = c4->material;

            world::Item* result = world::Crafting::GetRecipe(recipe);

            delete c5;
            if (result)
                c5 = result;
            else c5 = nullptr;

        } else if (c5)
        {
            delete c5;
            c5 = nullptr;
        }
    }
}
