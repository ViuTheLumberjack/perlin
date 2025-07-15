#include "Vect3.h"

Vect3 &Vect3::operator=(const Vect3 &v)
{
    if (this != &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }
    return *this;
}

Vect3 &Vect3::operator*=(const Vect3 &v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

Vect3 &Vect3::operator+=(const Vect3 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vect3 &Vect3::operator-=(const Vect3 &v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vect3 &Vect3::operator*=(const float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

Vect3 &Vect3::operator/=(const float s)
{
    if (s != 0)
    {
        x /= s;
        y /= s;
        z /= s;
    }
    else
    {
        x = INFINITY;
        y = INFINITY;
        z = INFINITY;
    }
    return *this;
}

Vect3 Vect3::operator/(const Vect3 &v) const
{
    float x, y, z;
    if (v.x != 0)
    {
        x = this->x / v.x;
    }
    else
    {
        x = INFINITY;
    }
    if (v.y != 0)
    {
        y = this->y / v.y;
    }
    else
    {
        y = INFINITY;
    }
    if (v.z != 0)
    {
        z = this->z / v.z;
    }
    else
    {
        z = INFINITY;
    }

    return Vect3(x, y, z);
}

Vect3 Vect3::operator/(const float s) const
{
    if (s != 0)
        return Vect3(x / s, y / s, z / s);
    else
        return Vect3(INFINITY, INFINITY, INFINITY);
}