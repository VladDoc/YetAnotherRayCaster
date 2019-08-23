#ifndef GAMECONSTANTS_H_INCLUDED
#define GAMECONSTANTS_H_INCLUDED

#include <limits>

#include "Utility.h"

//#define TEXTURE_GRADIENT 1

const int screenWidth = 800;
const int screenHeight = 480;
const int screenBits = 32;

const int mapHeight = 16;
const int mapWidth = 16;

const float pi = 3.14159f;
constexpr const float depth =  std::numeric_limits<float>::max();

const float defWalkingSpeed = 0.2f;
const float rotatingSpeed = 0.1f;

const float mouseSensitivity = 20.0f; // Works the opposite way. The bigger the value the less actual sensitivity gets.

constexpr const float FOV = pi / (6.4f * ((float)screenHeight / (float)screenWidth));

const float targetSpeed = 40.0f;

const float blockSize = 64.0f;
const float blockBitSize = 1.0f / blockSize;
const int targetFPS = 60;

const int horizonCap = (screenHeight * 2) / 3;

const int starsWidth = screenWidth * (int)((pi * 2) / FOV);
const int starsHeight = screenHeight + horizonCap * 2 + 1;

Uint32 defWallColor = ColorToUint(45, 20, 0);

#endif // GAMECONSTANTS_H_INCLUDED
