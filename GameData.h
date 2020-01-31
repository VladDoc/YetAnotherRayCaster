#pragma once

#ifndef GAMEDATA_H_INCLUDED
#define GAMEDATA_H_INCLUDED

#include <vector>
#include <fstream>
#include <map>

#include "Utility.h"
#include "GameConstants.h"
#include "player.h"
#include "MapBlock.h"
#include "Sprite.h"

// I think it is time to stop being header pnly
float directionToAngle(int x, int y);

void loadSkyTextures(std::vector<SDL_Surface*>&);

void loadTextures(std::vector<SDL_Surface*>&);

void loadLightmaps(std::vector<SDL_Surface*>&);

struct GameData {
public:
    float walkingSpeed = 0.2f;

    int horizonLine = 0; // 0 is default it means that horizon won't be changed

    volatile bool done = false;

    std::vector<SDL_Surface*> textures;
    std::vector<SDL_Surface*> lightmaps;

    // m_ stands for mirrored
    std::vector<SDL_Surface*> m_textures;
    std::vector<SDL_Surface*> m_lightmaps;

    std::vector<SDL_Surface*> sky_textures;

    std::vector<Sprite> hudSprites;

    std::vector<std::string> texturePaths;
    std::vector<std::string> lightmapPaths;
    std::vector<std::string> skyTexturePaths;

    std::vector<std::vector<MapBlock>> map =
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {2,1}},
        {3, 0, 0, 0, 0, 0, 0, 0, {0, 0, 50}, {0, 0, 50}, {20, 0, 50}, {20, 0, 50}, {20, 0, 50}, {20, 0, 50}, 0, 1},
        {3, 0, 0, 4, 0, 0, 0, 0, {50, 0, 0}, {50, 0, 0}, {50, 0, 0}, {50, 0, 0}, {50, 0, 0}, 0, 0, 1},
        {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {6, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };


    Uint8* stars{};

    float* distances{};
    float* rays{};
    Vector2D<float>* rayPositions{};
    int millisAtCell;

    std::vector<float> skyScaleCoefs;

    void populateCoefs()
    {
        using namespace Constants;
        skyScaleCoefs.clear();
        for(size_t i = 0; i < horizonCap; ++i) {
            skyScaleCoefs.push_back(sinf((float)i / ((float)horizonCap / (pi / 2))));
        }
        for(size_t i = skyScaleCoefs.size()-1; i > 0; --i) {
            float temp = skyScaleCoefs.at(i);
            skyScaleCoefs.push_back(temp);
        }
    }

    void fillUpTheStars() {
        using namespace Constants;
        std::mt19937 r;
        r.seed(1);
        stars = (Uint8*)realloc(stars, starsWidth * starsHeight * sizeof(*stars));
        memset(stars, 0, starsWidth * starsHeight);

        for(int i = 0; i < starsHeight; ++i) {
            for(int j = 0; j < starsWidth; ++j) {
                if(!(r() % screenWidth / 2)) {
                    stars[i * starsWidth + j] = r() & 0xFF;
                }
            }
        }
    }

    void loadAllTextures()
    {
        if(skyTexturePaths.empty()) {
            loadSkyTextures(sky_textures);
        } else {
            for(auto& path : skyTexturePaths) {
                if(!path.empty()) loadTexture(sky_textures, path.c_str());
            }
        }
        if(texturePaths.empty()) {
            loadTextures(textures);
        } else {
            for(auto& path : texturePaths) {
                if(!path.empty()) {
                        loadTexture(textures, path.c_str());
                        loadTexture(m_textures, path.c_str());
                }
            }
        }
        if(lightmapPaths.empty()) {
            loadLightmaps(lightmaps);
        } else {
            for(auto& path : lightmapPaths) {
                if(!path.empty()) {
                        loadTexture(lightmaps, path.c_str());
                        loadTexture(m_lightmaps, path.c_str());
                }
            }
        }
    }

    void allocateScreenSizeSensitiveData()
    {
        using namespace Constants;
        distances = (float*) realloc(distances, sizeof(float) * screenWidth);
        rayPositions = (Vector2D<float>*) realloc(rayPositions, sizeof(Vector2D<float>) * screenWidth);
        rays =  (float*) realloc(rays, sizeof(float) * screenWidth);
        fillUpTheStars();
        populateCoefs();
    }

    void freeScreenSizeSensitiveData()
    {
        free(distances);
        free(rayPositions);
        free(rays);
        free(stars);
    }

    void freeTextures()
    {
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

    ~GameData()
    {
        freeScreenSizeSensitiveData();
        freeTextures();
    }

    Player player{2.0f, 2.0f, Constants::pi / 4};

    Vector2D<int> destination{};


    Vector2D<bool> initMapFromFile(const char* filename)
    {
        Vector2D<bool> state{0,0}; // If no init happened then both are false
        std::ifstream in(filename);
        if(!in.good()) return state;


        map.clear(); // Map is vector<vector<MapBlock>>

        std::string line{};

        size_t i = 0;
        MapBlock def{2};
        MapBlock empty{0};
        while(std::getline(in, line) && !in.eof()) {
            map.push_back(std::vector<MapBlock>{});
            map[i].resize(line.size());
            for(size_t j = 0; j < line.size(); ++j) {
                char buf = line[j];
                if(buf == '0') map[i][j] = def;
                if(buf == ' ') map[i][j] = empty;
                if(buf == 'I') {
                    // So multiple occurrences of 'I' won't break anything
                    if(!state.x) {
                        player.x = j + 0.5f; // so you start in the middle of a cell, how you'd expect
                        player.y = i + 0.5f;
                        state.x = true;
                    }
                    map[i][j] = empty;
                }
                if(buf == 'E') {
                    // multiple destinations don't make sense either
                    if(!state.y) {
                        destination.x = j;
                        destination.y = i;
                        //map[i][j] = {100, 0, 0};
                        map[i][j]= empty;
                        state.y = true;
                    }
                }
            }
            ++i;
        }
        Constants::mapWidth = map[0].size(); // Possible bug if lines are different sizes.
        Constants::mapHeight = map.size();
    }

    // Returns true once finished.
    bool tracePath(std::vector<Vector2D<int>>& path, int frametime)
    {
        if(lastPos >= path.size()-2) return true;
        player.angle += (directionToAngle((path[lastPos+1].y - path[lastPos].y),
                                          (path[lastPos+1].x - path[lastPos].x))
                               - player.angle) * ((float)frametime / millisAtCell);
        player.x += (path[lastPos+1].x - path[lastPos].x) *
                            ((float)frametime / millisAtCell);
        player.y += (path[lastPos+1].y - path[lastPos].y) *
                            ((float)frametime / millisAtCell);
        if((int)(player.x) == path[lastPos+1].x &&
           (int)(player.y) == path[lastPos+1].y) {
               lastPos += 1;
           }
        return false;
    }
protected:
    int lastPos = 0;
};

thread_local std::mt19937 random;
thread_local std::uniform_int_distribution<int> dist{0, 255};

enum class SideOfAWall
{
    NORTH,
    EAST,
    SOUTH,
    WEST
};

const std::map<std::string, SideOfAWall> stringToSideOfAWall =
{
    {"NORTH", SideOfAWall::NORTH},
    {"EAST", SideOfAWall::EAST},
    {"SOUTH", SideOfAWall::SOUTH},
    {"WEST", SideOfAWall::WEST}
};


#endif // GAMEDATA_H_INCLUDED
