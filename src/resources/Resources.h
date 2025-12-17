#pragma once
#include "../gl/Texture.h"

namespace res
{
    namespace atlas
    {
        extern gl::Texture* tiles;
        extern gl::Texture* items;
        extern gl::Texture* uiMap;
        extern gl::Texture* entities;
    }

    void load();
    void clear();
}
