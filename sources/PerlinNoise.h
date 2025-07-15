#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "Vect3.h"
#include "NoiseFunction.h"

#pragma once

class PerlinNoise : public NoiseFunction
{
public:
    PerlinNoise(int seed, float persistence = 0.5f, int octaves = 1);

    float getValue(const Vect3 &value) const;

    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node) override;

private:
    int seed;
    float persistence;
    int octaves;

    int permutationTable[255 * 2];

    Vect3 gradients[12] = {
        Vect3(1, 1, 0), Vect3(-1, 1, 0), Vect3(1, -1, 0), Vect3(-1, -1, 0),
        Vect3(1, 0, 1), Vect3(-1, 0, 1), Vect3(1, 0, -1), Vect3(-1, 0, -1),
        Vect3(0, 1, 1), Vect3(0, -1, 1), Vect3(0, 1, -1), Vect3(0, -1, -1)};

    float fade(float t) const;
    float lerp(float a, float b, float t) const;
    float grad(int hash, float x, float y, float z) const;
    float noise(Vect3 &value) const;
};

#endif