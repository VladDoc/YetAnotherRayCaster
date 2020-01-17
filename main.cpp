#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <thread>

#include <SDL/SDL.h>

#include "a_star.h"
#include "LuaScript.h"
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
                  Vector2D<float>& test, float distanceToAWall,
                  const GameData& gamedata, const ControlState& ctrls)
{
    using namespace Constants;

    SDL_Color wallColor;
    RenderData r_data;

    r_data.distanceToAWall = distanceToAWall;

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

    r_data.ceilingHeight += gamedata.horizonLine;
    r_data.floorHeight += gamedata.horizonLine;

    r_data.wallSizeOnScreen = r_data.floorHeight - r_data.ceilingHeight;

    float bufferRay = clampLooping(ray, 0.0f, pi * 2);
    r_data.skyTextureIndex = clamp((int)(starsWidth * (bufferRay / (pi * 2))), 0, starsWidth-1);
    r_data.skyWidthIndex = (int)(screenWidth * (bufferRay / FOV));

    if(ctrls.texturedSky) {
        r_data.skyLightColor = *getTransposedTexturePixel(gamedata.sky_textures[0], 1907, 604);
    } else {
        r_data.skyLightColor = ColorToUint(skyColor.r, skyColor.g, skyColor.b);
    }

    if(ctrls.night) {
        r_data.fogColor = blend(fastPixelShadowing(r_data.skyLightColor), nightFogColor, 127);
    } else {
        r_data.fogColor = dayFogColor;
    }

    if(!withinRange(test.x, 0.0f, (float)mapWidth) ||
       !withinRange(test.y, 0.0f, (float)mapHeight)) {
        wallColor = MapBlock::defWallColor;
        distanceToAWall = offMapDepth;
    } else {
        wallColor = gamedata.map[(int)test.y][(int)test.x].getColor();
        r_data.ceilingHeight -= (r_data.floorHeight - r_data.ceilingHeight) *
                                gamedata.map[(int)test.y][(int)test.x].height -
                                (r_data.floorHeight - r_data.ceilingHeight);


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
    }

    // Non textured wall routine
        if(whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::WEST  ||
           whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::NORTH    )
        {
            r_data.wallColorPixel = getShadowedWallColor(wallColor, distanceToAWall);
        } else {
            r_data.wallColorPixel = getGradientedWallColor(wallColor, distanceToAWall);
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

void calculateDistances(float* inRays, int from, int to, GameData* gamedata, ControlState* ctrls)
{
    for(int j = from; j < to; ++j)
    {
        rayTraversal(*gamedata, inRays[j], gamedata->distances, gamedata->rayPositions, *ctrls, j);
    }
}

void renderColumns(SDL_Surface* screen, float* inRays, int from, int to, int threadId,
                   const GameData* gamedata, const ControlState* ctrls)
{
    random.seed(threadId);
    for(int j = from; j < to; ++j)
    {
        renderColumn(inRays[j], j, screen, gamedata->rayPositions[j],
                      gamedata->distances[j], *gamedata, *ctrls);
    }
}


std::vector<Vector2D<int>> findPath(Vector2D<int>& src, Vector2D<int>& dest, GameData& data)
{
    using namespace a_star;
    std::vector<std::vector<int>> grid(Constants::mapHeight,
                                    std::vector<int>(Constants::mapWidth, 0));
    for(int i = 0; i < Constants::mapHeight; ++i) {
        for(int j = 0; j < Constants::mapWidth; ++j) {
            grid[i][j] = data.map[i][j].isEmpty();
        }
    }


    std::stack<Pair> in_path = aStarSearch(grid, std::make_pair(src.y, src.x),
                                            std::make_pair(dest.y, dest.x));
    std::vector<Vector2D<int>> path;

    data.map[data.destination.y][data.destination.x] = {100, 0, 0};

    size_t in_path_size = in_path.size();

    for(size_t i = 0; i < in_path_size; ++i) {
        Pair p = in_path.top();
        path.push_back({p.second, p.first});
        in_path.pop();
    }

    if(path.size() != in_path_size) {
        std::swap(data.textures[0], data.textures[1]);
    }
    return path;
}

void readConfig(GameData& data, ControlState& ctrls)
{
    using namespace Constants;
    LuaScript script("conf.lua");
    if(!script) return; // Then defaults will be loaded.
    std::string filename = script.get<std::string>("config.map");


    Vector2D<bool> state{false, false};
    if(filename.size() > 0) {
        state = data.initMapFromFile(filename.c_str());
    }
    data.player.angle = script.get<float>("config.player.angle");

    // Then map doesn't have a starting point.
    if(!state.x) {
        data.player.x = script.get<float>("config.player.X") - 1;
        data.player.y = script.get<float>("config.player.Y") - 1;
    }

    screenWidth = script.get<int>("config.screenWidth");
    screenHeight = script.get<int>("config.screenHeight");
    screenBits = script.get<int>("config.screenBits");

    if(state.x && state.y) {
        ctrls.findpath = script.get<bool>("config.findPath");
        data.millisAtCell = (int)(script.get<float>("config.time_at_each_cell") * 1000);
    }

    defWalkingSpeed = script.get<float>("config.walkingSpeed");
    rotatingSpeed = script.get<float>("config.rotatingSpeed");
    targetSpeed = script.get<float>("config.targetSpeed");

    targetFPS = script.get<int>("config.targetFPS");

    mouseSensitivity = script.get<float>("config.mouseSensitivity");
    skyColor = SDL_Color{
        script.get<int>("config.skyColor.R"),
        script.get<int>("config.skyColor.G"),
        script.get<int>("config.skyColor.B"),
        255
    };

    floorColor = SDL_Color{
        script.get<int>("config.floorColor.R"),
        script.get<int>("config.floorColor.G"),
        script.get<int>("config.floorColor.B"),
        255
    };

    dayFogColor = ColorToUint(
        script.get<int>("config.dayFogColor.R"),
        script.get<int>("config.dayFogColor.G"),
        script.get<int>("config.dayFogColor.B")
    );

    nightFogColor = ColorToUint(
        script.get<int>("config.nightFogColor.R"),
        script.get<int>("config.nightFogColor.G"),
        script.get<int>("config.nightFogColor.B")
    );

    ctrls.multithreaded = script.get<bool>("config.multithreaded");
    ctrls.fog = script.get<bool>("config.fog");
    ctrls.coloredLight = script.get<bool>("config.coloredLight");
    ctrls.naiveApproach = script.get<bool>("config.naiveApproach");
    ctrls.isFullScreen = script.get<bool>("config.isFullScreen");
    ctrls.vSync = script.get<bool>("config.vSync");
    ctrls.night = script.get<bool>("config.night");
    ctrls.texturedSky = script.get<bool>("config.texturedSky");
    ctrls.shouldStarsBeRendered = script.get<bool>("config.shouldStarsBeRendered");
    ctrls.isFloorASky = script.get<bool>("config.isFloorASky");
    ctrls.textureGradient = script.get<bool>("config.textureGradient");
}

int main(int argc, char** argv)
{
    using namespace Constants;
    GameData data;
    ControlState controls;

    std::ofstream debug("debug.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(debug.rdbuf());

    readConfig(data, controls);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit(SDL_Quit);

    setWindowPos(8, 30);


    Uint32 fullscreen = controls.isFullScreen ? SDL_FULLSCREEN : 0x0;

    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF | fullscreen);

    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n",
               screenWidth, screenHeight, screenBits, SDL_GetError());
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

    std::vector<Vector2D<int>> path;
    if(controls.findpath) {
        Vector2D<int> src{data.player.x, data.player.y};
        path = findPath(src, data.destination, data);
    }

    bool finishedPath = false;

    while (!data.done)
    {
        if(controls.findpath && !finishedPath) {
            finishedPath = data.tracePath(path, frameTime);
        }
        int start = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            checkControls(event, &screen, data, controls);
        }

        doActions(frameTime, data, controls);

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
                            i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads),
                                 &data, &controls);
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
                    i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads),
                                    rand(), &data, &controls);
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

    debug.close();
    std::cout.rdbuf(coutbuf);

    std::ofstream out("out.txt");

    print2dVector(data.map, out);

    out << std::endl;

    out.close();

    freeTextures(data);
    SDL_FreeSurface(screen);

    data.freeScreenSizeSensitiveData();
    return 0;
}
