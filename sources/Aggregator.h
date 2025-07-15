#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include "NoiseFunction.h"

#include <memory>

#pragma once

class Aggregator : public NoiseFunction
{
public:
    Aggregator() = default;
    Aggregator(std::unique_ptr<NoiseFunction> noiseFunc) : noiseFunction(std::move(noiseFunc)) {}

    virtual float getValue(const Vect3 &value) const override = 0;

    void setNoiseFunction(std::unique_ptr<NoiseFunction> noiseFunc) { noiseFunction = std::move(noiseFunc); }

    ~Aggregator() {};

protected:
    std::unique_ptr<NoiseFunction> noiseFunction;
};

#endif