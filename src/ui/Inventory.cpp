#include "Inventory.h"

#include <imgui.h>

#include "../Application.h"
#include "../resources/Resources.h"
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
            {
                return true;
            }
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

void UI::Inventory::update()
{
    // reset updated status
    updated = false;
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

bool UI::Inventory::isVisible() const
{
    return visible;
}

void UI::Inventory::setVisible(const bool visible)
{
    this->visible = visible;
}

void DrawItem(const world::Material& mat,ImVec2 itemSize)
{
    static SDL_GPUTextureSamplerBinding tileBinding = {0,0};
    static SDL_GPUTextureSamplerBinding itemBinding = {0,0};

    gl::TextureMap uv;
    SDL_GPUTextureSamplerBinding* binding = nullptr;

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
            itemSize,ImVec2(uv.left,uv.top),ImVec2(uv.right,uv.bottom));
    }
}

void UI::Inventory::DrawSlot(const world::Material& mat,int amount,glm::vec2 size)
{
    static SDL_GPUTextureSamplerBinding uiBinding = {0,0};

    ImVec2 cPos = ImGui::GetCursorScreenPos();

    // render cell
    gl::Texture& ui = *res::atlas::uiMap;
    if (!uiBinding.texture) uiBinding = ui.GetBinding();

    ImGui::Image(reinterpret_cast<ImTextureID>(&uiBinding),
        ImVec2(size.x,size.y),ImVec2(0.0f,0.0f),ImVec2(12.0f/256.0f,12.0f/256.0f));

    // render item if item
    if (!mat.isEmpty())
    {
        ImGui::SetCursorScreenPos(ImVec2(cPos.x + 2.0f * GUI_SCALE,cPos.y + 2.0f * GUI_SCALE));
        DrawItem(mat,ImVec2(size.x - 4.0f * GUI_SCALE,size.y - 4.0f * GUI_SCALE));

        // draw item count
        if (amount > 1)
        {
            ImVec2 p_max = ImGui::GetItemRectMax();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            std::string number = std::to_string(amount);

            ImVec2 textSize = ImGui::CalcTextSize(number.c_str());

            ImVec2 textPos = ImVec2(
                p_max.x - textSize.x,
                p_max.y - textSize.y
            );

            draw_list->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0,0,0,255), number.c_str());
            draw_list->AddText(textPos, IM_COL32(255,255,255,255), number.c_str());
        }
    }

    ImGui::SetCursorScreenPos(cPos);
}

void UI::Inventory::CreateSlot(void** slot_ptr)
{
    world::Item*& item = reinterpret_cast<world::Item*&>(*slot_ptr);

    if (item) DrawSlot(item->material,item->getAmount());
    else DrawSlot(world::Material::EMPTY,0);

    ImGui::PushID(slot_ptr);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(1.0f, 1.0f, 1.0f, 0.2f));

    ImGui::Button("##slot",ImVec2(SLOT_SIZE.x,SLOT_SIZE.y));

    if (ImGui::IsItemClicked() && item)
    {
        Application::item_holder.setItem(item);
        item = nullptr;
        updated = true;
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
                        delete data;
                        updated = true;
                    }
                }
            }
            else
            {
                item = Application::item_holder.getItem();
                Application::item_holder.setItem(nullptr);
                updated = true;
            }
        }
    }

    ImGui::PopStyleColor(2);
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
        CreateSlot((void**)(items + i));
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
    ImGuiIO& io = ImGui::GetIO();
    // Scroll selection
    if (Application::GetMouseScroll() != 0 && !io.WantCaptureMouse)
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

UI::PlayerInventory::PlayerInventory(): Inventory(), width(9),height(3), crafting_shift(0)
{
    count = width * height;
    items = new world::Item*[count];
    for (auto i = 0; i < count; ++i)
    {
        items[i] = nullptr;
    }
    ingredients.items = new crafting::Ingredient[getCount()];
    ingredients.amount = 0;
}

UI::PlayerInventory::~PlayerInventory()
{
    delete[] ingredients.items;
}

