#include <cstdlib>

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <thread>

#include <SDL/SDL.h>

#include "MapBlock.h"
#include "ControlsState.h"
#include "Utility.h"
#include "GameData.h"
#include "Input.h"
#include "player.h"
#include "Vector2D.h"
#include "rendering.h"
#include "Sprite.h"
#include "Rectangle.h"


void freeTextures() {
    for(auto i = textures.begin(); i != textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = lightmaps.begin(); i != lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }

    for(auto i = m_textures.begin(); i != m_textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = m_lightmaps.begin(); i != m_lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }
}

void renderColumn(float ray, const int j, SDL_Surface* screen, Vector2D<float>& test, float distanceToAWall)
{


        SDL_Color wallColor;
        if(withinRange((float)test.x, 0.0f, (float)mapWidth) &&
           withinRange((float)test.y, 0.0f, (float)mapHeight)) {
                wallColor = map[(int)test.y][(int)test.x].getColor();
        } else {
            wallColor = MapBlock::defWallColor;
        }


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
        int skyTextureIndex = clamp((int)(starsWidth * (bufferRay / (pi * 2))), 0, starsWidth-1);
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
            if(isTextured) texture = textures[clamp(currentBlock.getTextureIndex(), 0, (int)textures.size()-1)];
            if(isLightMap) lightmap = lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)lightmaps.size()-1)];
        } else {
            if(isTextured) texture = m_textures[clamp(currentBlock.getTextureIndex(), 0, (int)textures.size()-1)];
            if(isLightMap) lightmap = m_lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)lightmaps.size()-1)];
        }

        Uint32 skyLightColor;
        Uint32 fogColor;

        Uint32 wallColorPixel;

        // Non textured wall routine
        if(whichSide(shouldTextureBeMirrored, isHorisontal) == SideOfAWall::WEST  ||
           whichSide(shouldTextureBeMirrored, isHorisontal) == SideOfAWall::NORTH    )
        {
            wallColorPixel = getShadowedWallColor(wallColor, distanceToAWall);
        } else {
            wallColorPixel = getGradientedWallColor(wallColor, distanceToAWall);
        }


        if(texturedSky) {
            skyLightColor = *getTransposedTexturePixel(sky_textures[0], 1907, 604);
        } else {
            skyLightColor = ColorToUint(skyColor.r, skyColor.g, skyColor.b);
        }

        if(night) {
            fogColor = nightFogColor;
        } else {
            fogColor = dayFogColor;
        }

        for(int i = 0; i < screenHeight; ++i)
        {
            Uint32* pixel = getTexturePixel(screen, i, j);
            Uint32 pixelColor;
            if(i < ceilingHeight)
            {
                // If star exists in the stars map for the current location set white flickering pixel
                if(shouldStarsBeRendered && stars[(i + (horizonCap - horizonLine)) * starsWidth + skyWidthIndex]) {
                    pixelColor = getStarColorPixel();
                } else {
                    // Do gradiented color sky or retrieve pixel out of sky box
                    if(!texturedSky) {
                        pixelColor = getSkyGradientedColor(skyColor, i, horizonLine);
                    } else {
                            pixelColor = *getTransposedScaledTexturePixel(sky_textures[0], starsWidth, starsHeight,
                                                            i + (horizonCap - horizonLine), skyTextureIndex);
                    }
                    if(fog) {
                        pixelColor = blend(pixelColor, fogColor,
                                     clamp((i + (horizonCap - horizonLine))  /  2 / (starsHeight / 2 / 256), 0, 255));
                    }
                }
            }
            else if(i >= ceilingHeight && i < floorHeight)
            {
                int wallSizeOnScreen = floorHeight - ceilingHeight;
                Uint32* texturePixel = NULL;
                Uint32* lightmapPixel = NULL;

                // Textured wall routine
                if(isTextured) {
                        // Swaped w and h because the texture is transposed. Same with lightmaps.
                        texturePixel = getTransposedTexturePixel(texture, (int)((i - ceilingHeight) * ((float)texture->w / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(scalingVar) * (float)texture->h));
                        pixelColor = *texturePixel;

                    if(textureGradient) {
                        pixelColor = applyWallGradientToPixel(pixelColor, distanceToAWall);
                    }

                    // Darkens walls for an illusion of directed light
                    if(whichSide(shouldTextureBeMirrored, isHorisontal) == SideOfAWall::WEST   ||
                       whichSide(shouldTextureBeMirrored, isHorisontal) == SideOfAWall::NORTH    ) {
                        // Doing fast pixel transformation for the texture, so the performance won't suffer
                            pixelColor = fastPixelShadowing(pixelColor);
                    }

                } else {
                    pixelColor = wallColorPixel;
                }
                if(isLightMap) {
                        // If current wall is lightmapped(on runtime) then transform pixel accordingly.
                        // Very slow so it's never used.
                        lightmapPixel = getTransposedTexturePixel(lightmap, (int)((i - ceilingHeight) * ((float)lightmap->w / (float)wallSizeOnScreen)),
                                                        (int)(getFractialPart(scalingVar) * (float)lightmap->h));
                        SDL_Color texColor = UintToColor(pixelColor);
                        SDL_Color lightColor = UintToColor(*lightmapPixel);
                        SDL_Color finalColor = transformColorByLightMap(texColor, lightColor);

                        pixelColor = ColorToUint(finalColor.r, finalColor.g, finalColor.b);
                    }

                 // If night mode enabled darken pixel even more
                if(night) {
                        pixelColor = fastPixelShadowing(pixelColor);
                }

                if(fog) {
                    pixelColor = blend(pixelColor, fogColor, clamp((Uint8)(distanceToAWall * 12), (Uint8)0, (Uint8)255));
                }

                if(coloredLight) pixelColor = blend(pixelColor, skyLightColor, 92);
            }
            else
            {
                if(!isFloorASky) {
                    pixelColor = getFloorGradientedColor(floorColor, i, horizonLine);
                    if(night) pixelColor = fastPixelShadowing(pixelColor);
                    if(fog) {
                        pixelColor = blend(pixelColor, fogColor,
                               clamp((starsHeight - (i + (horizonCap - horizonLine)))  / 2 / (starsHeight / 2 / 256), 0, 255));
                    }
                    if(coloredLight) pixelColor = blend(pixelColor, skyLightColor, 92);
                } else {
                    if(shouldStarsBeRendered && stars[(i + (horizonCap - horizonLine)) * starsWidth + skyWidthIndex]) {
                        pixelColor = getStarColorPixel();
                    } else {
                        if(!texturedSky) {
                            pixelColor = getSkyGradientedColor(skyColor, i, horizonLine);
                        } else {
                            pixelColor = *getTransposedScaledTexturePixel(sky_textures[0], starsWidth, starsHeight,
                                                            i + (horizonCap - horizonLine), skyTextureIndex);
                        }
                        if(fog) {
                            pixelColor = blend(pixelColor, fogColor,
                                        clamp((starsHeight - (i + (horizonCap - horizonLine)))  / 2 / (starsHeight / 2 / 256), 0, 255));
                        }
                    }
                }
            }
            *pixel = pixelColor;
        }
}

