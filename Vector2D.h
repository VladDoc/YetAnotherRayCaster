#ifndef VECTOR2D_H_INCLUDED
#define VECTOR2D_H_INCLUDED

template <typename T>
struct Vector2D
{
    T x{};
    T y{};
    Vector2D() = default;
    Vector2D(T xx, T yy) : x(xx), y(yy) {}
    Vector2D(const Vector2D<T>& vec) {
        x = vec.x;
        y = vec.y;
    }
};

#endif // VECTOR2D_H_INCLUDED
