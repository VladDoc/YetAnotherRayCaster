#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <iostream>
#include <vector>

#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "Vector2D.h"
#include "GameConstants.h"

namespace util {


    void loadTexture(std::vector<SDL_Surface*>& txt, const char* filename);

    float intersectDist(const Vector2D<float>& test,
                        float sine, float cosine,
                        int quarter, float margin = 1e-16);


    template<typename T>
    void print2dVector(std::vector<std::vector<T>>& vec,
                       std::ostream& where)
    {
        if(vec.empty() || vec[0].empty()) return;
        for(size_t i = 0; i < vec.size(); ++i) {
            for(size_t j = 0; j < vec[i].size(); ++j) {
                where << vec[i][j];
            }
            where << std::endl;
        }
    }

    template <typename T>
    inline T clamp(T value, T min, T max) {
        if(value > max) {
                return max;
        }
        if(value < min) {
                return min;
        }

        return value;
    }

    // Works only for positive min and max
    inline float clampLooping(float value, float min, float max) {
        int howManyTimesMax = value / max;
        float retval = value > 0 ? (value - (howManyTimesMax * max))
                         : max + (value - (howManyTimesMax * max));
        return retval;

    }

    template<typename T>
    inline bool withinRange(T arg, T min, T max)
    {
        return arg >= min && arg < max;
    }

    inline float getFractialPart(float arg)
    {
        int wholePart = (int)arg;
        return arg - wholePart;
    }

    inline float fractionBetweenNumbers(float arg, float min, float max)
    {
        return arg - min;
    }

    inline float invertFraction(float arg)
    {
        int wholePart = (int)arg;
        float fraction = arg - wholePart;
        fraction = 1.0f - fraction;
        return (float)wholePart + fraction;
    }

    inline float directionToAngle(int x, int y)
    {
        Constants& c = Constants::get();
        if(x == 0 && y == 0) return -std::numeric_limits<float>::infinity();

        if ( x >=  1 && y ==  0 ) return 0.0f;
        if ( x >=  1 && y >=  1 ) return c.deg45;
        if ( x ==  0 && y >=  1 ) return c.deg90;
        if ( x <= -1 && y >=  1 ) return c.deg90 + c.deg45;
        if ( x <= -1 && y ==  0 ) return c.deg180;
        if ( x <= -1 && y <= -1 ) return c.deg180 + c.deg45;
        if ( x ==  0 && y <= -1 ) return c.deg270;
        if ( x >=  1 && y <= -1 ) return c.deg270 + c.deg45;
    }

    template<typename T>
    constexpr void check()
    {
        static_assert(std::is_arithmetic<T>::value,
                      "Works only with arithmetic types.");
    }

    inline float angleBetweenPoints(const Vector2D<float>& a,
                                    const Vector2D<float>& b)
    {
        float deltaY = std::abs(b.y - a.y);
        float deltaX = std::abs(b.x - a.x);
        return std::atan2(deltaY, deltaX);
    }

    template <typename T>
    inline T uintPow(T value, unsigned power)
    {
        check<T>();

        if(power == 0) return T{1};

        T temp = value;
        while(--power) {
            temp *= value;
        }
        return temp;
    }

    constexpr float radToDeg(float rad)
    {
        return rad * (180.0f / M_PI);
    }

    constexpr float degToRad(float deg)
    {
        return deg * (M_PI / 180);
    }

    template <typename T>
    inline float vectorAngle(T x, T y)
    {
        float val = std::atan2(x, -y);
        return val < 0 ? M_PI * 2 + val : val;
    }

    template <typename T>
    inline T interpolateValue(time_t frametime, T value)
    {
        check<T>();
        return value * (static_cast<T>(frametime) / Constants::get().targetSpeed);
    }

    inline Vector2D<float> lengthWithAngleToVector(float angle, float length)
    {

        return {std::sin(angle) * length,
                std::cos(angle) * length};
    }

    inline size_t twoIndeciesIntoOne(size_t i, size_t j, size_t width)
    {
        return i * width + j;
    }

    inline void oneIndexIntoTwo(size_t index, size_t width, size_t& i, size_t& j)
    {
        i = index / width;
        j = index - (index / width) * width;
    }

    template <typename T>
    inline bool withinArea(Vector2D<T> src, Vector2D<T> dest, float radius)
    {
        check<T>();

        T dx = src.x - dest.x;
        T dy = src.x - dest.x;

        float distance = std::hypot(dx, dy);

        if(distance < radius) {
            return true;
        }
    }

}
#endif // UTILITY_H_INCLUDED
