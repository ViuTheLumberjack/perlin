#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "Terrain.h"

#pragma once

class Sphere : public Terrain
{
public:
    Sphere(std::unique_ptr<NoiseFunction> noiseFunction, float radius, float scale = 1, float maxHeight = 100.0f)
        : Terrain(std::move(noiseFunction), scale, maxHeight), radius(radius) {};

    float getHeight(const float latitude, const float longitude) const override
    {
        // Convert latitude and longitude to Cartesian coordinates
        float x = radius * cos(latitude) * cos(longitude);
        float y = radius * cos(latitude) * sin(longitude);
        float z = radius * sin(latitude);

        Vect3 position(x, y, z);
        return this->noiseFunction->getValue(position * this->scale) * this->maxHeight;
    }

    ~Sphere() {};

private:
    float radius;
};

#endif