#pragma once

#include "../../gl/Frame.h"
#include "../../math/Transform.h"

namespace world
{
    // forward declaration
    class Entity;

    class Sprite : public Transform
    {
    private:
        gl::TextureMap texture_map;
        gl::Frame dimensions;
        bool flipped;
    public:
        Sprite();
        Sprite(gl::Frame dimensions,gl::TextureMap texture_map, float scale = 1.0f);
        void render(const Entity* entity);

        bool isFlipped();
        void flip();
        void flip(bool f);

        void setTextureMap(gl::TextureMap map);
        void setDimensions(gl::Frame dim);
    };
}
