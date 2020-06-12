#pragma once

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <iostream>
#include <vector>
#include <random>
#include <ostream>

#include <SDL/SDL.h>

#include "Vector2D.h"
#include "GameConstants.h"

struct MapBlock;
struct Sprite;
struct GameData;

namespace util {


    void loadTexture(std::vector<SDL_Surface*>& txt, const char* filename);

    float intersectDist(const Vector2D<float>& test,
                        float sine, float cosine,
                        int quarter, float margin = 1e-16);

    void loadTextures(std::vector<SDL_Surface*>& txt);


    void loadLightmaps(std::vector<SDL_Surface*>& lmp);

    void fillUpTheMapToBeBox(MapBlock** aMap);

    void loadSkyTextures(std::vector<SDL_Surface*>& skyTxt);

    void setLightMapsTo0(GameData& d);

    void loadSprites(std::vector<Sprite>& sprts);

    template<typename T>
    void print2dVector(std::vector<std::vector<T>>& vec,
                             std::ostream& where)
    {
        if(vec.empty() || vec[0].empty()) return;
        for(size_t i = 0; i < vec.size(); ++i) {
            for(size_t j = 0; j < vec[0].size(); ++j) {
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

    inline float degreesToRad(float degrees) {
        return degrees * (Constants::get().pi / 180);
    }

    inline float angleBetweenPoints(Vector2D<float> a, Vector2D<float> b)
    {
        float deltaY = std::abs(b.y - a.y);
        float deltaX = std::abs(b.x - a.x);
        return std::atan2(deltaY, deltaX);
    }
}
#endif // UTILITY_H_INCLUDED
