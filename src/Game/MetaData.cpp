#include "MetaData.h"

using namespace MetaData;

StaticMeta MetaData::GetStaticMeta(Material mat){
    switch (mat)
    {
    case GRASS:
        return
        {
            "Grass",
            false,
            true,
            0u
        };

    default:
        return
        {
            "N/A",
            true,
            false,
            100u
        };
    }
}