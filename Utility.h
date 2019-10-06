#pragma once

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <vector>
#include <random>

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
    return value > 0 ? value - (howManyTimesMax * max)
                     : max - std::abs(value) - (howManyTimesMax * max);

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


float fractionBetweenNumbers(float arg, float min, float max)
{
    return arg - min;
}

float invertFraction(float arg)
{
    int wholePart = (int)arg;
    float fraction = arg - wholePart;
    fraction = 1.0f - fraction;
    return (float)wholePart + fraction;
}

inline Uint32* getTexturePixel(SDL_Surface* surf, int i, int j) {
    return (Uint32*)(surf->pixels + i * surf->pitch + j * sizeof(Uint32));
}

inline Uint32* getTransposedTexturePixel(SDL_Surface* surf, int i, int j) {
    return (Uint32*)(surf->pixels + j * surf->pitch + i * sizeof(Uint32));
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
#include "Vector2D.h"

float degreesToRad(float degrees) {
    return degrees * (pi / 180);
}

float getDistanceToTheNearestIntersection(const Vector2D<float>& test, float ray)
{

    /*
     *
     *          Actually gives speed up, but is held on duct tape.
     *          Spend a whole week getting it to work.
     *          Almost lost my sanity in the process.
     *          Performance changes depending on
     *          which quadrant of circle player currently is.
     *
     */
    if(!naiveApproach) {

        float bufferRay = clampLooping(ray, 0.0f, pi * 2);

        Vector2D<float> distances;
        Vector2D<float> delta;
        Vector2D<float> scaleCoeffs;


        if(bufferRay <= deg90) { // North-east
            delta.x = 1.0f - getFractialPart(test.x);
            scaleCoeffs.x = sinf(ray);
            delta.y = 1.0f - getFractialPart(test.y);
            scaleCoeffs.y = cosf(ray);
        } else if(bufferRay <= deg180) { // South-east
            delta.x = 1.0f - getFractialPart(test.x);
            scaleCoeffs.x = sinf(ray);
            delta.y = getFractialPart(test.y);
            scaleCoeffs.y = cosf(ray + pi);
        } else if(bufferRay <= deg270) { // South-west
            delta.x = getFractialPart(test.x);
            scaleCoeffs.x = sinf(ray + pi);
            delta.y = getFractialPart(test.y);
            scaleCoeffs.y = cosf(ray + pi);
        } else { // North-west
            delta.x = getFractialPart(test.x);
            scaleCoeffs.x = sinf(ray + pi);
            delta.y = 1.0f - getFractialPart(test.y - blockBitSize); // without constant ray overshoots walls by x axis
            scaleCoeffs.y = cosf(ray);
        }

         // Delta should not be zero otherwise renderer will loop forever.
        if(delta.x < blockBitSize) {
            delta.x = blockBitSize;
        }
        if(delta.y < blockBitSize) {
            delta.y = blockBitSize;
        }

        distances.x = delta.x / clamp(scaleCoeffs.x, std::numeric_limits<float>::min(), 1.0f);

        distances.y = delta.y / clamp(scaleCoeffs.y, std::numeric_limits<float>::min(), 1.0f);

        return distances.x < distances.y ? distances.x : distances.y;

    } else {
        return blockBitSize;
    }
}

void loadTextures(std::vector<SDL_Surface*>& txt) {
    loadTexture(txt, "wall2.bmp");
    loadTexture(txt, "wall.bmp");
    loadTexture(txt, "wall3.bmp");
    loadTexture(txt, "wall2.bmp");
    loadTexture(txt, "hellaworld.bmp");
}


void loadLightmaps(std::vector<SDL_Surface*>& lmp) {
    loadTexture(lmp, "wall2bumpmap.bmp");
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

void mirrorTexture(SDL_Surface* txt) {
    for(int i = 0; i < txt->h; ++i) {
        for(int j = 0; j < txt->w / 2; ++j) {
            Uint32* left = getTexturePixel(txt, i, j);
            Uint32* right = getTexturePixel(txt, i, txt->w - j - 1);

            Uint32 temp = *left;
            *left = *right;
            *right = temp;
        }
    }
}

void mirrorTextures(std::vector<SDL_Surface*>& txt) {
    for(auto i = txt.begin(); i != txt.end(); i++) {
        mirrorTexture(*i);
    }
}

void applyLightMapToTexture(SDL_Surface* texture, SDL_Surface* lightmap)
{
    Vector2D<float> coeffs;
    coeffs.x = (float)lightmap->w / (float)texture->w ;
    coeffs.y = (float)lightmap->h / (float)texture->h;
    for(int i = 0; i < texture->h; ++i) {
        for(int j = 0; j < texture->w; ++j) {

            Uint32* texPixel = getTexturePixel(texture, i, j);
            Uint32* lightmapPixel = getTexturePixel(lightmap, (int)(i * coeffs.y),(int)(j * coeffs.x));

            SDL_Color pixelColor = UintToColor(*texPixel);
            pixelColor = transformColorByLightMap(pixelColor, UintToColor(*lightmapPixel));

            *texPixel = ColorToUint(pixelColor.r, pixelColor.g, pixelColor.b);
        }
    }
}

void doLightMapsToAllTextures(std::vector<SDL_Surface*>& txt,
                              std::vector<SDL_Surface*>& lmp)
{
    for(int i = 0; i < mapHeight; ++i) {
        for(int j = 0; j < mapWidth; ++j) {
            if(map[i][j].getIsLightMapped() && map[i][j].getIsTextured()) {
                applyLightMapToTexture(txt.at(map[i][j].getTextureIndex()),
                                       lmp.at(map[i][j].getLightMapIndex()));
            }
        }
    }
}

void setLightMapsTo0()
{
    for(int i = 0; i < mapHeight; ++i) {
        for(int j = 0; j < mapWidth; ++j) {
            map[i][j].lightmap = 0;
        }
    }
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif

void transposeTexture(SDL_Surface** txt)
{
    SDL_Surface* newTxt = SDL_CreateRGBSurface(0, (*txt)->h, (*txt)->w,
                                               (*txt)->format->BitsPerPixel,
                                               rmask, gmask, bmask, amask);
    for(int i = 0; i < (*txt)->h; ++i) {
        for(int j = 0; j < (*txt)->w; ++j) {
            Uint32* oldPixel = getTexturePixel(*txt, i, j);
            Uint32* newPixel = getTransposedTexturePixel(newTxt, i, j);

            *newPixel = *oldPixel;
        }
    }

    SDL_FreeSurface(*txt);
    *txt = newTxt;
}

void transposeTextures(std::vector<SDL_Surface*>& txts)
{
    for(int i = 0; i < txts.size(); ++i) {
        transposeTexture(&txts.at(i));
    }
}

void fillUpTheStars() {
    std::mt19937 random;
    random.seed(1);
    stars = (bool*)realloc(stars, starsWidth * starsHeight);
    memset(stars, 0, starsWidth * starsHeight);

    for(int i = 0; i < starsHeight; ++i) {
        for(int j = 0; j < starsWidth; ++j) {
            if(!(random() % screenWidth / 2)) {
                stars[i * starsWidth + j] = true;
            }
        }
    }
}

void setWindowPos(int x, int y)
{
    char env[80];
    sprintf(env, "SDL_VIDEO_WINDOW_POS=%d,%d", x, y);

    SDL_putenv(env);
}

#endif // UTILITY_H_INCLUDED
