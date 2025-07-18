#include "Window.h"

void Window::initializeWindow()
{
    this->window = std::make_unique<sf::RenderWindow>(sf::VideoMode({(uint)width, (uint)height}), title);
}

void Window::redrawWindow(const std::vector<sf::Color> &noise_map)
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

void Window::setTerrain(std::unique_ptr<TerrainGenerator> terrain)
{
    this->generator = std::move(terrain);
}

void Window::saveImage(const std::string &filename, const std::vector<sf::Color> &noise_map) const
{
    std::vector<uint8_t> image_data(noise_map.size() * 4);
    oneapi::tbb::parallel_for(
        oneapi::tbb::blocked_range<size_t>(0, noise_map.size()),
        [&image_data, &noise_map](const oneapi::tbb::blocked_range<size_t> &r)
        {
            for (size_t i = r.begin(); i < r.end(); ++i)
            {
                image_data[i * 4 + 0] = noise_map[i].r;
                image_data[i * 4 + 1] = noise_map[i].g;
                image_data[i * 4 + 2] = noise_map[i].b;
                image_data[i * 4 + 3] = noise_map[i].a;
            }
        });

    sf::Image image({pixelsPerRow, pixelsPerColumn}, image_data.data());
    image.saveToFile(filename);
}

void Window::renderLoop()
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
                case sf::Keyboard::Scancode::C:
                {
                    // Parallelize the noise generation using TBB
                    std::cout << "Enter filename: ";
                    std::string saveName = "";
                    std::cin >> saveName;
                    if (saveName.empty())
                    {
                        saveName = "terrain.png";
                    }
                    saveImage(saveName, generator->getColorMap(pixelsPerRow, pixelsPerColumn));
                    std::cout << "Saved " + saveName << std::endl;
                    break;
                }
                }
            }
        }
    }
}
