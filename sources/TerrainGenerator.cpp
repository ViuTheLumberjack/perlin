#include "TerrainGenerator.h"

std::vector<float> TerrainGenerator::getHeightMap(int width, int height) const
{
    auto heightMap = std::vector<float>(width * height);

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            heightMap[y * width + x] = this->terrain->getHeight((float)x / width, (float)y / height);
        }
    }

    return heightMap;
}

std::vector<sf::Color> TerrainGenerator::getColorMap(int width, int height) const
{
    auto colorMap = std::vector<sf::Color>(width * height);

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            colorMap[y * width + x] = this->terrain->getColor((float)x / width, (float)y / height);
        }
    }

    return colorMap;
}

std::vector<sf::Color> TerrainGenerator::getColorMapOMP(int width, int height) const
{
    auto colorMap = std::vector<sf::Color>(width * height);

#pragma omp parallel for collapse(2)
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            colorMap[y * width + x] = this->terrain->getColor((float)x / width, (float)y / height);
        }
    }

    return colorMap;
}

std::vector<sf::Color> TerrainGenerator::getColorMapParallel(int width, int height) const
{
    auto colorMap = std::vector<sf::Color>(width * height);

    oneapi::tbb::parallel_for(
        oneapi::tbb::blocked_range<int>(0, width * height),
        [this, &colorMap, width, height](oneapi::tbb::blocked_range<int> const &r) -> void
        {
            for (int i = r.begin(); i < r.end(); ++i)
            {
                int x = i % width;
                int y = i / width;

                colorMap[y * width + x] = this->terrain->getColor((float)x / width, (float)y / height);
            }
        });

    return colorMap;
}

std::vector<sf::Color> TerrainGenerator::getColorMapGraph(int width, int height) const
{
    auto colorMap = std::vector<sf::Color>(width * height);

    oneapi::tbb::parallel_for(
        oneapi::tbb::blocked_range<int>(0, width * height),
        [this, &colorMap, width, height](oneapi::tbb::blocked_range<int> const &r) -> void
        {
            auto computationG = this->terrain->getGraph();

            for (int i = r.begin(); i < r.end(); ++i)
            {
                int x = i % width;
                int y = i / width;

                computationG.input_node->try_put(Vect3((float)x / width, (float)y / height, 0));

                computationG.graph->wait_for_all();

                float heightValue = 0;
                computationG.output_node->try_get(heightValue);
                colorMap[y * width + x] = this->terrain->getBiomeColor(heightValue);
            }
        });

    return colorMap;
}

void TerrainGenerator::exportToEXR(const std::string &filename, int width, int height) const
{
    Imf::Array2D<Imf::Rgba> pixels(width, height);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Vect3 position(static_cast<float>(x) / width, static_cast<float>(y) / height, 0);
            float rHeight = this->terrain->getHeight(position.getX(), position.getY());
            pixels[x][y] = Imf::Rgba(rHeight / 255.0f,
                                     rHeight / 255.0f,
                                     rHeight / 255.0f);
        }
    }

    Imf::RgbaOutputFile file(filename.c_str(), width, height, Imf::WRITE_RGBA);
    file.setFrameBuffer(&pixels[0][0], 1, width);
    file.writePixels(height);
}
