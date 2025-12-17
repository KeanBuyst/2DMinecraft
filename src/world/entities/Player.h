#pragma once

#include "MetaEntity.h"
#include "Sprite.h"
#include "../../Util.h"

namespace world
{
    class Player : public MetaEntity
    {
    private:
        static constexpr float MAX_DIST = 10.0f;
        static constexpr float ARM_ANGLE = Util::DegToRad(30.0f);

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
        void event(SDL_Event* event) const override;
        void update() override;
        void render() override;
        Sprite& getSprite(int index) override;
    };
}
