#pragma once

#ifndef RENDERING_H_INCLUDED
#define RENDERING_H_INCLUDED

#include "Utility.h"
#include "GameData.h"

struct RenderData
{
    Uint32 skyLightColor;
    Uint32 fogColor;
    Uint32 wallColorPixel;

    SDL_Color floorColor;
    SDL_Color skyColor;

    SDL_Surface* texture;
    SDL_Surface* lightmap;

    int skyTextureIndex;
    int skyWidthIndex;
    int ceilingHeight;
    int floorHeight;
    int wallSizeOnScreen;

    float distanceToAWall;
    float scalingVar;

    bool isTextured;
    bool isLightMap;
    bool shouldStarsBeRendered;
    bool shouldTextureBeMirrored;
    bool isHorisontal;
};

void rayTraversal(GameData& gamedata, float ray, float* distArray, Vector2D<float>* rayPosArray, int j = 0)
{
        using namespace Constants;
        float distanceToAWall = 0.0f;

        Vector2D<float> eye;

        eye.x = sinf(ray);
        eye.y = cosf(ray);

        Vector2D<float> test;
        test.x = gamedata.player.x;
        test.y = gamedata.player.y;

        int wasWallHit = 0;

        ray = clampLooping(ray, 0.0f, pi * 2);

        while(!wasWallHit) // Ray traversal
        {
            distanceToAWall += getDistanceToTheNearestIntersection(test, ray, eye.x, eye.y);

            test.x = gamedata.player.x + eye.x * distanceToAWall;
            test.y = gamedata.player.y + eye.y * distanceToAWall;

            if(!withinRange(test.x, 0.0f, (float)mapWidth) ||
               !withinRange(test.y, 0.0f, (float)mapHeight))
            {
                wasWallHit = 1;
                distanceToAWall = offMapDepth;
           } else {
                wasWallHit = !(int)gamedata.map[(int)test.y][(int)test.x].isEmpty();
            }
        }
//        test.x = clamp(test.x, 0.0f, (float)mapWidth);
//        test.y = clamp(test.y, 0.0f, (float)mapHeight);

        // Constant gives slightly better fish-eye correction. Without it walls are a little bit more 'rounded'
        distanceToAWall *= cosf(ray - gamedata.player.angle - (FOV / (screenWidth * 8)));

        if(distArray) distArray[j] = distanceToAWall;
        if(rayPosArray) rayPosArray[j] = test;
}

inline Uint32 getSkyGradientedColor(const SDL_Color color, const int i, const int horLine)
{
    return ColorToUint(
                clamp((int)(color.r * (float)(i - horLine + 256) / 256), 0, 255),
                clamp((int)(color.g * (float)(i - horLine + 256) / 256), 0, 255),
                clamp((int)(color.b * (float)(i - horLine + 256) / 256), 0, 255)
            );
}

inline Uint32 getFloorGradientedColor(const SDL_Color color, const int i, const int horLine)
{
    using namespace Constants;
    return ColorToUint(
                clamp((int)(color.r * (float)(screenHeight - i + horLine / 2 + 192) / 256), 0, 200),
                clamp((int)(color.g * (float)(screenHeight - i + horLine / 2 + 192) / 256), 0, 200),
                clamp((int)(color.b * (float)(screenHeight - i + horLine / 2 + 192) / 256), 0, 200)
            );
}

Uint32 getStarColorPixel()
{
    return ColorToUint(
                clamp(dist(random), 165, 255),
                clamp(dist(random), 165, 255),
                clamp(dist(random), 165, 255)
    );
}

inline Uint32 fastPixelShadowing(const Uint32 pix)
{
    return (pix >> 1) & 0x7F7F7F;
}

