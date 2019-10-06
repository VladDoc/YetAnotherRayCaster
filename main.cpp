#include <cstdlib>

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdio>

#include <SDL/SDL.h>

#include "MapBlock.h"
#include "ControlsState.h"
#include "Utility.h"
#include "GameData.h"
#include "Input.h"
#include "player.h"
#include "Vector2D.h"


void freeTextures() {
    for(auto i = textures.begin(); i != textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = lightmaps.begin(); i != lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }

    for(auto i = m_textures.begin(); i != textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = m_lightmaps.begin(); i != lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }
}


void renderColumn(int j, SDL_Surface* screen) {

        float ray = (player.angle - FOV / 2.0f) + ((float)j / (float)screenWidth) * FOV;
        float distanceToAWall = 0.0f;

        Vector2D<float> eye;

        eye.x = sinf(ray);
        eye.y = cosf(ray);

        Vector2D<float> test;
        test.x = player.x;
        test.y = player.y;

        int wasWallHit = 0;
        SDL_Color wallColor;

        while(!wasWallHit && distanceToAWall < depth) // Ray traversal
        {
            distanceToAWall += getDistanceToTheNearestIntersection(test, ray);


            test.x = player.x + eye.x * distanceToAWall;
            test.y = player.y + eye.y * distanceToAWall;

            if(test.x <= 0.0f || test.x >= mapWidth || test.y <= 0.0f || test.y >= mapHeight)
            {
                wasWallHit = 1;
                distanceToAWall = depth;
                wallColor = UintToColor(defWallColor);
           } else {
                wasWallHit = !(int)map[(int)test.y][(int)test.x].isEmpty();


                if(map[(int)test.y][(int)test.x].texture == 1) {
                    wallColor = UintToColor(defWallColor);
                } else {
                    wallColor = map[(int)test.y][(int)test.x].getColor();
                }
            }
        }

        // Constant gives slightly better fish-eye correction. Without it walls are a little bit more 'rounded'
        distanceToAWall *= cosf(ray - player.angle - (FOV / (screenWidth * 8)));

        int ceilingHeight = 0;
        if(!easterEgg) {
            ceilingHeight = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToAWall);
        } else {
            ceilingHeight = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToAWall) + abs(j  - screenWidth / 2);
        }
        int floorHeight = screenHeight - ceilingHeight;

        // Increases height of a wall
        ceilingHeight -= (floorHeight - ceilingHeight) * map[(int)test.y][(int)test.x].height - (floorHeight - ceilingHeight);

        ceilingHeight += horizonLine;
        floorHeight += horizonLine;

        float bufferRay = clampLooping(ray, 0.0f, pi * 2);
        int skyWidthIndex = (int)(screenWidth * (bufferRay / FOV));

        bool shouldTextureBeMirrored = false;
        bool isHorisontal = false;
        float checkY = test.y;
        float scalingVar;

        if(map[(int)(checkY - horisontalBlockCheckStep)][(int)test.x].isEmpty() ||
           map[(int)(checkY + horisontalBlockCheckStep)][(int)test.x].isEmpty()   ) {
                isHorisontal = true;
                scalingVar = test.x; // Then wall is along horizontal axis
                if(getFractialPart(test.y) > 0.5f) { // If y component greater than a half then it's a north wall
                    shouldTextureBeMirrored = true;
                }
        } else {
                scalingVar = test.y;
                if(getFractialPart(test.x) < 0.5f) {
                    shouldTextureBeMirrored = true;
                }
        }

        MapBlock currentBlock = map[(int)test.y][(int)test.x];

        SDL_Surface* texture = NULL;
        SDL_Surface* lightmap = NULL;

        bool isTextured = currentBlock.getIsTextured();
        bool isLightMap = currentBlock.getIsLightMapped();


        if(!shouldTextureBeMirrored) {
            // Sometimes if works with false boolean, which causes game to segfault, to prevent that I clamp index.
            if(isTextured) texture = textures[clamp(currentBlock.getTextureIndex(), 0, (int)textures.size())];
            if(isLightMap) lightmap = lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)lightmaps.size())];
        } else {
            if(isTextured) texture = m_textures[clamp(currentBlock.getTextureIndex(), 0, (int)textures.size())];
            if(isLightMap) lightmap = m_lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)lightmaps.size())];
        }


        for(int i = 0; i < screenHeight; ++i)
        {
            Uint32* pixel = getTexturePixel(screen, i, j);
            if(i < ceilingHeight)
            {
                Uint32 pixelColor;

                if(shouldStarsBeRendered && stars[(i + (horizonCap - horizonLine)) * starsWidth + skyWidthIndex]) {
                    pixelColor = ColorToUint(clamp(rand() % 256, 165, 255),
                                             clamp(rand() % 256, 165, 255),
                                             clamp(rand() % 256, 165, 255));
                } else {
                    pixelColor = ColorToUint(clamp((int)(skyColor.r * (float)(i - horizonLine + 256) / 256), 0, 255),
                                             clamp((int)(skyColor.g * (float)(i - horizonLine + 256) / 256), 0, 255),
                                             clamp((int)(skyColor.b * (float)(i - horizonLine + 256) / 256), 0, 255));
                }
                *pixel = pixelColor;
            }
            else if(i >= ceilingHeight && i < floorHeight)
            {
                int wallSizeOnScreen = floorHeight - ceilingHeight;
                Uint32 pixelColor = defWallColor;
                Uint32* texturePixel = NULL;
                Uint32* lightmapPixel = NULL;

                    if(isTextured) {
                        // Swaped w and h because the texture is transposed. Same with lightmaps.
                        texturePixel = getTransposedTexturePixel(texture, (int)((i - ceilingHeight) * ((float)texture->w / (float)wallSizeOnScreen)),
                                                       (int)(getFractialPart(scalingVar) * (float)texture->h));
                        pixelColor = *texturePixel;
                    if(textureGradient) {
                        SDL_Color pixelRGB = UintToColor(pixelColor);

                        pixelColor = ColorToUint(clamp((int)((pixelRGB.r / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.r / 3, clamp((int)(pixelRGB.r * 1.1), 0, 255)),
                                                 clamp((int)((pixelRGB.g / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.g / 3, clamp((int)(pixelRGB.g * 1.1), 0, 255)),
                                                 clamp((int)((pixelRGB.b / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.b / 3, clamp((int)(pixelRGB.b * 1.1), 0, 255)));
                    }
                } else {
                    if(   (isHorisontal && shouldTextureBeMirrored)  ||
                         (!isHorisontal && shouldTextureBeMirrored) ) {
                          pixelColor = ColorToUint(clamp((int)((wallColor.r * 0.8f) * (distanceToAWall * 16) / 32), (int)wallColor.r, 255),
                                                   clamp((int)((wallColor.g * 0.8f) * (distanceToAWall * 16) / 32), (int)wallColor.g, 255),
                                                   clamp((int)((wallColor.b * 0.8f) * (distanceToAWall * 16) / 32), (int)wallColor.b, 255));
                    } else {
                        pixelColor = ColorToUint(clamp((int)(wallColor.r * (distanceToAWall * 16) / 32), (int)wallColor.r, 255),
                                                 clamp((int)(wallColor.g * (distanceToAWall * 16) / 32), (int)wallColor.g, 255),
                                                 clamp((int)(wallColor.b * (distanceToAWall * 16) / 32), (int)wallColor.b, 255));
                    }
                }
                if(isLightMap) {
                        lightmapPixel = getTransposedTexturePixel(lightmap, (int)((i - ceilingHeight) * ((float)lightmap->w / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(scalingVar) * (float)lightmap->h));
                        SDL_Color texColor = UintToColor(pixelColor);
                        SDL_Color lightColor = UintToColor(*lightmapPixel);
                        SDL_Color finalColor = transformColorByLightMap(texColor, lightColor);

                        pixelColor = ColorToUint(finalColor.r, finalColor.g, finalColor.b);
                    }
                if(isTextured && ((isHorisontal && shouldTextureBeMirrored)
                              || (!isHorisontal && shouldTextureBeMirrored)) ) {
                    // Doing fast pixel transformation for the texture, so the performance won't suffer
                    pixelColor = (pixelColor >> 1) & 0x7F7F7F;
                }
                if(night) pixelColor = (pixelColor >> 1) & 0x7F7F7F;
                *pixel = pixelColor;
            }
            else
            {
                Uint32 pixelColor;
                if(!isFloorASky) {
                    pixelColor = ColorToUint(clamp((int)(0  * (float)(screenHeight - i + horizonLine / 2 + 192) / 256), 0, 200),
                                             clamp((int)(90 * (float)(screenHeight - i + horizonLine / 2 + 192) / 256), 0, 200),
                                             clamp((int)(30 * (float)(screenHeight - i + horizonLine / 2 + 192) / 256), 0, 200));
                    if(night) pixelColor = (pixelColor >> 1) & 0x7F7F7F;
                } else {
                    if(shouldStarsBeRendered && stars[(i + (horizonCap - horizonLine)) * starsWidth + skyWidthIndex]) {
                        pixelColor = ColorToUint(clamp(rand() % 256, 165, 255),
                                                 clamp(rand() % 256, 165, 255),
                                                 clamp(rand() % 256, 165, 255));
                    } else {
                        pixelColor = ColorToUint(clamp((int)(skyColor.r * (float)(i - horizonLine + 256) / 256), 0, 255),
                                                 clamp((int)(skyColor.g * (float)(i - horizonLine + 256) / 256), 0, 255),
                                                 clamp((int)(skyColor.b * (float)(i - horizonLine + 256) / 256), 0, 255));
                    }
                }
                *pixel = (Uint32)pixelColor;
            }
        }
}

int main(int argc, char** argv)
{
    defSkyColor.r = 0;
    defSkyColor.g = 10 * 2;
    defSkyColor.b = 50 * 2;

    skyColor = defSkyColor;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit(SDL_Quit);

    setWindowPos(8, 30);

    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        return 1;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    loadTextures(textures);
    loadTextures(m_textures);

    loadLightmaps(lightmaps);
    loadLightmaps(m_lightmaps);

    mirrorTextures(m_textures);
    mirrorTextures(m_lightmaps);

    doLightMapsToAllTextures(textures, lightmaps);
    doLightMapsToAllTextures(m_textures, m_lightmaps);
    setLightMapsTo0();

    transposeTextures(textures);
    transposeTextures(lightmaps);
    transposeTextures(m_textures);
    transposeTextures(m_lightmaps);
    fillUpTheStars();

    if(textures.empty() || lightmaps.empty())
    {
        printf("\nCould not load textures.\n");
        return 1;
    }

    char fps[80];

    int frameTime = 20;
    SDL_ShowCursor(SDL_DISABLE);
    int count = 0;

    while (!done)
    {
        int start = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            checkControls(event, &screen);
        }

        doActions(frameTime);

        for(int j = 0; j < screenWidth; ++j)
        {
            renderColumn(j, screen);
        }

        int end = SDL_GetTicks();
        frameTime = end - start;

        if(frameTime < (1000 / targetFPS))
        {
            SDL_Delay((1000 / targetFPS) - frameTime);
        }

        end = SDL_GetTicks();
        frameTime = end - start;

        if(count == 16)
        {
            sprintf(fps, "%d", 1000 / clamp(end - start, 1, INT_MAX));
            SDL_WM_SetCaption(fps, NULL);
            count = 0;
        }
        SDL_Flip(screen);
        ++count;
    }

    freeTextures();
    SDL_free(screen);
    free(stars);
    return 0;
}
