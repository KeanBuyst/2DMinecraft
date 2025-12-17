#include "Player.h"

#include "EntityHandler.h"
#include "Procedure.h"
#include "../../Application.h"
#include "../../Util.h"
#include "../../resources/Resources.h"
#include "Item.h"

static gl::Frame upper_frame = gl::GetFrame(0,10,9,8);
static gl::Frame mid_frame = gl::GetFrame(0,0,4,12);
static gl::Frame lower_frame = gl::GetFrame(0,-12,4,12);

REGISTER_ENTITY(world::EntityType::PLAYER, world::Player);

world::Player::Player(Util::ByteStream& stream)
: MetaEntity(EntityType::PLAYER,gl::GetFrame(0.0f, -3.0f, 5.0f, 32.0f),stream),
    sprites{
        Sprite(upper_frame, res::atlas::entities->format(0, 0, 8, 7)), // head
        Sprite(mid_frame, res::atlas::entities->format(4, 8, 4, 12)), // arm
        Sprite(mid_frame, res::atlas::entities->format(0, 8, 4, 12)), // body
        Sprite(lower_frame, res::atlas::entities->format(8, 8, 4, 12)), // leg 1
        Sprite(lower_frame, res::atlas::entities->format(8, 8, 4, 12)), // leg 2
}, break_state(0.0f), arm_rotation(0.0f), arm_dir(4.0f), leg_dir(-1.0f)
{
    numOfInventories = 2;
    inventories = new UI::Inventory*[2] {
        new UI::Hotbar(),
        new UI::PlayerInventory()
   };

    // set up pivot points
    sprites[0].pivot_point = {0,sprites[0].position.y};
    sprites[1].pivot_point = {0, 0.25};
    const glm::vec2 pivot(0, -0.4f);
    sprites[3].pivot_point = pivot;
    sprites[4].pivot_point = pivot;
}

world::Player::Player(glm::vec2 position)
    : MetaEntity(position, gl::GetFrame(0.0f, -3.0f, 5.0f, 32.0f), EntityType::PLAYER),
      sprites{
          Sprite(upper_frame, res::atlas::entities->format(0, 0, 8, 7)), // head
          Sprite(mid_frame, res::atlas::entities->format(4, 8, 4, 12)), // arm
          Sprite(mid_frame, res::atlas::entities->format(0, 8, 4, 12)), // body
          Sprite(lower_frame, res::atlas::entities->format(8, 8, 4, 12)), // leg 1
          Sprite(lower_frame, res::atlas::entities->format(8, 8, 4, 12)), // leg 2
      }, break_state(0.0f), arm_rotation(0.0f), arm_dir(4.0f), leg_dir(-1.0f)
{
    numOfInventories = 2;
    inventories = new UI::Inventory*[2] {
         new UI::Hotbar(),
         new UI::PlayerInventory()
    };
    inventories[0]->setVisible(true);
    UI::Renderer::Add(inventories[0]);
    UI::Renderer::Add(inventories[1]);

    // set up pivot points
    sprites[0].pivot_point = {0,sprites[0].position.y};
    sprites[1].pivot_point = {0, 0.25};
    const glm::vec2 pivot(0, -0.4f);
    sprites[3].pivot_point = pivot;
    sprites[4].pivot_point = pivot;

    // test items
    inventories[0]->addItem(new Item(DIAMOND_PICKAXE));
}

world::Player::~Player()
{}

void world::Player::event(SDL_Event* event) const
{}

