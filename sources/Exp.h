#ifndef EXP_H
#define EXP_H

#include "Aggregator.h"

#include <memory>
#include <cmath>

#pragma once

class Exp : public Aggregator
{
public:
    Exp(float exponent, float fudgeFactor, std::unique_ptr<NoiseFunction> noiseFunction);

    float getValue(const Vect3 &value) const override;

    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node) override;

    ~Exp() {};

private:
    float exponent;
    float fudgeFactor;
};

#endif