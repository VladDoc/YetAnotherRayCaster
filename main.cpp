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
}

float getDistanceToTheNearestIntersection(Vector2D<float> currentPosition)
{
    return blockBitSize; // TODO: Implement DDA algorithm
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
            distanceToAWall += getDistanceToTheNearestIntersection(test);


            test.x = player.x + eye.x * distanceToAWall;
            test.y = player.y + eye.y * distanceToAWall;

            if(test.x < 0 || test.x >= mapWidth || test.y < 0 || test.y >= mapHeight)
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

        distanceToAWall *= cosf(ray - player.angle - (FOV / (screenWidth * 8))); // You might ask: 'why 8?'. But it seems to give best results yet.

        int ceilingHeight = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToAWall);// + abs(j  - screenWidth / 2);
        int floorHeight = screenHeight - ceilingHeight;
        ceilingHeight += horizonLine;
        floorHeight += horizonLine;

        float bufferRay = clampLooping(ray, 0.0f, pi * 2);
        int skyWidthIndex = (int)(screenWidth * (bufferRay / FOV));

        for(int i = 0; i < screenHeight; ++i)
        {
            if(i < ceilingHeight)
            {
                Uint32 pixelColor;
                if(shouldStarsBeRendered && stars[i + (horizonCap - horizonLine)][skyWidthIndex]) {
                    pixelColor = ColorToUint(clamp(rand() % 256, 165, 255),
                                             clamp(rand() % 256, 165, 255),
                                             clamp(rand() % 256, 165, 255));
                } else {
                    pixelColor = ColorToUint(clamp((int)(skyColor.r * (float)(i - horizonLine + 128) / 128), 0, 255),
                                             clamp((int)(skyColor.g * (float)(i - horizonLine + 128) / 128), 0, 255),
                                             clamp((int)(skyColor.b * (float)(i - horizonLine + 128) / 128), 0, 255));
                }
                Uint32* pixel = getTexturePixel(screen, i, j);
                *pixel = pixelColor;
            }
            else if(i >= ceilingHeight && i < floorHeight)
            {
                int wallSizeOnScreen = floorHeight - ceilingHeight;
                Uint32* pixel = getTexturePixel(screen, i, j);
                Uint32 pixelColor;
                MapBlock currentBlock = map[(int)test.y][(int)test.x];

                    if(currentBlock.getIsTextured()) {
                        bool isHorisontal = false;
                        bool isLightMap = currentBlock.getIsLightMapped();
                        float checkY = test.y;
                        SDL_Surface* texture = textures.at(currentBlock.getTextureIndex());
                        SDL_Surface* lightmap = NULL;
                        if(isLightMap) {
                            lightmap = lightmaps.at(currentBlock.getLightMapIndex());
                        }

                        Uint32* texturePixel = &defWallColor;
                        Uint32* lightmapPixel;

                        if(map[(int)(checkY - blockBitSize)][(int)test.x].isEmpty() ||
                           map[(int)(checkY + blockBitSize)][(int)test.x].isEmpty()   ) {
                            isHorisontal = true;
                        }
                    if(isHorisontal) {
                            texturePixel = getTexturePixel(texture, (int)((i - ceilingHeight) * ((float)texture->h / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(test.x) * (float)texture->w));

                        if(isLightMap) {
                            lightmapPixel = getTexturePixel(lightmap, (int)((i - ceilingHeight) * ((float)lightmap->h / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(test.x) * (float)lightmap->w));
                        }
                    } else {
                            texturePixel = getTexturePixel(texture, (int)((i - ceilingHeight) * ((float)texture->h / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(test.y) * (float)texture->w));

                        if(isLightMap) {
                            lightmapPixel = getTexturePixel(lightmap, (int)((i - ceilingHeight) * ((float)lightmap->h / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(test.y) * (float)lightmap->w));
                        }
                    }
                    if(isLightMap) {
                        SDL_Color texColor = UintToColor(*texturePixel);
                        SDL_Color lightColor = UintToColor(*lightmapPixel);
                        SDL_Color finalColor = transformColorByLightMap(texColor, lightColor);

                        pixelColor = ColorToUint(finalColor.r, finalColor.g, finalColor.b);
                    } else {
                        pixelColor = *texturePixel;
                    }
                    #ifdef TEXTURE_GRADIENT
                        SDL_Color pixelRGB = UintToColor(pixelColor);

                        pixelColor = ColorToUint(clamp((int)((pixelRGB.r / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.r / 3, clamp((int)(pixelRGB.r * 1.2), 0, 255)),
                                                 clamp((int)((pixelRGB.g / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.g / 3, clamp((int)(pixelRGB.g * 1.2), 0, 255)),
                                                 clamp((int)((pixelRGB.b / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.b / 3, clamp((int)(pixelRGB.b * 1.2), 0, 255)));
                    #endif // TEXTURE_GRADIENT
                } else {
                    pixelColor = ColorToUint(clamp((int)(wallColor.r * (distanceToAWall * 16) / 32), (int)wallColor.r, 255),
                                             clamp((int)(wallColor.g * (distanceToAWall * 16) / 32), (int)wallColor.g, 255),
                                             clamp((int)(wallColor.b * (distanceToAWall * 16) / 32), (int)wallColor.b, 255));
                }
                *pixel = pixelColor;
            }
            else
            {
                Uint32 pixelColor;
                if(!isFloorASky) {
                    pixelColor = ColorToUint(clamp((int)(0  * (float)(screenHeight - i + horizonLine / 2 + 128) / 128), 0, 200),
                                             clamp((int)(50 * (float)(screenHeight - i + horizonLine / 2 + 128) / 128), 0, 200),
                                             clamp((int)(20 * (float)(screenHeight - i + horizonLine / 2 + 128) / 128), 0, 200));
                } else {
                    if(shouldStarsBeRendered && stars[i + (horizonCap - horizonLine)][skyWidthIndex]) {
                        pixelColor = ColorToUint(clamp(rand() % 256, 165, 255),
                                                 clamp(rand() % 256, 165, 255),
                                                 clamp(rand() % 256, 165, 255));
                    } else {
                        pixelColor = ColorToUint(clamp((int)(skyColor.r * (float)(i - horizonLine + 128) / 128), 0, 255),
                                                 clamp((int)(skyColor.g * (float)(i - horizonLine + 128) / 128), 0, 255),
                                                 clamp((int)(skyColor.b * (float)(i - horizonLine + 128) / 128), 0, 255));
                    }
                }
                Uint32* pixel = getTexturePixel(screen, i, j);
                *pixel = (Uint32)pixelColor;
            }
        }
}

int main(int argc, char** argv)
{

    defSkyColor.r = 0;
    defSkyColor.g = 10;
    defSkyColor.b = 50;

    skyColor = defSkyColor;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit(SDL_Quit);

    char env[80];
    sprintf(env, "SDL_VIDEO_WINDOW_POS=%d,%d", (SDL_GetVideoInfo()->current_w - screenWidth) / 2, 30);

    SDL_putenv(env);
    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        return 1;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    loadTextures();
    loadLightmaps();

    if(textures.empty() || lightmaps.empty())
    {
        printf("\nCould not load textures.\n");
        system("PAUSE");
        exit(-1);
    }

    char fps[80];
    int frameTime = 20;
    fillUpTheStars();
    SDL_ShowCursor(SDL_DISABLE);
    int count = 0;

    while (!done)
    {
        int start = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            checkControls(event, screen);
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
    return 0;
}