inline Uint32 applyWallGradientToPixel(const Uint32 pix, const float distanceToAWall)
{
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

inline Uint32 getShadowedWallColor(const SDL_Color color, const float distanceToAWall)
{
    return ColorToUint(
                clamp((int)((color.r * 0.7f) * (distanceToAWall / 2)), (int)(color.r / 1.2f), 255),
                clamp((int)((color.g * 0.7f) * (distanceToAWall / 2)), (int)(color.g / 1.2f), 255),
                clamp((int)((color.b * 0.7f) * (distanceToAWall / 2)), (int)(color.b / 1.2f), 255)
            );
}

inline Uint32 getGradientedWallColor(const SDL_Color color, const float distanceToAWall)
{
    return ColorToUint(
                clamp((int)(color.r * (distanceToAWall / 2)), (int)color.r, 255),
                clamp((int)(color.g * (distanceToAWall / 2)), (int)color.g, 255),
                clamp((int)(color.b * (distanceToAWall / 2)), (int)color.b, 255)
            );
}


Uint32 noise(Uint32 color)
{
    static Uint32 greyscale[4];

    greyscale[0] = ColorToUint(32, 32, 32);
    greyscale[1] = ColorToUint(64, 64, 64);
    greyscale[2] = ColorToUint(128, 128, 128);
    greyscale[3] = ColorToUint(192, 192, 192);

    return blend(color, greyscale[dist(random) & 3], 32);
}

inline Uint32 doFloorFog(Uint32 source, Uint32 fogColor, int i, GameData& data)
{
    using namespace Constants;
    Uint8 alpha = clamp((starsHeight -
                (i + (horizonCap - data.horizonLine))) /
                 2 / (starsHeight / 2 / 256), 0, 255);
    return blend(source, fogColor, alpha);
}

inline Uint32 doCeilingFog(Uint32 source, Uint32 fogColor, int i, GameData& data)
{
    using namespace Constants;
    Uint8 alpha = clamp((i + (Constants::horizonCap - data.horizonLine)) /
                        2 / (starsHeight / 2 / 256), 0, 255);
    return blend(source, fogColor, alpha);
}

inline Uint32 renderCeiling(ControlState& ctrls, GameData& gamedata,
                            RenderData& r_data, int i)
{
    using namespace Constants;
    Uint32 pixelColor;
    // If star exists in the stars map for the current location set white flickering pixel
    if(ctrls.shouldStarsBeRendered &&
       gamedata.stars[(i + (horizonCap - gamedata.horizonLine)) * starsWidth + r_data.skyWidthIndex]) {
       pixelColor = getStarColorPixel();
    } else {
        // Do gradiented color sky or retrieve pixel out of sky box
        if(!ctrls.texturedSky) {
            pixelColor = getSkyGradientedColor(r_data.skyColor, i, gamedata.horizonLine);
        } else {
                pixelColor = *getTransposedScaledTexturePixel(gamedata.sky_textures[0],
                              starsWidth, starsHeight,
                              i + (horizonCap - gamedata.horizonLine), r_data.skyTextureIndex);
        }
        if(ctrls.fog) {
            pixelColor = doCeilingFog(pixelColor, r_data.fogColor, i, gamedata);
        }
    }
    return pixelColor;
}

inline renderFloor(ControlState ctrls, GameData& gamedata, RenderData& r_data, int i)
{
    using namespace Constants;
    Uint32 pixelColor;
    if(!ctrls.isFloorASky) {
        pixelColor = getFloorGradientedColor(r_data.floorColor, i, gamedata.horizonLine);
        if(ctrls.night) pixelColor = fastPixelShadowing(pixelColor);
        if(ctrls.fog) {
            pixelColor = doFloorFog(pixelColor, r_data.fogColor, i, gamedata);
        }
        if(ctrls.coloredLight) pixelColor = blend(pixelColor, r_data.skyLightColor, 92);
    } else {
        if(ctrls.shouldStarsBeRendered &&
           gamedata.stars[(i + (horizonCap - gamedata.horizonLine))
                           * starsWidth + r_data.skyWidthIndex]) {

            pixelColor = getStarColorPixel();
        } else {
            if(!ctrls.texturedSky) {
                pixelColor = getSkyGradientedColor(r_data.skyColor, i, gamedata.horizonLine);
            } else {
                pixelColor = *getTransposedScaledTexturePixel(gamedata.sky_textures[0],
                                starsWidth, starsHeight,
                                i + (horizonCap - gamedata.horizonLine),
                                                        r_data.skyTextureIndex);
            }
            if(ctrls.fog) {
                pixelColor = doFloorFog(pixelColor, r_data.fogColor, i, gamedata);
            }
        }
    }

    return pixelColor;
}

inline Uint32 renderWall(ControlState& ctrls, GameData& gamedata, RenderData& r_data, int i)
{
    using namespace Constants;
    Uint32 pixelColor;
    Uint32* texturePixel = NULL;
    Uint32* lightmapPixel = NULL;

    // Textured wall routine
    if(r_data.isTextured) {
        // Swaped w and h because the texture is transposed. Same with lightmaps.
        texturePixel = getTransposedTexturePixel(r_data.texture,
                (int)((i - r_data.ceilingHeight) * ((float)r_data.texture->w / (float)r_data.wallSizeOnScreen)),
                (int)(getFractialPart(r_data.scalingVar) * (float)r_data.texture->h));

            pixelColor = *texturePixel;

        if(ctrls.textureGradient) {
            pixelColor = applyWallGradientToPixel(pixelColor, r_data.distanceToAWall);
        }

        // Darkens walls for an illusion of directed light
        if(whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::WEST   ||
           whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::NORTH    ) {
            // Doing fast pixel transformation for the texture, so the performance won't suffer
                pixelColor = fastPixelShadowing(pixelColor);
        }

    } else {
        pixelColor = r_data.wallColorPixel;
    }
    if(r_data.isLightMap) {
            // If current wall is lightmapped(on runtime) then transform pixel accordingly.
            // Very slow so it's never used.
            lightmapPixel = getTransposedTexturePixel(r_data.lightmap,
                    (int)((i - r_data.ceilingHeight) * ((float)r_data.lightmap->w /
                    (float)r_data.wallSizeOnScreen)),
                    (int)(getFractialPart(r_data.scalingVar) * (float)r_data.lightmap->h));

            SDL_Color texColor = UintToColor(pixelColor);
            SDL_Color lightColor = UintToColor(*lightmapPixel);
            SDL_Color finalColor = transformColorByLightMap(texColor, lightColor);

            pixelColor = ColorToUint(finalColor.r, finalColor.g, finalColor.b);
        }

     // If night mode enabled darken pixel even more
    if(ctrls.night) {
            pixelColor = fastPixelShadowing(pixelColor);
    }

    if(ctrls.fog) {
        pixelColor = blend(pixelColor, r_data.fogColor,
                     clamp((Uint8)(r_data.distanceToAWall * 12), (Uint8)0, (Uint8)255));
    }

    if(ctrls.coloredLight) pixelColor = blend(pixelColor, r_data.skyLightColor, 92);

    return pixelColor;
}

#endif // RENDERING_H_INCLUDED
