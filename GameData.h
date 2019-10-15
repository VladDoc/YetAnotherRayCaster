#pragma once

#ifndef GAMEDATA_H_INCLUDED
#define GAMEDATA_H_INCLUDED

#include <vector>

#include "GameConstants.h"
#include "MapBlock.h"

float walkingSpeed = 0.2f;

int horizonLine = 0; // 0 is default it means that horizon won't be changed

bool done = false;

std::vector<SDL_Surface*> textures;
std::vector<SDL_Surface*> lightmaps;

// m_ stands for mirrored
std::vector<SDL_Surface*> m_textures;
std::vector<SDL_Surface*> m_lightmaps;

std::vector<SDL_Surface*> sky_textures;

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


bool* stars;

#endif // GAMEDATA_H_INCLUDED
