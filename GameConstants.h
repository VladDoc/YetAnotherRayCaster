#pragma once

#ifndef GAMECONSTANTS_H_INCLUDED
#define GAMECONSTANTS_H_INCLUDED

#include <limits>

#include "Utility.h"
#include "Vector2D.h"

bool textureGradient = false;

int screenWidth = 800;
int screenHeight = 480;
const int screenBits = 32;

int currentRes = 2;

const int mapHeight = 16;
const int mapWidth = 16;

const float pi = 3.14159265358979323846f;
const float depth =  std::numeric_limits<float>::infinity();

const float defWalkingSpeed = 0.2f;
const float rotatingSpeed = 0.1f;

const float deg90 = pi / 2;
const float deg180 = pi;
const float deg270 = (pi * 3) / 2;
const float deg360 = pi * 2;

const float mouseSensitivity = 20.0f; // Works the opposite way. The bigger the value the less actual sensitivity gets.

float calcFOV() {
    return pi / (6.0f * ((float)screenHeight / (float)screenWidth));
}

float FOV = calcFOV();

const float targetSpeed = 40.0f;

const float blockSize = 64.0f;
const float naiveBlockBitSize = 1.0f / blockSize;
const float nonNaiveBBS = 1.0f / 65536.0f / 8;
float blockBitSize = nonNaiveBBS;
float horisontalBlockCheckStep = nonNaiveBBS * 2;
int targetFPS = 1000;

int calcHorizonCap() {
    return (screenHeight * 2) / 3;
}

int horizonCap = calcHorizonCap();

int calcStarsWidth() {
    return screenWidth * (int)((pi * 2) / FOV);
}

int calcStarsHeight() {
    return screenHeight + horizonCap * 2 + 1;
}

int starsWidth = calcStarsWidth();
int starsHeight = calcStarsHeight();

Uint32 defWallColor = ColorToUint(45, 20, 0);

const Vector2D<int> resolutions[] =
    {
        {320,  240},
        {640,  480},
        {800,  480},
        {800,  600},
        {1024, 768},
        {1280, 720},
        {1366, 768},
        {1600, 900},
        {1920, 1080}
    };

const int resArraySize = 9;
#endif // GAMECONSTANTS_H_INCLUDED
