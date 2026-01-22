#include "Inventory.h"

#include <imgui.h>

#include "../Application.h"
#include "../resources/Resources.h"
#include "../world/crafting/Crafting.h"
#include "../world/entities/EntityHandler.h"
#include "../world/entities/Item.h"

constexpr static ImVec2 ITEM_SIZE = ImVec2(8.0f * GUI_SCALE,8.0f * GUI_SCALE);

static ImGuiWindowFlags GetWindowFlags()
{
    return ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
         | ImGuiWindowFlags_NoResize   | ImGuiWindowFlags_NoCollapse
         | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove
         | ImGuiWindowFlags_AlwaysAutoResize;
}

UI::Inventory::~Inventory()
{
    // delete all items as well. Since they only exist in the inventory
    // Thus die with the inventory
    for (auto i = 0; i < count; ++i)
    {
        delete items[i];
    }
    delete[] items;
}

bool UI::Inventory::addItem(world::Item* item)
{
    int emptySlot = -1;
    for (auto i = 0; i < count; ++i)
    {
        world::Item*& slot = items[i];
        if (slot)
        {
            if (slot->combine(item))
                return true;
        } else if (emptySlot == -1)
        {
            emptySlot = i;
        }
    }
    if (emptySlot != -1)
    {
        items[emptySlot] = item;
        return true;
    }
    return false;
}

void UI::Inventory::setItem(const int index, world::Item* item)
{
    if (index < count && index >= 0)
    {
        if (items[index])
        {
            delete items[index];
        }
        items[index] = item;
    } else
    {
        std::cerr << "ERROR: (setItem) invalid slot index" << std::endl;
    }
}

world::Item* UI::Inventory::getItem(int index)
{
    if (index < count && index >= 0)
    {
        return items[index];
    }
    std::cerr << "ERROR: (getItem) invalid slot index" << std::endl;
    return nullptr;
}

int UI::Inventory::getCount() const
{
    return count;
}

void UI::Inventory::serialize(Util::ByteStream& stream)
{
    // store items (other data is static)
    for (auto i = 0; i < getCount(); ++i)
    {
        if (items[i])
        {
            items[i]->serialize(stream);
        }
        else
        {
            uint8_t zero = 0;
            stream << zero;
        }
    }
}

void UI::Inventory::load(Util::ByteStream& stream)
{
    // load items (other data is static)
    for (auto i = 0; i < getCount(); ++i)
    {
        uint8_t type;
        stream >> type;
        if (type != 0)
        {
            if (items[i])
            {
                std::cerr << "Inventory load conflicting with already populated inventory\nOverriding item" << std::endl;
                delete items[i];
            }
            items[i] = new world::Item(stream);
        }
    }
}

void UI::Inventory::update()
{
    /*updated = false;
    if (!visible) return;
    int index;
    if (Application::item_holder.getItem() == nullptr)
    {
        if (Application::isMousePressed(SDL_BUTTON_LEFT))
        {
            if (Cell* cell = GetMouseSlot(index))
            {
                Application::item_holder.setItem(cell->item);
                Application::item_holder.saveLastSlot(this,index);
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
                world::Item* item = Application::item_holder.getItem();
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
                            Application::item_holder.setItem(nullptr);
                            cell->item->setAmount(amount);
                            return;
                        }
                        item->setAmount(overflow);
                        cell->item->setAmount(amount);
                    }
                    Application::item_holder.setLastSlot(item);
                    Application::item_holder.setItem(nullptr);
                }
                else
                {
                    cell->item = item;
                    Application::item_holder.setItem(nullptr);
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
                    world::Item& item = *Application::item_holder.getItem();
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
    }*/
}

bool UI::Inventory::isVisible() const
{
    return visible;
}

void UI::Inventory::setVisible(const bool visible)
{
    this->visible = visible;
}

