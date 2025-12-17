#pragma once

#include <iostream>

namespace gl
{
    struct Frame
    {
        // offset positions for a given origin
        float left;
        float right;
        float top;
        float bottom;

        void operator*=(float scale);
    };

    typedef Frame TextureMap;

    Frame GetFrame(float pX, float pY, float pWidth, float pHeight);
}