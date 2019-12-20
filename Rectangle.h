#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

template <typename T = int>
struct Rectangle
{
    T x{0};
    T y{0};
    T w{0};
    T h{0};

    Rectangle(T xx, T yy, T ww, T hh) :
        x(xx), y(yy), w(ww), h(hh) { }
    Rectangle(const Rectangle& other) = default;
    Rectangle(Rectangle&& other) = default;
};

#endif // RECTANGLE_H_INCLUDED