void UI::Inventory::DrawSlot(void** slot_ptr)
{
    const static ImVec2 slotSize = ImVec2(12.0f * GUI_SCALE,12.0f * GUI_SCALE);

    static SDL_GPUTextureSamplerBinding uiBinding = {0,0};
    static SDL_GPUTextureSamplerBinding tileBinding = {0,0};
    static SDL_GPUTextureSamplerBinding itemBinding = {0,0};

    world::Item*& item = reinterpret_cast<world::Item*&>(*slot_ptr);
    gl::TextureMap uv;
    ImVec2 cPos = ImGui::GetCursorScreenPos();

    // render cell
    gl::Texture& ui = *res::atlas::uiMap;
    if (!uiBinding.texture) uiBinding = ui.GetBinding();

    ImGui::Image(reinterpret_cast<ImTextureID>(&uiBinding),
        slotSize,ImVec2(0.0f,0.0f),ImVec2(12.0f/256.0f,12.0f/256.0f));

    // render item if item
    if (item)
    {
        ImGui::SetCursorScreenPos(ImVec2(cPos.x + 2.0f * GUI_SCALE,cPos.y + 2.0f * GUI_SCALE));

        SDL_GPUTextureSamplerBinding* binding = nullptr;

        const world::Material& mat = item->material;
        uint8_t index = mat.getRaw();
        if (mat.isBlock())
        {
            if (index != 0)
            {
                gl::Texture& tiles = *res::atlas::tiles;
                if (!tileBinding.texture) tileBinding = tiles.GetBinding();
                binding = &tileBinding;

                glm::vec2 pixelPos = glm::vec2((index - 1) % TILES_PER_ROW, (index - 1) / TILES_PER_ROW) * 8.0f;
                uv = tiles.format(pixelPos.x,pixelPos.y,8.0f,8.0f);
            }
        }
        else
        {
            gl::Texture& tiles = *res::atlas::items;
            if (!itemBinding.texture) itemBinding = tiles.GetBinding();
            binding = &itemBinding;

            glm::vec2 pixelPos = glm::vec2(index % TILES_PER_ROW, index / TILES_PER_ROW) * 8.0f;
            uv = tiles.format(pixelPos.x,pixelPos.y,8.0f,8.0f);
        }

        if (binding)
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(binding),
                ITEM_SIZE,ImVec2(uv.left,uv.top),ImVec2(uv.right,uv.bottom));
        }
    }

    ImGui::SetCursorScreenPos(cPos);

    ImGui::PushID(slot_ptr);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(1.0f, 1.0f, 1.0f, 0.2f));

    ImGui::Button("##slot",slotSize);

    if (ImGui::IsItemClicked() && item)
    {
        Application::item_holder.setItem(item);
        item = nullptr;
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::IsMouseReleased(0))
    {
        if (Application::item_holder.getItem())
        {
            if (item)
            {
                if (*item == *Application::item_holder.getItem())
                {
                    world::Item* data = Application::item_holder.getItem();
                    if (item->combine(data))
                    {
                        Application::item_holder.setItem(nullptr);
                    }
                }
            }
            else
            {
                item = Application::item_holder.getItem();
                Application::item_holder.setItem(nullptr);
            }
        }
    }

    ImGui::PopStyleColor(2);

    // draw item count
    if (item && item->getAmount() > 1)
    {
        ImVec2 p_max = ImGui::GetItemRectMax();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        std::string number = std::to_string(item->getAmount());

        float padding = 4.0f;
        ImVec2 textSize = ImGui::CalcTextSize(number.c_str());

        ImVec2 textPos = ImVec2(
            p_max.x - textSize.x - padding,
            p_max.y - textSize.y - padding
        );

        draw_list->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0,0,0,255), number.c_str());
        draw_list->AddText(textPos, IM_COL32(255,255,255,255), number.c_str());
    }


    ImGui::PopID();
}

// HOTBAR

UI::Hotbar::Hotbar() : Inventory()
{
    selected_slot = 0;

    count = 9;
    items = new world::Item*[count];
    for (auto i = 0; i < count; ++i)
    {
        items[i] = nullptr;
    }
}

