#pragma once

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <iostream>
#include <vector>
#include <random>
#include <ostream>

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

template<typename T>
void print2dVector(std::vector<std::vector<T>>& vec, std::ostream& where)
{
    if(vec.empty() || vec[0].empty()) return;
    for(size_t i = 0; i < vec.size(); ++i) {
        for(size_t j = 0; j < vec[0].size(); ++j) {
            where << vec[i][j];
        }
        where << std::endl;
    }
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

inline Uint32 ColorToUint(int R, int G, int B)
{
	return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

inline SDL_Color UintToColor(Uint32 color) {
  SDL_Color retColor;

  retColor.unused = (color >> 24) & 0xFF;
  retColor.r = (color >> 16) & 0xFF;
  retColor.g = (color >> 8) & 0xFF;
  retColor.b = color & 0xFF;

  return retColor;
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

inline Uint32 blend(Uint32 color1, Uint32 color2, Uint8 alpha)
{
	Uint32 rb = color1 & 0xff00ff;
	Uint32 g  = color1 & 0x00ff00;
	rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
	g  += ((color2 & 0x00ff00) -  g) * alpha >> 8;
	return (rb & 0xff00ff) | (g & 0xff00);
}

inline Uint32* getTexturePixel(SDL_Surface* surf, int i, int j) {
    return (Uint32*)(surf->pixels + i * surf->pitch + j * sizeof(Uint32));
}

inline Uint32* getTransposedTexturePixel(SDL_Surface* surf, int i, int j) {
    return (Uint32*)(surf->pixels + j * surf->pitch + i * sizeof(Uint32));
}

inline void loadTexture(std::vector<SDL_Surface*>& txt, const char* filename)
{
    SDL_Surface* surf = SDL_LoadBMP(filename);
    SDL_Surface* texture = SDL_DisplayFormat(surf);
    if(!surf || !texture) {
        fprintf(stderr, "%s", "Unable to load textures. Exiting.....\n");
        fprintf(stderr, "%s", SDL_GetError());
        exit(-1);
    }
    SDL_FreeSurface(surf);
    txt.push_back(texture);
}

inline SDL_Color transformColorByLightMap(SDL_Color color, const SDL_Color lightmapColor) {
        color.r = clamp(color.r + lightmapColor.r - 192, 0, 255);
        color.g = clamp(color.g + lightmapColor.g - 192, 0, 255);
        color.b = clamp(color.b + lightmapColor.b - 192, 0, 255);

        return color;
}

#include "GameConstants.h"
#include "GameData.h"
#include "Vector2D.h"

inline float directionToAngle(int x, int y)
{
    using namespace Constants;
    if(x == 0 && y == 0) return -std::numeric_limits<float>::infinity();

    if ( x >=  1 && y ==  0 ) return 0.0f;
    if ( x >=  1 && y >=  1 ) return deg45;
    if ( x ==  0 && y >=  1 ) return deg90;
    if ( x <= -1 && y >=  1 ) return deg90 + deg45;
    if ( x <= -1 && y ==  0 ) return deg180;
    if ( x <= -1 && y <= -1 ) return deg180 + deg45;
    if ( x ==  0 && y <= -1 ) return deg270;
    if ( x >=  1 && y <= -1 ) return deg270 + deg45;
}

inline float degreesToRad(float degrees) {
    return degrees * (Constants::pi / 180);
}

inline float angleBetweenPoints(Vector2D<float> a, Vector2D<float> b)
{
    float deltaY = std::abs(b.y - a.y);
    float deltaX = std::abs(b.x - a.x);
    return std::atan2(deltaY, deltaX);
}

inline float getDistanceToTheNearestIntersection(const Vector2D<float>& test, float ray, float sine, float cosine)
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
     using namespace Constants;


        Vector2D<float> distance;
        Vector2D<float> delta;
        Vector2D<float> scaleCoeffs;


        int whichQuarter = (int)(ray / (pi / 2));

        switch(whichQuarter) {
            case 0:
                delta.x = 1.0f - getFractialPart(test.x);
                scaleCoeffs.x = sine;
                delta.y = 1.0f - getFractialPart(test.y);
                scaleCoeffs.y = cosine;
            break;
            case 1:
                delta.x = 1.0f - getFractialPart(test.x);
                scaleCoeffs.x = sine;
                delta.y = getFractialPart(test.y);
                scaleCoeffs.y = -cosine;
            break;
            case 2:
                delta.x = getFractialPart(test.x);
                scaleCoeffs.x = -sine;
                delta.y = getFractialPart(test.y);
                scaleCoeffs.y = -cosine;
            break;
            case 3:
                delta.x = getFractialPart(test.x);
                scaleCoeffs.x = -sine;
                delta.y = 1.0f - getFractialPart(test.y); // without constant ray overshoots walls by x axis
                scaleCoeffs.y = cosine;
            break;
            default:
                return naiveBlockBitSize;
            break;
        }

         // Delta should not be zero otherwise renderer will loop forever.
        if(delta.x < blockBitSize) {
            delta.x = blockBitSize;
        }
        if(delta.y < blockBitSize) {
            delta.y = blockBitSize;
        }


        distance.x = delta.x / scaleCoeffs.x;

        distance.y = delta.y / scaleCoeffs.y;

        return distance.x < distance.y ? distance.x : distance.y;
}

inline void loadTextures(std::vector<SDL_Surface*>& txt) {
    loadTexture(txt, "wall2.bmp");
    loadTexture(txt, "wall.bmp");
    loadTexture(txt, "wall3.bmp");
    loadTexture(txt, "wall2.bmp");
    loadTexture(txt, "hellaworld.bmp");
}


inline void loadLightmaps(std::vector<SDL_Surface*>& lmp) {
    loadTexture(lmp, "wall2bumpmap.bmp");
}

inline Uint32* getScaledTexturePixel(SDL_Surface* txt,
                              int yourWidth, int yourHeight, int i, int j) {
    Vector2D<float> coeffs;
    coeffs.x = (float)txt->w / (float)yourWidth;
    coeffs.y = (float)txt->h / (float)yourHeight;

    return getTexturePixel(txt, (int)(i * coeffs.y),(int)(j * coeffs.x));
}

inline Uint32* getTransposedScaledTexturePixel(SDL_Surface* txt,
                              int yourWidth, int yourHeight, int i, int j) {
    Vector2D<float> coeffs;
    coeffs.x = (float)txt->h / (float)yourWidth;
    coeffs.y = (float)txt->w / (float)yourHeight;

    return getTransposedTexturePixel(txt, (int)(i * coeffs.y),(int)(j * coeffs.x));
}

inline void fillUpTheMapToBeBox(MapBlock** aMap)
{
    using namespace Constants;
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

inline void mirrorTexture(SDL_Surface* txt) {
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

inline void mirrorTextures(std::vector<SDL_Surface*>& txt) {
    for(auto i = txt.begin(); i != txt.end(); i++) {
        mirrorTexture(*i);
    }
}

inline void loadSkyTextures(std::vector<SDL_Surface*>& skyTxt)
{
    loadTexture(skyTxt, "sky.bmp");
}

inline void applyLightMapToTexture(SDL_Surface* texture, SDL_Surface* lightmap)
{
    Vector2D<float> coeffs;
    coeffs.x = (float)lightmap->w / (float)texture->w;
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

inline void doLightMapsToAllTextures(std::vector<SDL_Surface*>& txt,
                              std::vector<SDL_Surface*>& lmp,
                              GameData& d)
{
    using namespace Constants;
    for(int i = 0; i < mapHeight; ++i) {
        for(int j = 0; j < mapWidth; ++j) {
            if(d.map[i][j].getIsLightMapped() && d.map[i][j].getIsTextured()) {
                applyLightMapToTexture(txt.at(d.map[i][j].getTextureIndex()),
                                       lmp.at(d.map[i][j].getLightMapIndex()));
            }
        }
    }
}

inline void setLightMapsTo0(GameData& d)
{
    using namespace Constants;
    for(int i = 0; i < mapHeight; ++i) {
        for(int j = 0; j < mapWidth; ++j) {
            d.map[i][j].lightmap = 0;
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

inline void transposeTexture(SDL_Surface** txt)
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

inline Uint8 getAlpha(Uint32 arg) {
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return (Uint8)arg;
    #else
        return (Uint8)arg >> 24;
    #endif // SDL_BYTEORDER
}

inline void loadSprites(std::vector<Sprite>& sprts)
{
    Sprite spr({0, 0, 0, 0}, "");
    sprts.push_back(spr);
}

inline void transposeTextures(std::vector<SDL_Surface*>& txts)
{
    for(size_t i = 0; i < txts.size(); ++i) {
        transposeTexture(&txts.at(i));
    }
}

inline void setWindowPos(int x, int y)
{
    char env[80];
    sprintf(env, "SDL_VIDEO_WINDOW_POS=%d,%d", x, y);

    SDL_putenv(env);
}

inline SideOfAWall whichSide(bool isMirrored, bool isHorisontal)
{
    if(!isMirrored &&  isHorisontal)    return SideOfAWall::NORTH;
    if(!isMirrored && !isHorisontal)    return SideOfAWall::WEST;
    if( isMirrored &&  isHorisontal)    return SideOfAWall::SOUTH;
    if( isMirrored && !isHorisontal)    return SideOfAWall::EAST;
}

#endif // UTILITY_H_INCLUDED
