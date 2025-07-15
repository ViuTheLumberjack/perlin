#ifndef TERRACE_H
#define TERRACE_H

#include "Aggregator.h"

#include <memory>
#include <functional>

#pragma once

float ROUND_TERRACE(float value, float step)
{
    return std::floor(value * step) / step;
}

float SMOOTH_TERRACE(float value, float step)
{
    return std::round(value / step) * step;
}

class Terrace : public Aggregator
{
public:
    Terrace(float terraces, std::function<float(float, float)> terraceOperation, std::unique_ptr<NoiseFunction> noiseFunction) : Aggregator(std::move(noiseFunction)), terraces(terraces) {};

    float getValue(const Vect3 &value) const override;

    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node) override;

    ~Terrace() {};

private:
    float terraces;
    std::function<float(float, float)> terraceOperation = ROUND_TERRACE; // Default operation to round the value
};

#endif