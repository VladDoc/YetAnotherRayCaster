#ifndef GAMEDATA_H_INCLUDED
#define GAMEDATA_H_INCLUDED

#include <vector>
#include <fstream>
#include <map>

#include <SDL/SDL.h>

#include "Sprite.h"
#include "MapBlock.h"
#include "Vector2D.h"
#include "GameConstants.h"
#include "MipmapTex.h"

struct GameData
{
    public:
        float walkingSpeed = 0.2f;

        int horizonLine = 0; // 0 is default it means that horizon won't be changed

        volatile bool done = false;

        std::vector<MipmapTex> textures;
        std::vector<MipmapTex> lightmaps;

        // m_ stands for mirrored
        std::vector<MipmapTex> m_textures;
        std::vector<MipmapTex> m_lightmaps;

        std::vector<SDL_Surface*> sky_textures;
        Uint32 avgSkyColor = 0;

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


        std::vector<Uint8> stars;

        std::vector<float> distances;
        std::vector<float> rays;

        std::vector<Vector2D<float>> eyes;
        std::vector<Vector2D<float>> rayPositions;
        int millisAtCell;

        std::vector<float> skyScaleCoefs;

        void populateCoefs();

        void fillUpTheStars();

        void loadAllTextures();

        void allocateScreenSizeSensitiveData();

        void freeScreenSizeSensitiveData() { }

        void freeTextures();

        ~GameData();

        Player player{2.0f, 2.0f, Constants::get().pi / 4};

        Vector2D<int> destination{};

        Vector2D<bool> initMapFromFile(const char* filename);

        // Returns true once finished.
        bool tracePath(std::vector<Vector2D<int>>& path, int frametime);
    protected:
        int lastPos = 0;
};

enum class SideOfAWall
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
    COUNT
};

const std::map<std::string, SideOfAWall> stringToSideOfAWall =
{
    {"NORTH", SideOfAWall::NORTH},
    {"EAST", SideOfAWall::EAST},
    {"SOUTH", SideOfAWall::SOUTH},
    {"WEST", SideOfAWall::WEST}
};

inline SideOfAWall whichSide(bool isMirrored, bool isHorisontal)
{
    if(!isMirrored &&  isHorisontal)    return SideOfAWall::NORTH;
    if(!isMirrored && !isHorisontal)    return SideOfAWall::WEST;
    if( isMirrored &&  isHorisontal)    return SideOfAWall::SOUTH;
    if( isMirrored && !isHorisontal)    return SideOfAWall::EAST;
}


#endif // GAMEDATA_H_INCLUDED
