#pragma once

#ifndef VECTOR2D_H_INCLUDED
#define VECTOR2D_H_INCLUDED

#include <utility>
#include <tuple>

#include "player.h"

template <typename T>
struct Vector2D
{
    T x{};
    T y{};
    Vector2D() = default;
    Vector2D(T xx, T yy) : x(xx), y(yy) {}
    Vector2D(const Vector2D<T>& vec) : x(vec.x), y(vec.y) {}

    Vector2D(const Player& p)
    {
        this.x = (T)p.x;
        this.y = (T)p.y;
    }

    Vector2D& operator=(const Player& p)
    {
        this.x = (T)p.x;
        this.y = (T)p.y;

        return *this;
    }

    template<typename V>
    Vector2D<T> operator+(const Vector2D<V> &another)
    {
        this->x += another.x;
        this->y += another.y;

        return *this;
    }

    template<typename V>
    Vector2D<T>& operator+=(const Vector2D<V> &another)
    {
        this->x += another.x;
        this->y += another.y;

        return *this;
    }

    template<typename V>
    Vector2D<T> operator-(const Vector2D<V> &another)
    {
        this->x -= another.x;
        this->y -= another.y;

        return *this;
    }

    template<typename V>
    Vector2D<T>& operator-=(const Vector2D<V> &another)
    {
        this->x -= another.x;
        this->y -= another.y;

        return *this;
    }

    template <typename V>
    operator Vector2D<V>() const
    {
        return Vector2D<V>{static_cast<V>(x),
                           static_cast<V>(y)};
    }

    template<typename V>
    bool operator<(const Vector2D<V>& b) const
    {
        return std::tie(x, y) < std::tie(b.x, b.y);
    }

    template<typename V>
    bool operator==(const Vector2D<V>& another)
    {
        return this->x == another.x && this->y == another.y;
    }

    template<typename V>
    bool operator!=(const Vector2D<V>& another)
    {
        return !(*this == another);
    }
};

#endif // VECTOR2D_H_INCLUDED
