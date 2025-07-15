#include "RidgedNoise.h"

float RidgedNoise::getValue(const Vect3 &value) const
{
    float amplitude = 1.0f;
    float frequency = 1.0f;

    float amplitudeSum = 0.0f;
    float valueSum = 0.0f;
    for (int i = 0; i < octaves; ++i)
    {
        valueSum += noise(value * frequency) * amplitude * (valueSum == 0.0f ? 1.0f : valueSum);

        amplitudeSum += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return valueSum / amplitudeSum;
}

float RidgedNoise::noise(const Vect3 &value) const
{
    float noiseValue = noiseFunction->getValue(value);

    return 2.0f * (0.5f - fabs(0.5 - noiseValue));
}

oneapi::tbb::flow::sender<float> *RidgedNoise::getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node)
{
    auto parent = this->noiseFunction->getComputationGraph(graph, input_node);

    auto node = new oneapi::tbb::flow::function_node<float, float>(graph, oneapi::tbb::flow::unlimited, [=](const float &value)
                                                                   { return 2.0f * (0.5f - fabs(0.5 - value)); });

    oneapi::tbb::flow::make_edge(*parent, *node);

    return node;
}
