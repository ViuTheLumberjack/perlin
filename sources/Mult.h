#ifndef MULT_H
#define MULT_H

#include "Aggregator.h"

#include <oneapi/tbb.h>

#pragma once

class Mult : public Aggregator
{
public:
    Mult(std::unique_ptr<NoiseFunction> noiseFunc1, std::unique_ptr<NoiseFunction> noiseFunc2)
        : Aggregator(std::move(noiseFunc1)), otherNoiseFunction(std::move(noiseFunc2)) {};

    float getValue(const Vect3 &value) const override
    {
        return this->noiseFunction->getValue(value) * this->otherNoiseFunction->getValue(value);
    }

    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node)
    {
        auto parent1 = this->noiseFunction->getComputationGraph(graph, input_node);
        auto parent2 = this->otherNoiseFunction->getComputationGraph(graph, input_node);

        auto join = new oneapi::tbb::flow::join_node<std::tuple<float, float>>(graph);

        oneapi::tbb::flow::make_edge(*parent1, get<0>(join->input_ports()));
        oneapi::tbb::flow::make_edge(*parent2, get<1>(join->input_ports()));

        auto node = new oneapi::tbb::flow::function_node<std::tuple<float, float>, float>(
            graph, oneapi::tbb::flow::unlimited,
            [=](const std::tuple<float, float> &input)
            {
                const auto [first, second] = input;
                return (first * second);
            });

        oneapi::tbb::flow::make_edge(*join, *node);

        return node;
    }

    ~Mult() {};

private:
    std::unique_ptr<NoiseFunction> otherNoiseFunction;
};

#endif