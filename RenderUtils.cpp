#include "RenderUtils.h"

#include <cassert>

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

void RenderUtils::doLightMapsToAllTextures(std::vector<MipmapTex>& txt,
                                           std::vector<MipmapTex>& lmp,
                                           GameData& d)
{
    Constants& c = Constants::get();

    std::vector<bool> wasApplied(txt.size());

    for(size_t i = 0; i < c.mapHeight; ++i) {
        for(size_t j = 0; j < c.mapWidth; ++j) {
            if(d.map[i][j].getIsLightMapped() && d.map[i][j].getIsTextured()) {
                size_t txtIndex = d.map[i][j].getTextureIndex();
                size_t lmpIndex = d.map[i][j].getLightMapIndex();
                if(!wasApplied[txtIndex]) {
                    for(size_t k = 0; k < MipmapTex::levels; ++k) {
                        applyLightMapToTexture(txt.at(txtIndex).mipmaps[k],
                                               lmp.at(lmpIndex).mipmaps[k]);
                    }
                }
                wasApplied[txtIndex] = true;
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


SDL_Surface* RenderUtils::CreateSurface(size_t w, size_t h, size_t bitsPerPixel)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    static Uint32 rmask = 0xff000000;
    static Uint32 gmask = 0x00ff0000;
    static Uint32 bmask = 0x0000ff00;
    static Uint32 amask = 0x000000ff;
#else
    static Uint32 rmask = 0x000000ff;
    static Uint32 gmask = 0x0000ff00;
    static Uint32 bmask = 0x00ff0000;
    static Uint32 amask = 0xff000000;
#endif

    return SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, bitsPerPixel,
                                rmask, gmask, bmask, amask);
}

SDL_Surface* RenderUtils::CreateScaledSurfaceFrom(SDL_Surface* surf, size_t newW, size_t newH, bool filter)
{
    SDL_Surface* result = CreateSurface(newW, newH, surf->format->BitsPerPixel);

    assert(result);

    auto func = filter ?
                getScaledTexturePixelFiltered :
                getScaledTexturePixelColor;

    for(int i = 0; i < newH; ++i) {
        for(int j = 0; j < newW; ++j) {
            *getTexturePixel(result, i, j) = func(surf, newW, newH, i, j);
        }
    }

    return result;
}

SDL_Surface* RenderUtils::CreateMipMap(SDL_Surface* surf)
{
    using namespace RenderUtils;
    int newH = surf->h / 2;
    int newW = surf->w / 2;
    SDL_Surface* result = CreateSurface(newW, newH, surf->format->BitsPerPixel);

    assert(result);

    for(int i = 0; i < newH; ++i) {
        for(int j = 0; j < newW; ++j) {
            Uint32* p = getTexturePixel(result, i, j);

            Vector2D<int> origin{j * 2, i * 2};
            Vector2D<int> plus1{util::clamp(origin.x + 1, 0, surf->w-1) - origin.x,
                                util::clamp(origin.y + 1, 0, surf->h-1) - origin.y};

            Vector2D<int> minus1{util::clamp(origin.x - 1, 0, surf->w-1) - origin.x,
                                 util::clamp(origin.y - 1, 0, surf->h-1) - origin.y};

            *p = *getTexturePixel(surf, i * 2, j * 2);

            *p = blend(*getTexturePixel(surf,            origin.y, origin.x +  plus1.x), *p, 128);
            *p = blend(*getTexturePixel(surf, origin.y +  plus1.y,            origin.x), *p, 128);
            *p = blend(*getTexturePixel(surf, origin.y +  plus1.y, origin.x +  plus1.x), *p, 128);
            /*
            *p = blend(*getTexturePixel(surf,            origin.y, origin.x + minus1.x), *p, 128);
            *p = blend(*getTexturePixel(surf, origin.y + minus1.y,            origin.x), *p, 128);
            *p = blend(*getTexturePixel(surf, origin.y + minus1.y, origin.x + minus1.x), *p, 128);
            *p = blend(*getTexturePixel(surf, origin.y +  plus1.y, origin.x + minus1.x), *p, 128);
            *p = blend(*getTexturePixel(surf, origin.y + minus1.y, origin.x +  plus1.x), *p, 128);*/
        }
    }

    return result;
}

void RenderUtils::transposeTexture(SDL_Surface** txt)
{
    SDL_Surface* newTxt = RenderUtils::CreateSurface((*txt)->h, (*txt)->w,
                                                     (*txt)->format->BitsPerPixel);

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

void RenderUtils::transposeMipMaps(std::vector<MipmapTex>& txts)
{
    for(auto& i : txts) {
        for(auto& a : i.mipmaps) {
            transposeTexture(&a);
        }
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

void RenderUtils::mirrorMipMaps(std::vector<MipmapTex>& txt) {
    for(auto& i : txt) {
        for(auto& a : i.mipmaps) {
            mirrorTexture(a);
        }
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

