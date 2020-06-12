#include "Utility.h"

#include "MapBlock.h"
#include "Sprite.h"

void util::loadTexture(std::vector<SDL_Surface*>& txt, const char* filename)
{
    SDL_Surface* surf = SDL_LoadBMP(filename);
    SDL_Surface* texture = SDL_DisplayFormat(surf);
    if(!surf || !texture) {
        fprintf(stderr, "%s", "Unable to load textures. Exiting.....\n");
        fprintf(stderr, "%s", SDL_GetError());
        exit(-1);
    }
    SDL_FreeSurface(surf);
    txt.push_back(texture);
}


float util::intersectDist(const Vector2D<float>& test,
                          float sine, float cosine,
                          int quarter, float margin)
{

    /*
     *
     *          Actually gives speed up, but is held on duct tape.
     *          Spend a whole week getting it to work.
     *          Almost lost my sanity in the process.
     *          Performance changes depending on
     *          which quadrant of circle player currently is.
     *
     */


    Vector2D<float> distance;
    Vector2D<float> delta;
    Vector2D<float> scaleCoeffs;


    switch(quarter) {
        case 0:
            delta.x = 1.0f - getFractialPart(test.x);
            scaleCoeffs.x = sine;
            delta.y = 1.0f - getFractialPart(test.y);
            scaleCoeffs.y = cosine;
        break;
        case 1:
            delta.x = 1.0f - getFractialPart(test.x);
            scaleCoeffs.x = sine;
            delta.y = getFractialPart(test.y);
            scaleCoeffs.y = -cosine;
        break;
        case 2:
            delta.x = getFractialPart(test.x);
            scaleCoeffs.x = -sine;
            delta.y = getFractialPart(test.y);
            scaleCoeffs.y = -cosine;
        break;
        case 3:
            delta.x = getFractialPart(test.x);
            scaleCoeffs.x = -sine;
            delta.y = 1.0f - getFractialPart(test.y);
            scaleCoeffs.y = cosine;
        break;
        default:
            return margin;
        break;
    }

     // Delta should not be zero otherwise renderer will loop forever.
    if(delta.x < margin) {
        delta.x = margin;
    }
    if(delta.y < margin) {
        delta.y = margin;
    }


    distance.x = delta.x / scaleCoeffs.x;

    distance.y = delta.y / scaleCoeffs.y;

    return distance.x < distance.y ? distance.x : distance.y;
}

void util::loadTextures(std::vector<SDL_Surface*>& txt) {
    loadTexture(txt, "wall2.bmp");
    loadTexture(txt, "wall.bmp");
    loadTexture(txt, "wall3.bmp");
    loadTexture(txt, "wall2.bmp");
    loadTexture(txt, "hellaworld.bmp");
}


void util::loadLightmaps(std::vector<SDL_Surface*>& lmp) {
    loadTexture(lmp, "wall2bumpmap.bmp");
}


void util::fillUpTheMapToBeBox(MapBlock** aMap)
{
    Constants& c = Constants::get();
    for(int i = 0; i < c.mapWidth; ++i)
    {
        aMap[0][i].setDefault();
    }

    for(int i = 1; i < c.mapHeight-1; ++i)
    {
        for(int j = 0; j < c.mapWidth; ++j)
        {
            if(j == 0 || j == c.mapWidth-1) aMap[i][j].setDefault();
            else aMap[i][j].setEmpty();
        }
    }

    for(int i = 0; i < c.mapWidth; ++i)
    {
        aMap[c.mapHeight-1][i].setDefault();
    }
}


void util::loadSkyTextures(std::vector<SDL_Surface*>& skyTxt)
{
    loadTexture(skyTxt, "sky.bmp");
}


void util::loadSprites(std::vector<Sprite>& sprts)
{
    Sprite spr({0, 0, 0, 0}, "");
    sprts.push_back(spr);
}

