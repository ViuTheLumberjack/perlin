#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>
#include <iostream>
#include <oneapi/tbb.h>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

#include "TerrainGenerator.h"

#pragma once

class Renderer
{
public:
    Renderer(const int width, const int height, const std::string &title, std::unique_ptr<TerrainGenerator> generator, const int pixelWidth = 1, const int pixelHeight = 1)
        : width(width), height(height), pixelsPerRow(pixelWidth), pixelsPerColumn(pixelHeight), title(title), generator(std::move(generator))
    {
        initializeWindow();
    }

    void setTerrain(std::unique_ptr<TerrainGenerator> terrain);

    void renderLoop();

    ~Renderer() {};

private:
    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<TerrainGenerator> generator;

    int width;
    int height;
    int pixelsPerRow;
    int pixelsPerColumn;
    std::string title;

    void initializeWindow();
    void redrawWindow(const std::vector<sf::Color> &noise_map);
};

#endif