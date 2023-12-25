#include<iostream>
#include <SFML/Graphics.hpp>
#include <X11/Xlib.h>

#include <Game.h>

using namespace sf;

void RenderThread(Game* game){
    // Load textures;
    std::cout << "Starting render thread\n";
    Color color(135, 206, 235);
    while (game->window->isOpen())
    {
        game->window->clear(color);
        game->draw();

        game->window->display();
    }
}

void events(){

}

int main(){
    // tell linux that this is a multi-threaded client
    XInitThreads();
    RenderWindow window(VideoMode(600,400),"2D Minecraft");

    // load shaders
    if (!sf::Shader::isAvailable())
    {
        std::cout << "Shaders are not supported by your graphics card\nThis game requires shaders\n";
        return -1;
    }
    sf::Shader shader;
    if (!shader.loadFromFile("shaders/vertex.glsl","shaders/fragment.glsl"))
    {
        std::cout << "Failed to load shader\n";
        return -1;
    }

    Game game(&window,&shader);

    // render thread
    Thread thread(&RenderThread,&game);
    thread.launch();

    sf::Clock clock;
    float dt = 1.f/40.f; // Modify this to change physics rate.
    float accumulator = 0.f;

    while (window.isOpen())
    {
        accumulator += clock.getElapsedTime().asSeconds();
        clock.restart();

        Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case Event::Closed:
                    window.close();
                    break;
            }
            game.event(&event);
        }

        while (accumulator >= dt)
        {
            game.update(accumulator);
            accumulator -= dt;
        }
    }

    return 0;
}