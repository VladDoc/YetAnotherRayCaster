#pragma once

#ifndef GAMEDATA_H_INCLUDED
#define GAMEDATA_H_INCLUDED

#include <vector>
#include <fstream>

#include "GameConstants.h"
#include "player.h"
#include "MapBlock.h"
#include "Sprite.h"

struct GameData {
    float walkingSpeed = 0.2f;

    int horizonLine = 0; // 0 is default it means that horizon won't be changed

    bool done = false;

    std::vector<SDL_Surface*> textures;
    std::vector<SDL_Surface*> lightmaps;

    // m_ stands for mirrored
    std::vector<SDL_Surface*> m_textures;
    std::vector<SDL_Surface*> m_lightmaps;

    std::vector<SDL_Surface*> sky_textures;

    std::vector<Sprite> hudSprites;

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

    void allocateScreenSizeSensitiveData()
    {
        using namespace Constants;
        distances = (float*) realloc(distances, sizeof(float) * screenWidth);
        rayPositions = (Vector2D<float>*) realloc(rayPositions, sizeof(Vector2D<float>) * screenWidth);
        rays =  (float*) realloc(rays, sizeof(float) * screenWidth);
        fillUpTheStars();
    }

    void freeScreenSizeSensitiveData()
    {
        free(distances);
        free(rayPositions);
        free(rays);
        free(stars);
    }

    Player player{2.0f, 2.0f, Constants::pi / 4};

    Vector2D<float> destination{};

    void initMapFromFile(const char* filename)
    {
        std::ifstream in(filename);
        if(!in.good()) return;

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
                    player.x = j;
                    player.y = i;
                    map[i][j]= empty;
                }
                if(buf == 'E') {
                    destination.x = j;
                    destination.y = i;
                    map[i][j] = {100, 0, 0};
                }
            }
            ++i;
        }
        Constants::mapWidth = map[0].size();
        Constants::mapHeight = map.size();
    }
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

#endif // GAMEDATA_H_INCLUDED
