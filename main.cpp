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


void freeTextures(GameData& gamedata) {
    for(auto i = gamedata.textures.begin(); i != gamedata.textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = gamedata.lightmaps.begin(); i != gamedata.lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }

    for(auto i = gamedata.m_textures.begin(); i != gamedata.m_textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = gamedata.m_lightmaps.begin(); i != gamedata.m_lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }
}

void renderColumn(float ray, const int j, SDL_Surface* screen,
                  Vector2D<float>& test, float distanceToAWall, GameData& gamedata, ControlState ctrls = controls)
{
    using namespace Constants;

    SDL_Color wallColor;
    if(withinRange((float)test.x, 0.0f, (float)mapWidth) &&
       withinRange((float)test.y, 0.0f, (float)mapHeight)) {
            wallColor = gamedata.map[(int)test.y][(int)test.x].getColor();
    } else {
        wallColor = MapBlock::defWallColor;
    }

    RenderData r_data;

    r_data.floorColor = Constants::floorColor;
    r_data.skyColor = Constants::skyColor;

    if(!ctrls.easterEgg) {
        r_data.ceilingHeight = (float)(screenHeight / 2.0) -
                                screenHeight / ((float)distanceToAWall);
    } else {
        r_data.ceilingHeight = (float)(screenHeight / 2.0) -
                                screenHeight / ((float)distanceToAWall) +
                                abs(j  - screenWidth / 2);
    }

    r_data.floorHeight = screenHeight - r_data.ceilingHeight;

    // Increases height of a wall
    r_data.ceilingHeight -= (r_data.floorHeight - r_data.ceilingHeight) *
                            gamedata.map[(int)test.y][(int)test.x].height -
                            (r_data.floorHeight - r_data.ceilingHeight);

    r_data.ceilingHeight += gamedata.horizonLine;
    r_data.floorHeight += gamedata.horizonLine;

    r_data.wallSizeOnScreen = r_data.floorHeight - r_data.ceilingHeight;

    float bufferRay = clampLooping(ray, 0.0f, pi * 2);
    r_data.skyTextureIndex = clamp((int)(starsWidth * (bufferRay / (pi * 2))), 0, starsWidth-1);
    r_data.skyWidthIndex = (int)(screenWidth * (bufferRay / FOV));

    r_data.shouldTextureBeMirrored = false;
    r_data.isHorisontal = false;

    float checkY = test.y;

    if(gamedata.map[(int)(checkY - horisontalBlockCheckStep)][(int)test.x].isEmpty() ||
       gamedata.map[(int)(checkY + horisontalBlockCheckStep)][(int)test.x].isEmpty()   ) {
            r_data.isHorisontal = true;
            r_data.scalingVar = test.x; // Then wall is along horizontal axis
            if(getFractialPart(test.y) > 0.5f) { // If y component greater than a half then it's a north wall
                r_data.shouldTextureBeMirrored = true;
            }
    } else {
            r_data.scalingVar = test.y;
            if(getFractialPart(test.x) < 0.5f) {
                r_data.shouldTextureBeMirrored = true;
            }
    }

    MapBlock currentBlock = gamedata.map[(int)test.y][(int)test.x];

    r_data.texture = NULL;
    r_data.lightmap = NULL;

    r_data.isTextured = currentBlock.getIsTextured();
    r_data.isLightMap = currentBlock.getIsLightMapped();



    if(!r_data.shouldTextureBeMirrored) {
        // Sometimes if works with false boolean, which causes game to segfault, to prevent that I clamp index.
        if(r_data.isTextured) r_data.texture =
            gamedata.textures[clamp(currentBlock.getTextureIndex(), 0, (int)gamedata.textures.size()-1)];
        if(r_data.isLightMap) r_data.lightmap =
            gamedata.lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)gamedata.lightmaps.size()-1)];
    } else {
        if(r_data.isTextured) r_data.texture =
            gamedata.m_textures[clamp(currentBlock.getTextureIndex(), 0, (int)gamedata.m_textures.size()-1)];
        if(r_data.isLightMap) r_data.lightmap =
            gamedata.m_lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)gamedata.m_lightmaps.size()-1)];
    }

    // Non textured wall routine
    if(whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::WEST  ||
       whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::NORTH    )
    {
        r_data.wallColorPixel = getShadowedWallColor(wallColor, distanceToAWall);
    } else {
        r_data.wallColorPixel = getGradientedWallColor(wallColor, distanceToAWall);
    }


    if(ctrls.texturedSky) {
        r_data.skyLightColor = *getTransposedTexturePixel(gamedata.sky_textures[0], 1907, 604);
    } else {
        r_data.skyLightColor = ColorToUint(skyColor.r, skyColor.g, skyColor.b);
    }

    if(ctrls.night) {
        r_data.fogColor = nightFogColor;
    } else {
        r_data.fogColor = dayFogColor;
    }


    for(int i = 0; i < screenHeight; ++i)
    {
        Uint32* pixel = getTexturePixel(screen, i, j);
        Uint32 pixelColor;
        if(i < r_data.ceilingHeight)
        {
            pixelColor = renderCeiling(ctrls, gamedata, r_data, i);
        }
        else if(i >= r_data.ceilingHeight && i < r_data.floorHeight)
        {
            pixelColor = renderWall(ctrls, gamedata, r_data, i);
        }
        else
        {
            pixelColor = renderFloor(ctrls, gamedata, r_data, i);
        }
        *pixel = pixelColor;
    }
}

