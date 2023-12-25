#pragma once

#include <Resources.h>

namespace MetaData
{
    struct StaticMeta
    {
        const char* name;
        bool colliable;
        bool transparent;
    };

    StaticMeta GetStaticMeta(Material mat);
}