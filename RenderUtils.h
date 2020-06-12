#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include "Utility.h"
#include "randomBS.h"
#include "GameData.h"

namespace RenderUtils
{
    void applyLightMapToTexture(SDL_Surface* texture,
                                SDL_Surface* lightmap);

    void doLightMapsToAllTextures(std::vector<SDL_Surface*>& txt,
                              std::vector<SDL_Surface*>& lmp,
                              GameData& d);

    void setLightMapsTo0(GameData& d);

    void transposeTexture(SDL_Surface** txt);

    Uint32* getSpherePixelOld(SDL_Surface* txt, float height,
                              float sine, float cosine, int i, int j);

    Uint32* getSpherePixel(SDL_Surface* txt, float height, float angle);

    Uint32 ColorToUint(int R, int G, int B);

    SDL_Color UintToColor(Uint32 color);

    inline Uint32 blend(Uint32 color1, Uint32 color2, Uint8 alpha)
    {
        Uint32 rb = color1 & 0xff00ff;
        Uint32 g  = color1 & 0x00ff00;
        rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
        g  += ((color2 & 0x00ff00) -  g) * alpha >> 8;
        return (rb & 0xff00ff) | (g & 0xff00);
    }

    inline Uint32 getSkyGradientedColor(SDL_Color color, int i, int horLine)
    {
        using namespace util;
        return ColorToUint(
                    clamp((int)(color.r * (float)(i - horLine + 256) / 256), 0, 255),
                    clamp((int)(color.g * (float)(i - horLine + 256) / 256), 0, 255),
                    clamp((int)(color.b * (float)(i - horLine + 256) / 256), 0, 255)
                );
    }

    inline Uint32 getFloorGradientedColor(SDL_Color color, int i, int horLine)
    {
        Constants& c = Constants::get();
        using namespace util;
        return ColorToUint(
                    clamp((int)(color.r * (float)(c.screenHeight - i + horLine / 2 + 192) / 256), 0, 200),
                    clamp((int)(color.g * (float)(c.screenHeight - i + horLine / 2 + 192) / 256), 0, 200),
                    clamp((int)(color.b * (float)(c.screenHeight - i + horLine / 2 + 192) / 256), 0, 200)
                );
    }

    inline Uint32 getStarColorPixel()
    {
        using namespace util;
        return ColorToUint(
                    clamp(dist(random_), 165, 255),
                    clamp(dist(random_), 165, 255),
                    clamp(dist(random_), 165, 255)
        );
    }

    inline Uint32 fastPixelShadowing(Uint32 pix)
    {
        return (pix >> 1) & 0x7F7F7F;
    }

    inline Uint32 applyWallGradientToPixel(Uint32 pix, float distanceToAWall)
    {
        using namespace util;
        SDL_Color pixelRGB = UintToColor(pix);
        return ColorToUint(
                    clamp((int)((pixelRGB.r / 3) * (distanceToAWall * 16) / 32),
                          (int)pixelRGB.r / 3, clamp((int)(pixelRGB.r * 1.1), 0, 255)),
                    clamp((int)((pixelRGB.g / 3) * (distanceToAWall * 16) / 32),
                          (int)pixelRGB.g / 3, clamp((int)(pixelRGB.g * 1.1), 0, 255)),
                    clamp((int)((pixelRGB.b / 3) * (distanceToAWall * 16) / 32),
                          (int)pixelRGB.b / 3, clamp((int)(pixelRGB.b * 1.1), 0, 255))
                 );
    }

    inline Uint32 getShadowedWallColor( SDL_Color color,  float distanceToAWall)
    {
        using namespace util;
        return ColorToUint(
                    clamp((int)((color.r * 0.7f) * (distanceToAWall / 2)), (int)(color.r / 1.2f), 255),
                    clamp((int)((color.g * 0.7f) * (distanceToAWall / 2)), (int)(color.g / 1.2f), 255),
                    clamp((int)((color.b * 0.7f) * (distanceToAWall / 2)), (int)(color.b / 1.2f), 255)
                );
    }

    inline Uint32 getGradientedWallColor( SDL_Color color,  float distanceToAWall)
    {
        using namespace util;
        return ColorToUint(
                    clamp((int)(color.r * (distanceToAWall / 2)), (int)color.r, 255),
                    clamp((int)(color.g * (distanceToAWall / 2)), (int)color.g, 255),
                    clamp((int)(color.b * (distanceToAWall / 2)), (int)color.b, 255)
                );
    }


    inline Uint32 noise(Uint32 color)
    {
        static Uint32 greyscale[4];

        greyscale[0] = ColorToUint(32, 32, 32);
        greyscale[1] = ColorToUint(64, 64, 64);
        greyscale[2] = ColorToUint(128, 128, 128);
        greyscale[3] = ColorToUint(192, 192, 192);

        return blend(color, greyscale[dist(random_) & 3], 32);
    }

    inline Uint32 doFloorFog(Uint32 source, Uint32 fogColor, int i, const GameData& data)
    {
        Constants& c = Constants::get();
        Uint8 alpha = util::clamp((c.starsHeight -
                    (i + (c.horizonCap - data.horizonLine))) /
                     2 / (c.starsHeight / 2 / 256), 0, 255);
        return blend(source, fogColor, alpha);
    }

    inline Uint32 doCeilingFog(Uint32 source, Uint32 fogColor, int i, const GameData& data)
    {
        Constants& c = Constants::get();
        Uint8 alpha = util::clamp((i + (c.horizonCap - data.horizonLine)) /
                            2 / (c.starsHeight / 2 / 256), 0, 255);
        return blend(source, fogColor, alpha);
    }

    inline float getSkyTextureHeightRad(int i, int horline)
    {
        Constants& c = Constants::get();
        return (float)((i + (c.horizonCap - horline)) * (c.pi) / c.starsHeight);
    }

    inline float getSkyTextureHeight(int i, int horline)
    {
        Constants& c = Constants::get();
        return (float)((i + (c.horizonCap - horline)) / (float)c.starsHeight);
    }

    inline float getSkyTextureHeightRev(int i, int horline)
    {
        Constants& c = Constants::get();
        return getSkyTextureHeight(c.screenHeight - 1 - i, -horline);
    }

    inline float getSkyTextureHeightRevRad(int i, int horline)
    {
        Constants& c = Constants::get();
        return getSkyTextureHeightRad(c.screenHeight - 1 - i, -horline);
    }

    inline Uint8 getAlpha(Uint32 arg)
    {
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            return (Uint8)arg;
        #else
            return (Uint8)arg >> 24;
        #endif // SDL_BYTEORDER
    }

    inline Uint32* getTexturePixel(SDL_Surface* surf, int i, int j) {
        return (Uint32*)(surf->pixels + i * surf->pitch + j * sizeof(Uint32));
    }

    inline Uint32* getTransposedTexturePixel(SDL_Surface* surf, int i, int j) {
        return (Uint32*)(surf->pixels + j * surf->pitch + i * sizeof(Uint32));
    }

    inline SDL_Color transformColorByLightMap(SDL_Color color, const SDL_Color lightmapColor)
    {
        using namespace util;

        color.r = clamp(color.r + lightmapColor.r - 192, 0, 255);
        color.g = clamp(color.g + lightmapColor.g - 192, 0, 255);
        color.b = clamp(color.b + lightmapColor.b - 192, 0, 255);

        return color;
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

    void transposeTextures(std::vector<SDL_Surface*>& txts);

    void setWindowPos(int x, int y);

    void mirrorTexture(SDL_Surface* txt);

    void mirrorTextures(std::vector<SDL_Surface*>& txt);

}

#endif // RENDERUTILS_H
