#pragma once
#include <string>
template <typename T>
struct Vector2
{
    T x;
    T y;
};

template <typename T>
struct Vector3
{
    T x;
    T y;
    T z;

    std::string ToString() { return "x: " + std::to_string(x) + ", y: " + std::to_string(y) + ", z: " + std::to_string(z); }
};