void calculateDistances(float* inRays, int from, int to)
{
    for(int j = from; j < to; ++j)
    {
        rayTraversal(inRays[j], distances, rayPositions, j);
    }
}

void renderColumns(SDL_Surface* screen, float* inRays, int from, int to, int threadId)
{
    random.seed(threadId);
    for(int j = from; j < to; ++j)
    {
        renderColumn(inRays[j], j, screen, rayPositions[j], distances[j]);
    }
}


int main(int argc, char** argv)
{
    logFile = fopen("log.txt", "w");
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

    loadSkyTextures(sky_textures);

    mirrorTextures(m_textures);
    mirrorTextures(m_lightmaps);

    doLightMapsToAllTextures(textures, lightmaps);
    doLightMapsToAllTextures(m_textures, m_lightmaps);
    setLightMapsTo0();

    transposeTextures(sky_textures);
    transposeTextures(textures);
    transposeTextures(lightmaps);
    transposeTextures(m_textures);
    transposeTextures(m_lightmaps);

    if(textures.empty() || lightmaps.empty())
    {
        printf("\nCould not load textures.\n");
        return 1;
    }

    char fps[80];

    int frameTime = 20;
    SDL_ShowCursor(SDL_DISABLE);
    int count = 0;

    allocateScreenSizeSensitiveData();

    while (!done)
    {

        int start = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            checkControls(event, &screen);
        }

        doActions(frameTime);

        player.angle = clampLooping(player.angle, 0.0f, pi * 2);

        for(int j = 0; j < screenWidth; ++j)
        {
           rays[j] = (player.angle - FOV / 2.0f) + ((float)j / (float)screenWidth) * FOV;
        }


        size_t numThreads;
        if(multithreaded) numThreads = std::thread::hardware_concurrency();
        else numThreads = 1;

        std::thread** distanceThreads = new std::thread*[numThreads];

        for(size_t i = 0; i < numThreads; ++i) {
            distanceThreads[i] =
                new std::thread(calculateDistances, rays,
                            i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads));
        }

        for(size_t i = 0; i < numThreads; ++i) {
            distanceThreads[i]->join();
            delete distanceThreads[i];
        }

        std::thread** renderThreads = new std::thread*[numThreads];

        for(size_t i = 0; i < numThreads; ++i) {
            renderThreads[i] =
                    new std::thread(renderColumns, screen, rays,
                            i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads), rand());
        }

        for(size_t i = 0; i < numThreads; ++i) {
            renderThreads[i]->join();
            delete renderThreads[i];
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
    SDL_FreeSurface(screen);

    freeScreenSizeSensitiveData();
    fclose(logFile);
    return 0;
}
