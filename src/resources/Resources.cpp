#include "Resources.h"

gl::Texture* res::atlas::tiles = nullptr;
gl::Texture* res::atlas::items = nullptr;
gl::Texture* res::atlas::uiMap = nullptr;
gl::Texture* res::atlas::entities = nullptr;

void res::load()
{
    atlas::tiles = new gl::Texture("../assets/tiles.png");
    atlas::items = new gl::Texture("../assets/items.png");
    atlas::uiMap = new gl::Texture("../assets/ui.png");
    atlas::entities = new gl::Texture("../assets/entities.png");

    atlas::tiles->bind(0);
    atlas::entities->bind(1);
    atlas::items->bind(2);
    atlas::uiMap->bind(3);
}

void res::clear()
{
    delete atlas::tiles;
    delete atlas::items;
    delete atlas::uiMap;
    delete atlas::entities;

    atlas::tiles = nullptr;
    atlas::items = nullptr;
    atlas::uiMap = nullptr;
    atlas::entities = nullptr;
}