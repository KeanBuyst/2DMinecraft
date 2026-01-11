#pragma once

#include "Entity.h"
#include "Sprite.h"
#include "../../Util.h"
//#include "../../ui/Inventory.h"

namespace world
{
    class Player : public Entity
    {
    private:
        static constexpr float MAX_DIST = 10.0f;
        static constexpr float ARM_ANGLE = Util::DegToRad(30.0f);

        //UI::Hotbar hotbar;
        //UI::PlayerInventory inventory;
        Sprite sprites[5];
        // block the player is attempting to break's state
        float break_state;
        float arm_rotation;
        float arm_dir;
        float leg_dir;
    public:
        Player(glm::vec2 position);
        Player(Util::ByteStream& stream);
        ~Player() override;
        //void drop(Item* item);
        void update() override;
        void render() override;
        void onCollision(Entity* other) override;
        void serialize(Util::ByteStream& stream) override;
        Sprite& getSprite(int index) override;

        //UI::Hotbar& getHotbar();
        //UI::PlayerInventory& getInventory();
    };
}
