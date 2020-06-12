#pragma once

#ifndef MAPBLOCK_H_INCLUDED
#define MAPBLOCK_H_INCLUDED

#include <ostream>

class MapBlock
{
public:
    constexpr static const SDL_Color defWallColor{45, 20, 0, 255};
    Uint8 r{};
    Uint8 g{};
    Uint8 b{};
    Uint16 texture{};
    Uint16 lightmap{};
    float height = 1.0f;

    MapBlock(): r(0), g(0), b(0), texture(0), lightmap(0), height(1.0f) {}

    MapBlock& operator=(const MapBlock& m)
    {
        r = m.r;
        g = m.g;
        b = m.b;
        texture = m.texture;
        lightmap = m.lightmap;
        height = m.height;

        return *this;
    }

    MapBlock(const MapBlock& m)
    {
        r = m.r;
        g = m.g;
        b = m.b;
        texture = m.texture;
        lightmap = m.lightmap;
        height = m.height;
    }
    ~MapBlock() {}

    MapBlock(int red, int green, int blue) :
        r((Uint8)red), g((Uint8)green), b((Uint8)blue), texture(0), lightmap(0) { }

    MapBlock(int red, int green, int blue, float h) :
        r((Uint8)red), g((Uint8)green), b((Uint8)blue), height(h) { }

    MapBlock(int textureType) : texture((Uint16)textureType), lightmap(0) { }

    MapBlock(int textureType, float h) : texture((Uint16)textureType), height(h) { }

    MapBlock(int textureType, int lightmapType) :
        texture((Uint16)textureType), lightmap((Uint16)lightmapType) { }

    MapBlock(int textureType, int lightmapType, float h) :
        texture((Uint16)textureType), lightmap((Uint16)lightmapType), height(h) { }

    inline bool getIsTextured() {
        return texture >= 2;
    }

    inline bool getIsLightMapped() {
        return lightmap >= 1;
    }

    void setDefault() {
        r = 0;
        g = 0;
        b = 0;
        texture = 1;
        lightmap = 0;
    }

    void setEmpty() {
        r = 0;
        g = 0;
        b = 0;
        texture = 0;
        lightmap = 0;
    }

    inline int getTextureIndex() {
        return (int)(texture)-2;
    }

    inline int getLightMapIndex() {
        return (int)(lightmap)-1;
    }

    inline bool isDefault() const
    {
        return !(r || g || b || lightmap) && texture == 1;
    }

    SDL_Color getColor() const {
        if(isDefault()) {
            return defWallColor;
        }
        SDL_Color retColor;
        retColor.r = r;
        retColor.g = g;
        retColor.b = b;
        return retColor;
    }

    inline bool isEmpty() const {
        return !(r || g || b || texture);
    }

    friend std::ostream& operator<<(std::ostream& os, const MapBlock& mb);
};

inline std::ostream& operator<<(std::ostream& os, const MapBlock& mb)
{
    if(!mb.isEmpty()) os << "0";
    else os << " ";
    return os;
}

#endif // MAPBLOCK_H_INCLUDED
