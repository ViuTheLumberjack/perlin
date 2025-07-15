#include "Terrain.h"

ComputationGraph Terrain::getGraph()
{
    auto graph = new oneapi::tbb::flow::graph();

    auto input_node = new oneapi::tbb::flow::broadcast_node<Vect3>(*graph);
    auto input_scale = new oneapi::tbb::flow::function_node<Vect3, Vect3>(
        *graph, oneapi::tbb::flow::unlimited,
        [this](const Vect3 &input)
        {
            return input * scale;
        });

    auto parent = this->noiseFunction->getComputationGraph(*graph, *input_scale);

    auto output = new oneapi::tbb::flow::function_node<float, float>(
        *graph, oneapi::tbb::flow::unlimited,
        [this](const float &input)
        {
            return (input)*maxHeight;
        });

    auto output_queue = new oneapi::tbb::flow::queue_node<float>(*graph);

    oneapi::tbb::flow::make_edge(*input_node, *input_scale);
    oneapi::tbb::flow::make_edge(*parent, *output);
    oneapi::tbb::flow::make_edge(*output, *output_queue);

    this->calcGraph = ComputationGraph(graph, input_node, output_queue);

    return this->calcGraph;
}

void Terrain::addBiome(const float value, const sf::Color color)
{
    biomes.emplace_back(value, color);
}

void Terrain::clearBiomes()
{
    biomes.clear();
}

const sf::Color Terrain::getColor(const float first, const float second) const
{
    float height = getHeight(first, second);

    return getBiomeColor(height);
}

const sf::Color Terrain::getBiomeColor(float height) const
{
    if (biomes.empty())
    {
        auto noiseValue = height / maxHeight;
        // Calculate the grayscale equivalent if no biomes are defined
        return sf::Color(static_cast<int>(noiseValue * 255),
                         static_cast<int>(noiseValue * 255),
                         static_cast<int>(noiseValue * 255));
    }

    std::pair<float, sf::Color> bestMatch({-1, sf::Color::White});
    for (const auto &biome : biomes)
    {
        if (height < biome.first && (biome.first < bestMatch.first || bestMatch.first < 0))
            bestMatch = biome;
    }

    return sf::Color(bestMatch.second);
}