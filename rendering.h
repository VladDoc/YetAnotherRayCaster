#pragma once

#ifndef RENDERING_H_INCLUDED
#define RENDERING_H_INCLUDED

#include "Utility.h"
#include "GameData.h"

void rayTraversal(GameData& gamedata, float ray, float* distArray, Vector2D<float>* rayPosArray, int j = 0)
{
        float distanceToAWall = 0.0f;

        Vector2D<float> eye;

        eye.x = sinf(ray);
        eye.y = cosf(ray);

        Vector2D<float> test;
        test.x = gamedata.player.x;
        test.y = gamedata.player.y;

        int wasWallHit = 0;

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
                clamp((int)((pixelRGB.r / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.r / 3, clamp((int)(pixelRGB.r * 1.1), 0, 255)),
                clamp((int)((pixelRGB.g / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.g / 3, clamp((int)(pixelRGB.g * 1.1), 0, 255)),
                clamp((int)((pixelRGB.b / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.b / 3, clamp((int)(pixelRGB.b * 1.1), 0, 255))
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

#endif // RENDERING_H_INCLUDED
