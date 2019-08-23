#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <vector>

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

inline float clampLooping(float value, float min, float max) {
    if(value > max) {
        return min + fmod(value, max);
    }
    if(value < min) {
        return max - fmod(value, max);
    }

    return value;
}

Uint32 ColorToUint(int R, int G, int B)
{
	return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

SDL_Color UintToColor(Uint32 color)
{
	SDL_Color retColor;

	retColor.unused = 255; // Alpha

	retColor.r = (color >> 16) & 0xFF; // Takes second byte
	retColor.g = (color >> 8) & 0xFF; // Takes third byte
	retColor.b = color & 0xFF; // Takes last one

	return retColor;
}

float getFractialPart(float arg)
{
    int wholePart = (int)arg;
    return arg - wholePart;
}

int getBiggerNearestInt(float arg)
{
    return (int)arg + 1;
}

inline Uint32* getTexturePixel(SDL_Surface* surf, int i, int j) {
    return (Uint32*)(surf->pixels + i * surf->pitch + j * sizeof(Uint32));
}

void loadTexture(std::vector<SDL_Surface*>& txt, const char* filename)
{
    SDL_Surface* surf = SDL_LoadBMP(filename);
    SDL_Surface* texture = SDL_DisplayFormat(surf);
    if(!surf || !texture) {
        printf("Unable to load textures. Exiting.....");
        system("PAUSE");
        exit(-1);
    }
    txt.push_back(texture);
}

SDL_Color transformColorByLightMap(SDL_Color color, const SDL_Color lightmapColor) {
        color.r = clamp(color.r + lightmapColor.r - 192, 0, 255);
        color.g = clamp(color.g + lightmapColor.g - 192, 0, 255);
        color.b = clamp(color.b + lightmapColor.b - 192, 0, 255);

        return color;
}

#include "GameConstants.h"
#include "GameData.h"

void loadTextures() {
    loadTexture(textures, "wall2.bmp");
    loadTexture(textures, "wall.bmp");
}

void loadLightmaps() {
    loadTexture(lightmaps, "wall2bumpmap.bmp");
}

void fillUpTheMapToBeBox(MapBlock** aMap)
{
    for(int i = 0; i < mapWidth; ++i)
    {
        aMap[0][i].setDefault();
    }

    for(int i = 1; i < mapHeight-1; ++i)
    {
        for(int j = 0; j < mapWidth; ++j)
        {
            if(j == 0 || j == mapWidth-1) aMap[i][j].setDefault();
            else aMap[i][j].setEmpty();
        }
    }

    for(int i = 0; i < mapWidth; ++i)
    {
        aMap[mapHeight-1][i].setDefault();
    }
}

void fillUpTheStars() {
    srand(256);
    for(int i = 0; i < starsHeight; ++i) {
        for(int j = 0; j < starsWidth; ++j) {
            if(!(rand() % screenWidth / 2)) {
                stars[i][j] = true;
            }
        }
    }
}

#endif // UTILITY_H_INCLUDED
