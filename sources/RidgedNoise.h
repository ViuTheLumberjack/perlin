#ifndef RIDGEDNOISE_H
#define RIDGEDNOISE_H

#include "Aggregator.h"

#include <memory>

#pragma once

class RidgedNoise : public Aggregator
{
public:
    RidgedNoise(std::unique_ptr<NoiseFunction> noise, int octaves = 1) : Aggregator(std::move(noise)), octaves(octaves) {};

    float getValue(const Vect3 &value) const override;

    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node) override;

    ~RidgedNoise() {};

private:
    int octaves;

    float noise(const Vect3 &value) const;
};

#endif