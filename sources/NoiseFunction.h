#ifndef NOISEFUNCTION_H
#define NOISEFUNCTION_H

#include <oneapi/tbb.h>

#include "Vect3.h"

#pragma once

class NoiseFunction
{
public:
    virtual float getValue(const Vect3 &value) const = 0;
    virtual oneapi::tbb::flow::sender<float> *getComputationGraph(oneapi::tbb::flow::graph &graph, oneapi::tbb::flow::sender<Vect3> &input_node) = 0;
    virtual ~NoiseFunction() {};

private:
};

#endif