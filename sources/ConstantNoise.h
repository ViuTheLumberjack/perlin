#ifndef CONSTANTNOISE_H
#define CONSTANTNOISE_H

#include "NoiseFunction.h"

#pragma once

class ConstantNoise : public NoiseFunction
{
public:
    ConstantNoise(float value) : value(value) {};
    float getValue(const Vect3 &value) const { return this->value; }
    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node) override
    {
        auto node = new oneapi::tbb::flow::function_node<Vect3, float>(graph, oneapi::tbb::flow::unlimited,
                                                                       [this](const Vect3 &input)
                                                                       { return getValue(input); });

        oneapi::tbb::flow::make_edge(input_node, *node);

        return node;
    }

    ~ConstantNoise() {};

private:
    float value;
};

#endif