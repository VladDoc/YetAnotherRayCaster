#pragma once

#ifndef GAMEDATA_H_INCLUDED
#define GAMEDATA_H_INCLUDED

#include <vector>

#include "GameConstants.h"
#include "MapBlock.h"
#include "Sprite.h"

float walkingSpeed = 0.2f;

int horizonLine = 0; // 0 is default it means that horizon won't be changed

bool done = false;

std::vector<SDL_Surface*> textures;
std::vector<SDL_Surface*> lightmaps;

// m_ stands for mirrored
std::vector<SDL_Surface*> m_textures;
std::vector<SDL_Surface*> m_lightmaps;

std::vector<SDL_Surface*> sky_textures;

std::vector<Sprite> hudSprites;

MapBlock map[mapHeight][mapWidth] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {2,1}},
    {3, 0, 0, 0, 0, 0, 0, 0, {0, 0, 50}, {0, 0, 50}, {20, 0, 50}, {20, 0, 50}, {20, 0, 50}, {20, 0, 50}, 0, 1},
    {3, 0, 0, 4, 0, 0, 0, 0, {50, 0, 0}, {50, 0, 0}, {50, 0, 0}, {50, 0, 0}, {50, 0, 0}, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {6, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};


Uint8* stars;

float* distances;
float* rays;
Vector2D<float>* rayPositions;

FILE* logFile;

thread_local std::mt19937 random;
thread_local std::uniform_int_distribution<int> dist{0, 255};

void fillUpTheStars() {
    std::mt19937 r;
    r.seed(1);
    stars = (Uint8*)realloc(stars, starsWidth * starsHeight * sizeof(*stars));
    memset(stars, 0, starsWidth * starsHeight);

    for(int i = 0; i < starsHeight; ++i) {
        for(int j = 0; j < starsWidth; ++j) {
            if(!(r() % screenWidth / 2)) {
                stars[i * starsWidth + j] = r() & 0xFF;
            }
        }
    }
}

enum class SideOfAWall
{
    NORTH,
    EAST,
    SOUTH,
    WEST
};

void allocateScreenSizeSensitiveData()
{
    distances = (float*) realloc(distances, sizeof(float) * screenWidth);
    rayPositions = (Vector2D<float>*) realloc(rayPositions, sizeof(Vector2D<float>) * screenWidth);
    rays =  (float*) realloc(rays, sizeof(float) * screenWidth);
    fillUpTheStars();
}

void freeScreenSizeSensitiveData()
{
    free(distances);
    free(rayPositions);
    free(rays);
    free(stars);
}

#endif // GAMEDATA_H_INCLUDED
