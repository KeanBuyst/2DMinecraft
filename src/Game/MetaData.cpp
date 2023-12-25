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
            true
        };

    default:
        return
        {
            "N/A",
            true,
            false
        };
    }
}