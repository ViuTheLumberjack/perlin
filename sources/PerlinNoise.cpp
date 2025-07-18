#include "PerlinNoise.h"

#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>

int inc(int num)
{
    num++;

    return num;
}

PerlinNoise::PerlinNoise(int seed, float persistence, int octaves) : seed(seed), persistence(persistence), octaves(octaves)
{
    // Initialize the permutation table with values from 0 to 255
    for (int i = 0; i < 256; ++i)
    {
        permutationTable[i] = i;
    }

    std::mt19937 generator(seed);
    std::shuffle(std::begin(permutationTable), &permutationTable[256], generator);

    for (int i = 0; i < 256; ++i)
    {
        permutationTable[i + 256] = permutationTable[i];
    }
}

float PerlinNoise::getValue(const Vect3 &value) const
{
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double sum = 0;
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> unif(0, 100);

    for (int i = 0; i < octaves; i++)
    {
        auto scaled_value = value * frequency;
        auto randoms = Vect3(unif(generator),
                             unif(generator),
                             unif(generator));
        scaled_value = scaled_value + randoms; // Ensure the value is within the range of the permutation table

        total += noise(scaled_value) * amplitude;

        sum += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total / sum;
}

float PerlinNoise::noise(Vect3 &value) const
{
    int xi = (int)value.getX() & 255;
    int yi = (int)value.getY() & 255;
    int zi = (int)value.getZ() & 255;
    double xf = value.getX() - (int)value.getX();
    double yf = value.getY() - (int)value.getY();
    double zf = value.getZ() - (int)value.getZ();
    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    // Unit cube random grads
    int aaa, aba, aab, abb, baa, bba, bab, bbb;

    aaa = permutationTable[permutationTable[permutationTable[xi] + yi] + zi];
    aba = permutationTable[permutationTable[permutationTable[xi] + inc(yi)] + zi];
    aab = permutationTable[permutationTable[permutationTable[xi] + yi] + inc(zi)];
    abb = permutationTable[permutationTable[permutationTable[xi] + inc(yi)] + inc(zi)];
    baa = permutationTable[permutationTable[permutationTable[inc(xi)] + yi] + zi];
    bba = permutationTable[permutationTable[permutationTable[inc(xi)] + inc(yi)] + zi];
    bab = permutationTable[permutationTable[permutationTable[inc(xi)] + yi] + inc(zi)];
    bbb = permutationTable[permutationTable[permutationTable[inc(xi)] + inc(yi)] + inc(zi)];

    float x1, x2, y1, y2;

    x1 = lerp(grad(aaa, xf, yf, zf),
              grad(baa, xf - 1, yf, zf),
              u);
    x2 = lerp(grad(aba, xf, yf - 1, zf),
              grad(bba, xf - 1, yf - 1, zf),
              u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, xf, yf, zf - 1),
              grad(bab, xf - 1, yf, zf - 1),
              u);
    x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
              grad(bbb, xf - 1, yf - 1, zf - 1),
              u);
    y2 = lerp(x1, x2, v);

    return (lerp(y1, y2, w) + 1) / 2;
}

float PerlinNoise::fade(float t) const
{
    return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3;
}

float PerlinNoise::lerp(float a, float b, float t) const
{
    return a + (b - a) * t;
}

float PerlinNoise::grad(int hash, float x, float y, float z) const
{
    switch (hash & 0xF)
    {
    case 0x0:
        return x + y;
    case 0x1:
        return -x + y;
    case 0x2:
        return x - y;
    case 0x3:
        return -x - y;
    case 0x4:
        return x + z;
    case 0x5:
        return -x + z;
    case 0x6:
        return x - z;
    case 0x7:
        return -x - z;
    case 0x8:
        return y + z;
    case 0x9:
        return -y + z;
    case 0xA:
        return y - z;
    case 0xB:
        return -y - z;
    case 0xC:
        return y + x;
    case 0xD:
        return -y + z;
    case 0xE:
        return y - x;
    case 0xF:
        return -y - z;
    default:
        return 0; // never happens
    }
}

oneapi::tbb::flow::sender<float> *PerlinNoise::getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node)
{
    auto node = new oneapi::tbb::flow::function_node<Vect3, float>(graph, oneapi::tbb::flow::unlimited,
                                                                   [*this](const Vect3 &input)
                                                                   { return getValue(input); });

    oneapi::tbb::flow::make_edge(input_node, *node);

    return node;
}
