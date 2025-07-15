#include "Renderer.h"

void Renderer::initializeWindow()
{
    this->window = std::make_unique<sf::RenderWindow>(sf::VideoMode({(uint)width, (uint)height}), title);
}

void Renderer::redrawWindow(const std::vector<sf::Color> &noise_map)
{
    window->clear(sf::Color::Black);

    for (size_t i = 0; i < this->pixelsPerRow; i++)
    {
        for (size_t j = 0; j < pixelsPerColumn; j++)
        {

            sf::RectangleShape pixel(sf::Vector2f(this->width / pixelsPerRow, this->height / pixelsPerColumn));
            pixel.setPosition({static_cast<float>(i) * this->width / pixelsPerRow, static_cast<float>(j) * this->height / pixelsPerColumn});
            pixel.setFillColor(noise_map[j * pixelsPerRow + i]);
            window->draw(pixel);
        }
    }

    window->display();
}

void Renderer::setTerrain(std::unique_ptr<TerrainGenerator> terrain)
{
    this->generator = std::move(terrain);
}

void Renderer::renderLoop()
{
    std::vector<sf::Color> noise_map(pixelsPerRow * pixelsPerColumn, sf::Color::Black);
    while (window->isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window->pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
            {
                window->close();
            }
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                switch (keyPressed->scancode)
                {
                case sf::Keyboard::Scancode::E:
                {
                    std::string filename = "terrain.exr";
                    generator->exportToEXR(filename, this->pixelsPerRow, this->pixelsPerColumn);
                    std::cout << "Exported to " << filename << std::endl;
                    break;
                }
                case sf::Keyboard::Scancode::S:
                {
                    std::cout << "Noise SEQ" << std::endl;
                    redrawWindow(generator->getColorMap(pixelsPerRow, pixelsPerColumn));
                    break;
                }
                case sf::Keyboard::Scancode::P:
                {
                    // Parallelize the noise generation using TBB

                    std::cout << "Noise PARALLEL" << std::endl;
                    redrawWindow(generator->getColorMapParallel(pixelsPerRow, pixelsPerColumn));
                    break;
                }
                case sf::Keyboard::Scancode::G:
                {
                    // Parallelize the noise generation using TBB
                    std::cout << "Noise PARALLEL graph" << std::endl;
                    redrawWindow(generator->getColorMapGraph(pixelsPerRow, pixelsPerColumn));
                    break;
                }
                }
            }
        }
    }
}