void calculateDistances(float* inRays, int from, int to, GameData* gamedata)
{
    for(int j = from; j < to; ++j)
    {
        rayTraversal(*gamedata, inRays[j], gamedata->distances, gamedata->rayPositions, j);
    }
}

void renderColumns(SDL_Surface* screen, float* inRays, int from, int to, int threadId, GameData* gamedata)
{
    random.seed(threadId);
    for(int j = from; j < to; ++j)
    {
        renderColumn(inRays[j], j, screen, gamedata->rayPositions[j], gamedata->distances[j], *gamedata);
    }
}


int main(int argc, char** argv)
{
    using namespace Constants;
    data.logFile = fopen("log.txt", "w");
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
    loadTextures(data.textures);
    loadTextures(data.m_textures);

    loadLightmaps(data.lightmaps);
    loadLightmaps(data.m_lightmaps);

    loadSkyTextures(data.sky_textures);

    mirrorTextures(data.m_textures);
    mirrorTextures(data.m_lightmaps);

    doLightMapsToAllTextures(data.textures, data.lightmaps, data);
    doLightMapsToAllTextures(data.m_textures, data.m_lightmaps, data);
    setLightMapsTo0(data);

    transposeTextures(data.sky_textures);
    transposeTextures(data.textures);
    transposeTextures(data.lightmaps);
    transposeTextures(data.m_textures);
    transposeTextures(data.m_lightmaps);

    if(data.textures.empty() || data.lightmaps.empty())
    {
        printf("\nCould not load textures.\n");
        return 1;
    }

    char fps[80];

    int frameTime = 20;
    SDL_ShowCursor(SDL_DISABLE);
    int count = 0;

    data.allocateScreenSizeSensitiveData();

    while (!data.done)
    {

        int start = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            checkControls(event, &screen, data);
        }

        doActions(frameTime, data);

        data.player.angle = clampLooping(data.player.angle, 0.0f, pi * 2);

        for(int j = 0; j < screenWidth; ++j)
        {
           data.rays[j] = (data.player.angle - FOV / 2.0f) +
                          ((float)j / (float)Constants::screenWidth) * FOV;
        }


        size_t numThreads;
        if(controls.multithreaded) numThreads = std::thread::hardware_concurrency();
        else numThreads = 1;

        std::thread** distanceThreads = new std::thread*[numThreads];

        for(size_t i = 0; i < numThreads; ++i) {
            distanceThreads[i] =
                new std::thread(calculateDistances, data.rays,
                            i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads), &data);
        }

        for(size_t i = 0; i < numThreads; ++i) {
            distanceThreads[i]->join();
            delete distanceThreads[i];
        }

        delete[] distanceThreads;
        std::thread** renderThreads = new std::thread*[numThreads];

        for(size_t i = 0; i < numThreads; ++i) {
            renderThreads[i] =
                    new std::thread(renderColumns, screen, data.rays,
                            i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads), rand(), &data);
        }

        for(size_t i = 0; i < numThreads; ++i) {
            renderThreads[i]->join();
            delete renderThreads[i];
        }

        delete[] renderThreads;
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

    freeTextures(data);
    SDL_FreeSurface(screen);

    data.freeScreenSizeSensitiveData();
    fclose(data.logFile);
    return 0;
}
