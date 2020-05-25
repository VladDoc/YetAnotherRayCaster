#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <thread>
#include <future>

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
#include "ThreadPool.h"

void calcRays(std::vector<float>* inRays, int from, int to, const GameData* data)
{
    using namespace Constants;
    for(size_t j = from; j < to; ++j)
    {
           (*inRays)[j] = clampLooping((data->player.angle - FOV / 2.0f) +
                                      ((float)j / (float)screenWidth) * FOV,
                                       0.0f, 2 * pi);
    }
}

void calculateDistances(std::vector<float>* inRays, int from, int to, GameData* gamedata, ControlState* ctrls)
{
    for(int j = from; j < to; ++j)
    {
        rayTraversal(*gamedata, (*inRays)[j], &gamedata->distances[0],
                      &gamedata->rayPositions[0], &gamedata->eyes[0], *ctrls, j);
    }
}

void renderColumns(SDL_Surface* screen, std::vector<float>* inRays, int from, int to, int threadId,
                   const GameData* gamedata, const ControlState* ctrls)
{
    random.seed(threadId);
    for(int j = from; j < to; j++)
    {
        renderColumn((*inRays)[j], j, screen, gamedata->rayPositions[j],
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

    char buf[80];
    int texSize = script.get<int>("config.texturesSize");
    for(int i = 1; i <= texSize;   ++i)
    {
        sprintf(buf, "config.textures.tex%d", i);
        data.texturePaths.push_back(script.get<std::string>(buf));
    }

    texSize = script.get<int>("config.lightmapsSize");
    for(int i = 1;  i <= texSize;   ++i)
    {
        sprintf(buf, "config.lightmaps.tex%d", i);
        data.lightmapPaths.push_back(script.get<std::string>(buf));
    }

    texSize = script.get<int>("config.skyTexturesSize");
    for(int i = 1;  i <= texSize;   ++i)
    {
        sprintf(buf, "config.skyTextures.tex%d", i);
        data.skyTexturePaths.push_back(script.get<std::string>(buf));
    }

    if(state.x && state.y) {
        ctrls.findpath = script.get<bool>("config.findPath");
        data.millisAtCell = (int)(script.get<float>("config.time_at_each_cell") * 1000);
    }

    screenWidth = script.get<int>("config.screenWidth");
    screenHeight = script.get<int>("config.screenHeight");
    screenBits = script.get<int>("config.screenBits");
    ctrls.isFullScreen = script.get<bool>("config.isFullScreen");

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

    std::ofstream debug(controls.debug ? "debug.txt" : "");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf

    if(controls.debug) std::cout.rdbuf(debug.rdbuf());

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }


    atexit(SDL_Quit);

    setWindowPos(8, 30);

    readConfig(data, controls);

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

    data.loadAllTextures();

    mirrorTextures(data.m_textures);
    mirrorTextures(data.m_lightmaps);

    doLightMapsToAllTextures(data.textures, data.lightmaps, data);
    doLightMapsToAllTextures(data.m_textures, data.m_lightmaps, data);
    setLightMapsTo0(data);

    //transposeTextures(data.sky_textures);
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

    ThreadPool pool(std::thread::hardware_concurrency());

    std::vector<std::future<void>> results;
    results.reserve(std::thread::hardware_concurrency());

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
        if(!controls.findpath || finishedPath) {
            doActions(frameTime, data, controls);
        }
        data.player.angle = clampLooping(data.player.angle, 0.0f, pi * 2);


        size_t numThreads;
        if(controls.multithreaded) numThreads = std::thread::hardware_concurrency();
        else numThreads = 1;


        for(size_t i = 0; i < numThreads; ++i) {
            results.emplace_back(pool.enqueue(calcRays, &data.rays,
                        i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads),
                            &data));
        }

        for(auto& a : results) a.wait();
        results.clear();

        for(size_t i = 0; i < numThreads; ++i) {
            results.emplace_back(pool.enqueue(calculateDistances, &data.rays,
                        i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads),
                            &data, &controls));
        }

        for(auto& a : results) a.wait();
        results.clear();


        for(size_t i = 0; i < numThreads; ++i) {
            results.emplace_back(pool.enqueue(renderColumns, screen, &data.rays,
                         i * (screenWidth / numThreads), (i + 1) * (screenWidth / numThreads),
                                    rand(), &data, &controls));
        }

        for(auto& a : results) a.wait();
        results.clear();

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

    SDL_FreeSurface(screen);

    return 0;
}
