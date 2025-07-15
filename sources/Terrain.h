#ifndef TERRAIN_H
#define TERRAIN_H

#include <memory>
#include <vector>
#include <oneapi/tbb.h>
#include <SFML/Graphics/Color.hpp>

#include "NoiseFunction.h"

#pragma once

struct ComputationGraph
{
    oneapi::tbb::flow::graph *graph;
    oneapi::tbb::flow::broadcast_node<Vect3> *input_node;
    oneapi::tbb::flow::queue_node<float> *output_node;

    ComputationGraph() : graph(nullptr), input_node(nullptr), output_node(nullptr) {}

    ComputationGraph(oneapi::tbb::flow::graph *g, oneapi::tbb::flow::broadcast_node<Vect3> *input,
                     oneapi::tbb::flow::queue_node<float> *output)
        : graph(g), input_node(input), output_node(output) {}
};

class Terrain
{
public:
    Terrain() = default;

    Terrain(std::unique_ptr<NoiseFunction> noiseFunction, float scale = 1.0f, float maxHeight = 100.0f)
        : noiseFunction(std::move(noiseFunction)), scale(scale), maxHeight(maxHeight) {}

    virtual float getHeight(const float first, const float second) const = 0;

    ComputationGraph getGraph();

    void addBiome(const float value, const sf::Color color);

    void clearBiomes();

    const sf::Color getColor(const float first, const float second) const;

    const sf::Color getBiomeColor(float height) const;

    ~Terrain() {};

protected:
    float scale;
    float maxHeight;
    std::vector<std::pair<float, sf::Color>> biomes;

    bool calcGraphReady = false;
    ComputationGraph calcGraph;
    std::unique_ptr<NoiseFunction> noiseFunction;
};

#endif