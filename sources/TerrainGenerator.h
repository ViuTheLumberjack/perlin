#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include <memory>
#include <vector>
#include <string>

#include <ImfRgbaFile.h>
#include <ImfArray.h>

#include <SFML/Graphics/Color.hpp>

#include "Terrain.h"

#pragma once

class TerrainGenerator
{
public:
    TerrainGenerator(std::unique_ptr<Terrain> terrain)
        : terrain(std::move(terrain)) {};

    std::vector<float> getHeightMap(int width, int height) const;
    std::vector<sf::Color> getColorMap(int width, int height) const;
    std::vector<sf::Color> getColorMapOMP(int width, int height) const;
    std::vector<sf::Color> getColorMapGraph(int width, int height) const;
    std::vector<sf::Color> getColorMapParallel(int width, int height) const;

    void exportToEXR(const std::string &filename, int width, int height) const;

    ~TerrainGenerator() {};

private:
    std::unique_ptr<Terrain> terrain;
};

#endif