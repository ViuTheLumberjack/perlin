#ifndef ADD_H
#define ADD_H

#include "Aggregator.h"
#include <memory>

#pragma once

class Add : public Aggregator
{
public:
    Add(std::unique_ptr<NoiseFunction> noiseFunc1, std::unique_ptr<NoiseFunction> noiseFunc2, float scale1 = 1, float scale2 = 1)
        : Aggregator(std::move(noiseFunc1)), otherNoiseFunction(std::move(noiseFunc2)), scale1(scale1), scale2(scale2) {}

    float getValue(const Vect3 &value) const override;

    oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node);

    ~Add() {};

private:
    std::unique_ptr<NoiseFunction> otherNoiseFunction;
    float scale1;
    float scale2;
};

#endif