void world::Player::update()
{
    constexpr float SPEED = 8.0f;

    Procedure::HitBoxResult collision = Procedure::HitBox(this,dimensions);

    if (Application::isKeyPressed(SDL_SCANCODE_E))
    {
        inventories[1]->setVisible(!inventories[1]->isVisible());
    }
    if (Application::isKeyDown(SDL_SCANCODE_A))
    {
        if (acceleration.x == 0) velocity.x = -SPEED / 2.0f;

        acceleration.x = -SPEED;
        appliedMovement = true;
    }
    if (Application::isKeyDown(SDL_SCANCODE_D))
    {
        // creates more instantaneous movement
        if (acceleration.x == 0) velocity.x = SPEED / 2.0f;

        acceleration.x = SPEED;
        appliedMovement = true;
    }
    if (Application::isKeyPressed(SDL_SCANCODE_W) && collision.bottom)
    {
        velocity.y = 20.0f;
    }
    if (Application::isKeyUp(SDL_SCANCODE_A) || Application::isKeyUp(SDL_SCANCODE_D))
    {
        acceleration.x = 0;
        appliedMovement = false;
    }

    velocity += acceleration * delta_time;
    Procedure::RigidBody(this,collision);
    position += velocity * delta_time;

    // Inventory actions
    // dropping item
    if (Application::isKeyPressed(SDL_SCANCODE_Q))
    {
        Item*& item = reinterpret_cast<UI::Hotbar*>(inventories[0])->getSelectedItem();
        if (item != nullptr)
        {
            if (item->getAmount() > 1)
            {
                item->setAmount(item->getAmount() - 1);
                auto* newItem = new Item(*item);
                newItem->setAmount(1);
                newItem->position = position;
                newItem->velocity = velocity * 10.0f;
                newItem->toEntity();
                EntityHandler::Add(newItem);
            } else
            {
                // pass ownership over to the entity handler
                item->position = position;
                item->rotation = 0.0f;
                item->velocity = velocity * 10.0f;
                item->toEntity();
                EntityHandler::Add(item);
                item = nullptr;
            }
        }
    }
    if (Application::isMouseDown(SDL_BUTTON_LEFT))
    {
        // break block
        if (Application::item_holder->getItem() == nullptr)
        {
            const glm::vec2 pos = Application::GetWorldMouse();
            const float distance = Util::Distance2(origin, pos);
            if (distance <= MAX_DIST)
            {
                Block block = m_world.getBlock(pos);
                if (block.getType() != EMPTY)
                {
                    int state = block.getBreakState();
                    Item*& item = reinterpret_cast<UI::Hotbar*>(inventories[0])->getSelectedItem();

                    Tool tool;
                    if (item == nullptr)
                    {
                        tool = Tool(STICK); // stick is equivalent to hand
                    } else
                    {
                        tool = item->getTool();
                    }
                    break_state += tool.getToolBonus(block.getType()) * delta_time;

                    if (break_state >= 1.0f)
                    {
                        ++state;
                        --break_state;

                        if (state > 10)
                        {
                            const glm::vec2 dropPoint(block.position.x + 0.5f, block.position.y + 0.5f);
                            auto* newItem = new Item(dropPoint,block.getType());
                            block.setBreakState(0);
                            block.setType(EMPTY);
                            m_world.setBlock(block,true);
                            EntityHandler::Add(newItem);
                        } else
                        {
                            block.setBreakState(state);
                            m_world.setBlock(block);
                        }
                    }
                }
            }
            // item swinging animation
            arm_rotation += arm_dir * delta_time;
            if (arm_rotation > ARM_ANGLE || arm_rotation < -ARM_ANGLE)
                arm_dir = -arm_dir;
        }
    }
    if (Application::isMouseReleased(SDL_BUTTON_LEFT))
    {
        Item* item = Application::item_holder->getItem();
        if (item != nullptr)
        {
            // also drop item
            item->position = position;
            item->rotation = 0;
            item->velocity = position * 10.0f;
            item->toEntity();
            EntityHandler::Add(item);
            Application::item_holder->setItem(nullptr);
        } else
        {
            // reset block break state
            const glm::vec2 pos = Application::GetWorldMouse();
            const float distance = Util::Distance2(origin, pos);
            if (distance <= MAX_DIST)
            {
                Block block = m_world.getBlock(pos);
                block.setBreakState(0);
                m_world.setBlock(block);
            }
            break_state = 0.0f;
        }
        arm_rotation = 0.0f;
    }
    // block placing
    if (Application::isMousePressed(SDL_BUTTON_RIGHT))
    {
        if (Application::item_holder->getItem() == nullptr)
        {
            Item*& item = reinterpret_cast<UI::Hotbar*>(inventories[0])->getSelectedItem();
            if (item != nullptr && item->material.isBlock())
            {
                const glm::vec2 pos = Application::GetWorldMouse();
                const float distance = Util::Distance2(origin, pos);
                if (distance <= MAX_DIST)
                {
                    Block block = m_world.getBlock(pos);
                    if (block.getType() == EMPTY)
                    {
                        block.setType(item->material);
                        RemoveAmount(item,1);
                        m_world.setBlock(block);
                    }
                }
            }
        }
    }

    // head animation
    const glm::vec2 mousePos = Application::GetWorldMouse();
    float head_rotation = atan2f(mousePos.y - (position.y + sprites[0].position.y),
        mousePos.x - (position.x + sprites[0].position.x));

    static constexpr float MAX_ANGLE = Util::DegToRad(40.0f);
    static constexpr float MID_ANGLE = Util::DegToRad(90.0f);

    if (head_rotation > MID_ANGLE) head_rotation = M_PI - head_rotation;
    if (head_rotation < -MID_ANGLE) head_rotation = -static_cast<float>(M_PI) - head_rotation;

    Util::clamp(head_rotation,MAX_ANGLE);

    sprites[0].rotation = head_rotation;

    // leg animations
    float ratio = abs(velocity.x) / MAX_SPEED;
    float angle_ratio = ratio * 3.0f;
    float speed_ratio = ratio * 8.0f;
    float vector = leg_dir * speed_ratio * delta_time;

    if (vector == 0.0f)
    {
        sprites[3].rotation = 0.0f;
        sprites[4].rotation = 0.0f;
    } else
    {
        sprites[3].rotation += vector;
        sprites[4].rotation -= vector;
    }
    float angle = MAX_ANGLE * angle_ratio;
    if (sprites[3].rotation > angle)
    {
        leg_dir = -1;
        sprites[3].rotation = angle;
        sprites[4].rotation = -angle;
    } else if (sprites[3].rotation < -angle)
    {
        leg_dir = 1;
        sprites[3].rotation = -angle;
        sprites[4].rotation = angle;
    }
}

void world::Player::render()
{
    for (Sprite& sprite : sprites)
    {
        sprite.render(this);
    }
    // holding item render
    Item*& item = reinterpret_cast<UI::Hotbar*>(inventories[0])->getSelectedItem();
    if (item == nullptr)
    {

        sprites[1].rotation = 0.0f;
        return;
    }
    sprites[1].rotation = ARM_ANGLE + arm_rotation;
    float net_rotation = ARM_ANGLE + arm_rotation;
    float x = 0.5f;
    float y = -0.3f;
    if (!item->material.isBlock())
    {
        net_rotation -= Util::DegToRad(30.0f);
        x = 0.6f;
        y = 0.15f;
    }
    glm::vec2 net_position = Util::rotate(sprites[1].position + pivot_point + glm::vec2(sprites[1].isFlipped() ? -x : x,y), sprites[1].isFlipped() ? -net_rotation : net_rotation);

    item->position = net_position + position;
    item->rotation = net_rotation;
    item->getSprite(0).flip(sprites[1].isFlipped());
    item->render();
}

world::Sprite& world::Player::getSprite(int index)
{
    if (index < 0 || index > 4)
        throw "Player: Invalid sprite index";
    return sprites[index];
}