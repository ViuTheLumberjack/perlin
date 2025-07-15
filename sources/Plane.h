#ifndef PLANE_H
#define PLANE_H

#include <memory>

#include "Terrain.h"

#pragma once

class Plane : public Terrain
{
public:
    Plane(std::unique_ptr<NoiseFunction> noiseFunction, float scale, float maxHeight)
        : Terrain(std::move(noiseFunction), scale, maxHeight) {};

    float getHeight(const float x, const float y) const override
    {
        auto value = Vect3(x, y, 42.0f); // Set z to 0 for a flat plane
        return this->noiseFunction->getValue(value * this->scale) * this->maxHeight;
    }

    ~Plane() {};
};

#endif