void UI::PlayerInventory::UpdateRecipies()
{
    uint8_t amount = 0;
    for (auto i = 0; i < getCount(); ++i)
    {
        world::Item*& item = items[i];
        if (item)
        {
            bool found = false;
            for (auto j = 0; j < amount; ++j)
            {
                if (ingredients.items[j].item == item->material)
                {
                    ingredients.items[j].amount += item->getAmount();
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                ingredients.items[amount].item = item->material;
                ingredients.items[amount].amount = item->getAmount();
                ++amount;
            }
        }
    }

    ingredients.amount = amount;

    recipes = crafting::GetRecipe(ingredients);
}

void UI::PlayerInventory::setVisible(bool visible)
{
    if (visible)
    {
        UpdateRecipies();
    }
    Inventory::setVisible(visible);
}

void UI::PlayerInventory::render()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f,10.0f + 12.0f * GUI_SCALE), ImGuiCond_Always);

    ImGui::Begin("Inventory",nullptr,GetWindowFlags());
    ImGui::SeparatorText("Inventory");
    ImGui::PushStyleColor(ImGuiCol_Button,0);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = x + y * width;
            CreateSlot((void**)(items + index));
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    ImGui::SeparatorText("Crafting");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(1.0f, 1.0f, 1.0f, 0.2f));

    constexpr int size = 11;
    constexpr int mid_slots = size / 2;
    int shift = static_cast<int>(recipes.size() / 2) - mid_slots;

    // apply scroll wheel
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        crafting_shift += Application::GetMouseScroll();

    for (int i = 0; i < size; ++i)
    {
        int indexOfRecipe = i + shift + crafting_shift;

        bool invalidIndex = indexOfRecipe < 0 || indexOfRecipe >= recipes.size();
        glm::vec2 slotSize = SLOT_SIZE * ((0.1f - static_cast<float>(abs(mid_slots - i)) / size) + 0.9f);

        if (invalidIndex)
        {
            DrawSlot(world::Material::EMPTY,0,slotSize);
        } else
        {
            DrawSlot(recipes[indexOfRecipe].result.item,recipes[indexOfRecipe].result.amount,slotSize);
        }
        ImGui::PushID(i + ingredients.items);
        ImGui::Button("##slot",ImVec2(slotSize.x,slotSize.y));
        if (ImGui::IsItemClicked() && !Application::item_holder.getItem())
        {
            bool enoughItems = true;
            bool excessItems = true;
            crafting::Ingredients& ing = recipes[indexOfRecipe].ingredients;
            for (auto j = 0; j < ing.amount; ++j)
            {
                int required = ing.items[j].amount;
                int quantity = 0;
                for (auto k = 0; k < ingredients.amount; ++k)
                {
                    if (ingredients.items[k].item == ing.items[j].item)
                    {
                        quantity += ingredients.items[k].amount;
                        break;
                    }
                }
                if (quantity < required)
                {
                    enoughItems = false;
                }
                else if (quantity - required < required)
                {
                    excessItems = false;
                }
            }

            if (enoughItems)
            {
                for (auto j = 0; j < ing.amount; ++j)
                {
                    int required = ing.items[j].amount;
                    for (auto k = 0; k < getCount(); ++k)
                    {
                        world::Item*& item = items[k];
                        if (item && item->material == ing.items[j].item)
                        {
                            int amount = item->getAmount();
                            item->setAmount(amount - required);
                            if (item->getAmount() <= 0)
                            {
                                delete item;
                                item = nullptr;
                            }
                            required -= amount;
                            if (required <= 0) break;
                        }
                    }
                }
                if (!excessItems)
                {
                    recipes[indexOfRecipe] = recipes.back();
                    recipes.pop_back();
                }
                world::Item* result = new world::Item(recipes[indexOfRecipe].result.item);
                result->setAmount(recipes[indexOfRecipe].result.amount);
                Application::item_holder.setItem(result);
            }
        }
        else if (!invalidIndex && ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();

            crafting::Ingredients& ing = recipes[indexOfRecipe].ingredients;
            for (auto j = 0; j < ing.amount; ++j)
            {
                int quantity = -static_cast<int>(ing.items[j].amount);
                for (auto k = 0; k < ingredients.amount; ++k)
                {
                    if (ingredients.items[k].item == ing.items[j].item)
                    {
                        quantity += ingredients.items[k].amount;
                        break;
                    }
                }
                DrawItem(ing.items[j].item,ImVec2(12.0f,12.0f));
                ImGui::SameLine();
                ImGui::Text("%hu",ing.items[j]);
                ImGui::SameLine();
                ImVec4 color(1.0f,1.0f,0.0f,1.0f);
                if (quantity < 0)
                {
                    color = ImVec4(1.0f,0.0f,0.0f,1.0f);
                }
                ImGui::TextColored(color,"(%i)",quantity);
            }

            ImGui::EndTooltip();
        }
        ImGui::PopID();
        ImGui::SameLine();
    }

    ImGui::PopStyleColor(3);
    ImGui::End();
}

void UI::PlayerInventory::update()
{
    if (updated)
    {
        UpdateRecipies();
    }
    Inventory::update();
}
