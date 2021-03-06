#pragma once

#ifndef GAMECONSTANTS_H_INCLUDED
#define GAMECONSTANTS_H_INCLUDED

#include <limits>
#include <SDL/SDL.h>

#include "Vector2D.h"

namespace RenderUtils
{
    Uint32 ColorToUint(int, int, int);
}

struct Constants {
    int screenWidth = 800;
    int screenHeight = 480;
    int screenBits = 32;

    int currentRes = 2;

    int mapHeight = 16;
    int mapWidth = 16;

    const float pi = 3.14159265358979323846f;
    const float offMapDepth =  std::numeric_limits<float>::infinity();

    float defWalkingSpeed = 0.2f;
    float rotatingSpeed = 0.1f;

    const float deg45 = pi / 4;
    const float deg90 = pi / 2;
    const float deg180 = pi;
    const float deg270 = (pi * 3) / 2;
    const float deg360 = pi * 2;

    float mouseSensitivity = 20.0f; // Works the opposite way. The bigger the value the less actual sensitivity gets.

    inline float calcFOV() {
        return pi / (6.4f * ((float)screenHeight / (float)screenWidth));
    }
    float FOV = calcFOV();

    float targetSpeed = 40.0f;

    const float blockSize = 64.0f;
    const float naiveBlockBitSize = 1.0f / blockSize;
    const float nonNaiveBBS = 1.0f / 65536.0f / 8;
    float blockBitSize = nonNaiveBBS;
    float horisontalBlockCheckStep = nonNaiveBBS * 2;
    int targetFPS = 1000;

    inline int calcHorizonCap() {
        return (screenHeight * 3);// / 3;
    }

    int horizonCap = calcHorizonCap();

    inline int calcStarsWidth() {
        return screenWidth * (int)((pi * 2) / FOV);
    }

    inline int calcStarsHeight() {
        return screenHeight + horizonCap * 2 + 1;
    }

    int starsWidth = calcStarsWidth();
    int starsHeight = calcStarsHeight();

    const Vector2D<int> resolutions[12] =
    {
            {320,  240},
            {640,  480},
            {800,  480},
            {800,  600},
            {1024, 600},
            {1024, 768},
            {1280, 720},
            {1366, 768},
            {1600, 900},
            {1920, 1080},
            {2560, 1440},
            {3840, 2160}
    };

    int resArraySize = sizeof(resolutions) / sizeof(resolutions[0]);

    const SDL_Color defSkyColor{0, 20, 100, 255};
    SDL_Color skyColor = defSkyColor;

    const SDL_Color defFloorColor{0, 90, 30, 255};
    SDL_Color floorColor = defFloorColor;

    Uint32 dayFogColor = RenderUtils::ColorToUint(127, 127, 127);
    Uint32 nightFogColor = RenderUtils::ColorToUint(0, 0, 0);

    Uint32 starColors[4] = {
                             RenderUtils::ColorToUint(255, 0, 0),
                             RenderUtils::ColorToUint(0, 0, 255),
                             RenderUtils::ColorToUint(255, 255, 0),
                             RenderUtils::ColorToUint(165, 165, 165)
                           };

    static Constants& get();
};


#endif // GAMECONSTANTS_H_INCLUDED
