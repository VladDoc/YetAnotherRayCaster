#pragma once

#ifndef VECTOR2D_H_INCLUDED
#define VECTOR2D_H_INCLUDED

#include "player.h"

template <typename T>
struct Vector2D
{
    T x{};
    T y{};
    Vector2D() = default;
    Vector2D(T xx, T yy) : x(xx), y(yy) {}
    Vector2D(const Vector2D<T>& vec) : x(vec.x), y(vec.y) {}

    Vector2D(const Player& p) {
        this.x = (T)p.x;
        this.y = (T)p.y;
    }

    Vector2D& operator=(const Player& p) {
        this.x = (T)p.x;
        this.y = (T)p.y;

        return *this;
    }
};

#endif // VECTOR2D_H_INCLUDED
