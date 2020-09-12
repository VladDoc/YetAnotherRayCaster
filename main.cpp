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
#include "randomBS.h"
#include "RenderUtils.h"

void calcRays(std::vector<float>* inRays, int from, int to, const GameData* data)
{
    Constants& c = Constants::get();

    for(size_t j = from; j < to; ++j)
    {
           (*inRays)[j] = util::clampLooping((data->player.angle - c.FOV / 2.0f) +
                                            ((float)j / (float)c.screenWidth) * c.FOV,
                                            0.0f, 2 * c.pi);
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
    random_.seed(threadId);
    for(int j = from; j < to; j++)
    {
        renderColumn((*inRays)[j], j, screen, gamedata->rayPositions[j],
                      gamedata->distances[j], *gamedata, *ctrls);
    }
}


std::vector<Vector2D<int>> findPath(Vector2D<int>& src, Vector2D<int>& dest, GameData& data)
{
    using namespace a_star;
    std::vector<std::vector<int>> grid(Constants::get().mapHeight,
                                    std::vector<int>(Constants::get().mapWidth, 0));
    for(int i = 0; i < Constants::get().mapHeight; ++i) {
        for(int j = 0; j < Constants::get().mapWidth; ++j) {
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
    Constants& c = Constants::get();
    using namespace util;
    using namespace RenderUtils;

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

    c.screenWidth = script.get<int>("config.screenWidth");
    c.screenHeight = script.get<int>("config.screenHeight");
    c.screenBits = script.get<int>("config.screenBits");
    ctrls.isFullScreen = script.get<bool>("config.isFullScreen");

    c.defWalkingSpeed = script.get<float>("config.walkingSpeed");
    c.rotatingSpeed = script.get<float>("config.rotatingSpeed");
    c.targetSpeed = script.get<float>("config.targetSpeed");

    c.targetFPS = script.get<int>("config.targetFPS");

    c.mouseSensitivity = script.get<float>("config.mouseSensitivity");
    c.skyColor = SDL_Color{
        script.get<int>("config.skyColor.R"),
        script.get<int>("config.skyColor.G"),
        script.get<int>("config.skyColor.B"),
        255
    };

    c.floorColor = SDL_Color{
        script.get<int>("config.floorColor.R"),
        script.get<int>("config.floorColor.G"),
        script.get<int>("config.floorColor.B"),
        255
    };

    c.dayFogColor = ColorToUint(
        script.get<int>("config.dayFogColor.R"),
        script.get<int>("config.dayFogColor.G"),
        script.get<int>("config.dayFogColor.B")
    );

    c.nightFogColor = ColorToUint(
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
    Constants& c = Constants::get();
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

    RenderUtils::setWindowPos(8, 30);

    readConfig(data, controls);

    Uint32 fullscreen = controls.isFullScreen ? SDL_FULLSCREEN : 0x0;

    SDL_Surface* screen = SDL_SetVideoMode(c.screenWidth, c.screenHeight, c.screenBits,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF | fullscreen);

    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n",
               c.screenWidth, c.screenHeight, c.screenBits, SDL_GetError());
        return 1;
    }
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    data.loadAllTextures();

    RenderUtils::mirrorMipMaps(data.m_textures);
    RenderUtils::mirrorMipMaps(data.m_lightmaps);

    RenderUtils::doLightMapsToAllTextures(data.textures, data.lightmaps, data);
    RenderUtils::doLightMapsToAllTextures(data.m_textures, data.m_lightmaps, data);
    RenderUtils::setLightMapsTo0(data);

    //transposeTextures(data.sky_textures);
    RenderUtils::transposeMipMaps(data.textures);
    RenderUtils::transposeMipMaps(data.lightmaps);
    RenderUtils::transposeMipMaps(data.m_textures);
    RenderUtils::transposeMipMaps(data.m_lightmaps);

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

    size_t hardwareThreads = std::thread::hardware_concurrency();
    hardwareThreads = hardwareThreads ? hardwareThreads : 4;

    Thread::Pool pool(hardwareThreads);

    std::vector<std::future<void>> results;
    results.reserve(hardwareThreads);

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
        data.player.angle = util::clampLooping(data.player.angle, 0.0f, c.pi * 2);


        size_t numThreads;
        if(controls.multithreaded) numThreads = hardwareThreads;
        else numThreads = 1;


        for(size_t i = 0; i < numThreads; ++i) {
            results.emplace_back(pool.Add_Task(calcRays, &data.rays,
                        i * (c.screenWidth / numThreads), (i + 1) * (c.screenWidth / numThreads),
                            &data));
        }

        for(auto& a : results) a.wait();
        results.clear();

        for(size_t i = 0; i < numThreads; ++i) {
            results.emplace_back(pool.Add_Task(calculateDistances, &data.rays,
                        i * (c.screenWidth / numThreads), (i + 1) * (c.screenWidth / numThreads),
                            &data, &controls));
        }


        for(auto& a : results) a.wait();
        results.clear();


        for(size_t i = 0; i < numThreads; ++i) {
            results.emplace_back(pool.Add_Task(renderColumns, screen, &data.rays,
                         i * (c.screenWidth / numThreads), (i + 1) * (c.screenWidth / numThreads),
                                    rand(), &data, &controls));
        }

        for(auto& a : results) a.wait();
        results.clear();

        int end = SDL_GetTicks();
        frameTime = end - start;

        if(frameTime < (1000 / c.targetFPS))
        {
            SDL_Delay((1000 / c.targetFPS) - frameTime);
        }

        end = SDL_GetTicks();
        frameTime = end - start;

        if(count == 16)
        {
            sprintf(fps, "%d", 1000 / util::clamp(end - start, 1, INT_MAX));
            SDL_WM_SetCaption(fps, NULL);
            count = 0;
        }
        SDL_Flip(screen);
        ++count;
    }

    debug.close();
    std::cout.rdbuf(coutbuf);

    std::ofstream out("out.txt");

    util::print2dVector(data.map, out);

    out << std::endl;

    out.close();

    SDL_FreeSurface(screen);

    return 0;
}
