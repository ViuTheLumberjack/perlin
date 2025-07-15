#ifndef VECT2_H
#define VECT2_H

#include <cmath>

#pragma once

class Vect3
{
public:
    Vect3(const float x = 0.0, const float y = 0.0, const float z = 0.0)
        : x(x), y(y), z(z) {};

    Vect3(const Vect3 &v)
        : x(v.x), y(v.y), z(v.z) {};

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

    Vect3 &operator=(const Vect3 &v);
    Vect3 &operator*=(const Vect3 &v);
    Vect3 &operator+=(const Vect3 &v);
    Vect3 &operator-=(const Vect3 &v);
    Vect3 &operator*=(const float s);
    Vect3 &operator/=(const float s);
    Vect3 operator+(const Vect3 &v) const { return Vect3(x + v.x, y + v.y, z + v.z); }
    Vect3 operator+(const float s) const { return Vect3(x + s, y + s, z + s); }
    Vect3 operator-(const Vect3 &v) const { return Vect3(x - v.x, y - v.y, z - v.z); }
    Vect3 operator-(const float s) const { return Vect3(x - s, y - s, z - s); }
    Vect3 operator*(const Vect3 &v) const { return Vect3(x * v.x, y * v.y, z * v.z); }
    Vect3 operator*(const float s) const { return Vect3(x * s, y * s, z * s); }
    Vect3 operator/(const Vect3 &v) const;
    Vect3 operator/(const float s) const;

    Vect3 operator%(const float v) const { return Vect3(fmod(x, v), fmod(y, v), fmod(z, v)); }

    float dot(Vect3 &v) const { return x * v.x + y * v.y + z * v.z; }

    ~Vect3() {};

private:
    float x, y, z;
};

#endif