#ifndef GAMEDATA_H_INCLUDED
#define GAMEDATA_H_INCLUDED

#include "GameConstants.h"
#include "MapBlock.h"

float walkingSpeed = 0.2f;

int horizonLine = 0; // 0 is default it means that horizon won't be changed

bool done = false;

std::vector<SDL_Surface*> textures;
std::vector<SDL_Surface*> lightmaps;

SDL_Color defSkyColor;
SDL_Color skyColor;

MapBlock map[mapHeight][mapWidth] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {0, 0, 0, 0, 0, 0, 0, 0, MapBlock(0, 0, 50), MapBlock(0, 0, 50), MapBlock(20, 0, 50), MapBlock(20, 0, 50), MapBlock(20, 0, 50), MapBlock(20, 0, 50), 0, 1},
    {3, 0, 3, 0, 0, 0, 0, 0, MapBlock(50, 0, 0), MapBlock(50, 0, 0), MapBlock(50, 0, 0), MapBlock(50, 0, 0), MapBlock(50, 0, 0), 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

bool stars[starsHeight][starsWidth];

#endif // GAMEDATA_H_INCLUDED
