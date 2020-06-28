#include "GameData.h"

#include <cmath>
#include <random>

#include "MapBlock.h"
#include "Utility.h"

void GameData::populateCoefs()
{
    Constants& c = Constants::get();
    skyScaleCoefs.clear();
    for(size_t i = 0; i < c.horizonCap; ++i) {
        skyScaleCoefs.push_back(sinf((float)i / ((float)c.horizonCap / (c.pi / 2))));
    }
    for(size_t i = skyScaleCoefs.size()-1; i > 0; --i) {
        float temp = skyScaleCoefs.at(i);
        skyScaleCoefs.push_back(temp);
    }
}

static void loadTextures(std::vector<SDL_Surface*>& txt) {
    util::loadTexture(txt, "wall2.bmp");
    util::loadTexture(txt, "wall.bmp");
    util::loadTexture(txt, "wall3.bmp");
    util::loadTexture(txt, "wall2.bmp");
    util::loadTexture(txt, "hellaworld.bmp");
}


static void loadLightmaps(std::vector<SDL_Surface*>& lmp) {
    util::loadTexture(lmp, "wall2bumpmap.bmp");
}

static void loadSkyTextures(std::vector<SDL_Surface*>& skyTxt)
{
    util::loadTexture(skyTxt, "sky.bmp");
}


static void loadSprites(std::vector<Sprite>& sprts)
{
//    Sprite spr({0, 0, 0, 0}, "");
//    sprts.push_back(spr);
}

void GameData::fillUpTheStars() {
    Constants& c = Constants::get();

    std::mt19937 r;
    r.seed(1);
    stars.clear();
    stars.resize(c.starsWidth * (c.starsHeight + c.screenHeight / 2 + 1000));


    for(int i = 0; i < c.starsHeight; ++i) {
        for(int j = 0; j < c.starsWidth; ++j) {
            if(!(r() % c.screenWidth / 2)) {
                stars[i * c.starsWidth + j] = r() & 0xFF;
            }
        }
    }
}

void GameData::loadAllTextures()
{
    using namespace util;
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

void GameData::allocateScreenSizeSensitiveData()
{
    Constants& c = Constants::get();

    distances.resize(c.screenWidth);
    rayPositions.resize(c.screenWidth);
    rays.resize(c.screenWidth);
    eyes.resize(c.screenWidth);

    fillUpTheStars();
    populateCoefs();
}

void freeScreenSizeSensitiveData()
{
}

void GameData::freeTextures()
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

GameData::~GameData()
{
    freeScreenSizeSensitiveData();
    freeTextures();
}

Vector2D<bool> GameData::initMapFromFile(const char* filename)
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
    Constants::get().mapWidth = map[0].size(); // Possible bug if lines are different sizes.
    Constants::get().mapHeight = map.size();
}

// Returns true once finished.
bool GameData::tracePath(std::vector<Vector2D<int>>& path, int frametime)
{
    if(lastPos >= path.size()-2) return true;
    player.angle += (util::directionToAngle((path[lastPos+1].y - path[lastPos].y),
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

