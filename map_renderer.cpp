#include <oneapi/tbb.h>
#include <iostream>
#include <memory>

#include "sources/Vect3.h"
#include "sources/PerlinNoise.h"
#include "sources/ConstantNoise.h"
#include "sources/Add.h"
#include "sources/Exp.h"
#include "sources/Mult.h"
#include "sources/RidgedNoise.h"
#include "sources/Plane.h"
#include "sources/Renderer.h"

#include "SFML/Graphics/Color.hpp"

constexpr const int WIDTH = 1900;
constexpr const int HEIGHT = 900;

constexpr const int NOISE_WIDTH = 1900;
constexpr const int NOISE_HEIGHT = 900;

constexpr const float MAX_MAP_HEIGHT = 4000.0f;

int main()
{
    int num_threads = oneapi::tbb::info::default_concurrency();
    printf("Number of threads: %d\n", num_threads);

    std::unique_ptr<Terrain> plane = std::make_unique<Plane>(std::make_unique<RidgedNoise>(std::make_unique<Add>(
                                                                                               std::make_unique<Exp>(10.0f, 1.2f, std::make_unique<PerlinNoise>(69, 0.5f, 3)),
                                                                                               std::make_unique<Mult>(
                                                                                                   std::make_unique<PerlinNoise>(69, 0.5f, 3),
                                                                                                   std::make_unique<ConstantNoise>(0.25f)),
                                                                                               2.0f, 1.0f),
                                                                                           8),
                                                             5.0f, MAX_MAP_HEIGHT);

    plane->addBiome(4000, sf::Color::White);
    plane->addBiome(3000, sf::Color(84, 48, 5));
    plane->addBiome(2000, sf::Color(140, 81, 10));
    plane->addBiome(1500, sf::Color(191, 129, 45));
    plane->addBiome(1000, sf::Color(223, 194, 125));
    plane->addBiome(800, sf::Color(128, 205, 193));
    plane->addBiome(600, sf::Color(53, 151, 143));
    plane->addBiome(300, sf::Color(1, 102, 94));
    plane->addBiome(200, sf::Color(0, 60, 48));

    plane->addBiome(150, sf::Color(107, 174, 214));
    plane->addBiome(100, sf::Color(66, 146, 198));
    plane->addBiome(75, sf::Color(33, 113, 181));
    plane->addBiome(50, sf::Color(8, 81, 156));
    plane->addBiome(25, sf::Color(8, 48, 107));

    auto generator = std::make_unique<TerrainGenerator>(std::move(plane));

    Renderer renderer(WIDTH, HEIGHT, "Noisy Terrain", std::move(generator), NOISE_WIDTH, NOISE_HEIGHT);

    renderer.renderLoop();

    return 0;
}
