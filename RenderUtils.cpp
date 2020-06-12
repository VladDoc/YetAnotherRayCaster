#include "RenderUtils.h"

void RenderUtils::applyLightMapToTexture(SDL_Surface* texture, SDL_Surface* lightmap)
{
    Vector2D<float> coeffs;
    coeffs.x = (float)lightmap->w / (float)texture->w;
    coeffs.y = (float)lightmap->h / (float)texture->h;
    for(int i = 0; i < texture->h; ++i) {
        for(int j = 0; j < texture->w; ++j) {

            Uint32* texPixel = getTexturePixel(texture, i, j);
            Uint32* lightmapPixel = getTexturePixel(lightmap, (int)(i * coeffs.y),(int)(j * coeffs.x));

            SDL_Color pixelColor = UintToColor(*texPixel);
            pixelColor = transformColorByLightMap(pixelColor, UintToColor(*lightmapPixel));

            *texPixel = ColorToUint(pixelColor.r, pixelColor.g, pixelColor.b);
        }
    }
}

void RenderUtils::doLightMapsToAllTextures(std::vector<SDL_Surface*>& txt,
                              std::vector<SDL_Surface*>& lmp,
                              GameData& d)
{
    Constants& c = Constants::get();
    for(int i = 0; i < c.mapHeight; ++i) {
        for(int j = 0; j < c.mapWidth; ++j) {
            if(d.map[i][j].getIsLightMapped() && d.map[i][j].getIsTextured()) {
                applyLightMapToTexture(txt.at(d.map[i][j].getTextureIndex()),
                                       lmp.at(d.map[i][j].getLightMapIndex()));
            }
        }
    }
}

void RenderUtils::setLightMapsTo0(GameData& d)
{
    Constants& c = Constants::get();
    for(int i = 0; i < c.mapHeight; ++i) {
        for(int j = 0; j < c.mapWidth; ++j) {
            d.map[i][j].lightmap = 0;
        }
    }
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif

void RenderUtils::transposeTexture(SDL_Surface** txt)
{
    SDL_Surface* newTxt = SDL_CreateRGBSurface(0, (*txt)->h, (*txt)->w,
                                               (*txt)->format->BitsPerPixel,
                                               rmask, gmask, bmask, amask);
    for(int i = 0; i < (*txt)->h; ++i) {
        for(int j = 0; j < (*txt)->w; ++j) {
            Uint32* oldPixel = getTexturePixel(*txt, i, j);
            Uint32* newPixel = getTransposedTexturePixel(newTxt, i, j);

            *newPixel = *oldPixel;
        }
    }

    SDL_FreeSurface(*txt);
    *txt = newTxt;
}

void RenderUtils::transposeTextures(std::vector<SDL_Surface*>& txts)
{
    for(auto& i : txts) {
        transposeTexture(&i);
    }
}

void RenderUtils::setWindowPos(int x, int y)
{
    char env[80];
    sprintf(env, "SDL_VIDEO_WINDOW_POS=%d,%d", x, y);

    SDL_putenv(env);
}

void RenderUtils::mirrorTexture(SDL_Surface* txt) {
    for(int i = 0; i < txt->h; ++i) {
        for(int j = 0; j < txt->w / 2; ++j) {
            Uint32* left = getTexturePixel(txt, i, j);
            Uint32* right = getTexturePixel(txt, i, txt->w - j - 1);

            Uint32 temp = *left;
            *left = *right;
            *right = temp;
        }
    }
}

void RenderUtils::mirrorTextures(std::vector<SDL_Surface*>& txt) {
    for(auto& i : txt) {
        mirrorTexture(i);
    }
}

Uint32* RenderUtils::getSpherePixelOld(SDL_Surface* txt, float height, float sine, float cosine,
                                            int i, int j)
{
    Constants& c = Constants::get();
    int middle = std::min(txt->h / 2, txt->w / 2);

    int IndexI = middle + (middle * height * cosine);
    int IndexJ = middle + (middle * height * sine);

//    IndexI += (i - screenHeight / 2) * ((float)(screenHeight / 2) / middle);
//    IndexJ += (j - screenWidth / 2) * ((float)(screenWidth / 2) / middle);

    //j = std::cos(pi / 2)*j - std::sin(pi / 2)*i;
    //i = std::sin(pi / 2)*j + std::cos(pi / 2)*i;

    return getTexturePixel(txt, IndexI, IndexJ);
}

Uint32* RenderUtils::getSpherePixel(SDL_Surface* txt, float height, float angle)
{
    Constants& c = Constants::get();
    int middleX = txt->w / 2;
    int middleY = txt->h / 2;

    height -= c.pi / 2;
    //angle = clampLooping(angle, 0.0f, 2 * pi);
    angle -= c.pi;


    int i = middleY + (int)(middleY * (height / (c.pi / 2)));
    int j = middleX + (int)(middleX * (angle / c.pi));

    //j = std::cos(pi / 2)*j - std::sin(pi / 2)*i;
    //i = std::sin(pi / 2)*j + std::cos(pi / 2)*i;

    return getTexturePixel(txt, i, j);
}

Uint32 RenderUtils::ColorToUint(int R, int G, int B)
{
    return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

SDL_Color RenderUtils::UintToColor(Uint32 color)
{
    SDL_Color retColor;

    retColor.unused = (color >> 24) & 0xFF;
    retColor.r = (color >> 16) & 0xFF;
    retColor.g = (color >> 8) & 0xFF;
    retColor.b = color & 0xFF;

    return retColor;
}