void UI::Hotbar::render()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f,10.0f), ImGuiCond_Always);

    ImGui::Begin("Hotbar",nullptr,GetWindowFlags());

    for (int i = 0; i < count; ++i)
    {
        ImVec4 color;
        if (selected_slot == i)
        {
            color = ImVec4(1.0f,1.0f,0.0f,0.1f);
        }
        ImGui::PushStyleColor(ImGuiCol_Button,color);
        DrawSlot((void**)(items + i));
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    ImGui::End();
}

void UI::Hotbar::update()
{
    Inventory::update();
    // Key slot selection
    for (auto i = 0; i < count; ++i)
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
    selected_slot = slot;
    if (selected_slot < 0) selected_slot += 9;
    if (selected_slot > 8) selected_slot -= 9;
}

world::Item*& UI::Hotbar::getSelectedItem() const
{
    return items[selected_slot];
}

UI::MouseItemHolder::MouseItemHolder() : Inventory()
{
    count = 1;
    items = new world::Item*[1];
    items[0] = nullptr;

    visible = true;
    lastSlot = 0;
    lastInv = nullptr;
}

void UI::MouseItemHolder::setItem(world::Item* item)
{
    items[0] = item;
}

void UI::MouseItemHolder::saveLastSlot(Inventory* last,const int slot)
{
    lastSlot = slot;
    lastInv = last;
}

world::Item* UI::MouseItemHolder::getItem() const
{
    return items[0];
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

void UI::MouseItemHolder::render()
{
    static SDL_GPUTextureSamplerBinding tileBinding = {0,0};
    static SDL_GPUTextureSamplerBinding itemBinding = {0,0};

    if (items[0])
    {
        gl::TextureMap uv;
        world::Material& mat = items[0]->material;
        uint8_t index = mat.getRaw();

        ImDrawList* drawList = ImGui::GetForegroundDrawList();

        SDL_GPUTextureSamplerBinding* binding;
        if (mat.isBlock())
        {
            gl::Texture& tiles = *res::atlas::tiles;
            if (!tileBinding.texture) tileBinding = tiles.GetBinding();
            binding = &tileBinding;

            glm::vec2 pixelPos = glm::vec2((index - 1) % TILES_PER_ROW, (index - 1) / TILES_PER_ROW) * 8.0f;
            uv = tiles.format(pixelPos.x,pixelPos.y,8.0f,8.0f);
        } else
        {
            gl::Texture& tiles = *res::atlas::items;
            if (!itemBinding.texture) itemBinding = tiles.GetBinding();
            binding = &itemBinding;

            glm::vec2 pixelPos = glm::vec2(index % TILES_PER_ROW, index / TILES_PER_ROW) * 8.0f;
            uv = tiles.format(pixelPos.x,pixelPos.y,8.0f,8.0f);
        }

        ImVec2 mousePos = ImGui::GetMousePos();
        mousePos.x -= ITEM_SIZE.x / 2;
        mousePos.y -= ITEM_SIZE.y / 2;

        ImVec2 p_min = mousePos;
        ImVec2 p_max = ImVec2(mousePos.x + ITEM_SIZE.x, mousePos.y + ITEM_SIZE.y);

        drawList->AddImage(reinterpret_cast<ImTextureID>(binding),p_min,p_max,
            ImVec2(uv.left,uv.top),ImVec2(uv.right,uv.bottom));
    }
}

void UI::MouseItemHolder::update()
{}

UI::PlayerInventory::PlayerInventory(): Inventory(), width(9),height(3)
{
    count = width * height;
    items = new world::Item*[count];
    for (auto i = 0; i < count; ++i)
    {
        items[i] = nullptr;
    }
}

void UI::PlayerInventory::render()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f,10.0f + 12.0f * GUI_SCALE), ImGuiCond_Always);

    ImGui::Begin("Inventory",nullptr,GetWindowFlags());

    ImGui::SeparatorText("Inventory");

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = x + y * width;
            DrawSlot((void**)(items + index));
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    ImGui::End();
}

void UI::PlayerInventory::update()
{
    /*bool previous;
    world::Item*& c5 = at(4,5);
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
            Application::item_holder.addItem(c5);
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
    }*/
}
