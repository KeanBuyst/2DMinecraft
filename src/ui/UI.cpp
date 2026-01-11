#include "UI.h"

#include "../Util.h"
#include "../glew.h"
#include "../world/entities/Item.h"

static GLuint VAO;
static GLuint VBO;

std::vector<UI::UIComponent*> buffer;

struct RenderCell
{
    glm::vec2 position;
    uint32_t type;
    int item;
    int stack_size;
};

void UI::Renderer::Init()
{
    buffer.reserve(5);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RenderCell),
        reinterpret_cast<void*>(offsetof(RenderCell, position)));
    glEnableVertexAttribArray(0);

    // type attribute
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(RenderCell),
        reinterpret_cast<void*>(offsetof(RenderCell, type)));
    glEnableVertexAttribArray(1);

    // item attribute
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(RenderCell),
        reinterpret_cast<void*>(offsetof(RenderCell, item)));
    glEnableVertexAttribArray(2);

    // stack size attribute
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(RenderCell),
        reinterpret_cast<void*>(offsetof(RenderCell, stack_size)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void UI::Renderer::Update()
{
    for (UIComponent*& comp : buffer)
    {
        comp->update();
    }
}

void UI::Renderer::Cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void UI::Renderer::Add(UIComponent* component)
{
    buffer.push_back(component);
}

void UI::Renderer::Render(gl::Shader* shader)
{
    static std::vector<RenderCell> render_cells;
    render_cells.clear();

    shader->useTexture("tileAtlas",0);
    shader->useTexture("itemAtlas",2);
    shader->useTexture("uiAtlas",3);

    for (UIComponent*& comp : buffer)
    {
        if (!comp->isVisible()) continue;

        const Cell* cells = comp->getCells();
        for (auto i = 0; i < comp->getCount(); ++i)
        {
            // ignore empty cells
            if (cells[i].type == EMPTY_CELL && cells[i].item == nullptr)
                continue;

            int amount = 0;

            int item;
            if (cells[i].item == nullptr)
                item = -1;
            else
            {
                item = cells[i].item->material.getRenderData();
                amount = cells[i].item->getAmount();
            }
            const int width = comp->getSize().x;
            glm::vec2 position = glm::vec2(i % width, i / width) + comp->getPosition();
            render_cells.push_back({
                position * CELL_SIZE,
                cells[i].type,
                item,
                amount
            });
        }
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER,render_cells.size() * sizeof(RenderCell), render_cells.data(),GL_STATIC_DRAW);
    glDrawArrays(GL_POINTS,0,render_cells.size());
}

bool UI::isCell(const CellType& type)
{
    switch (type)
    {
    case EMPTY_CELL:
    case ARROW_CELL:
    case BLANK_CELL:
        return true;
    default:
        return false;
    }
}

bool UI::isSlot(const CellType& type)
{
    return !isCell(type);
}
