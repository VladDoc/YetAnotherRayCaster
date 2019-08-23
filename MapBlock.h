#ifndef MAPBLOCK_H_INCLUDED
#define MAPBLOCK_H_INCLUDED

class MapBlock
{
public:
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint16 texture;
    Uint16 lightmap;

    MapBlock(int red, int green, int blue) :
        r((Uint8)red), g((Uint8)green), b((Uint8)blue), texture(0), lightmap(0) { }

    MapBlock(int textureType) : texture((Uint16)textureType), lightmap(0) { }

    MapBlock(int textureType, int lightmapType) :
        texture((Uint16)textureType), lightmap((Uint16)lightmapType) { }

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

    SDL_Color getColor(){
        SDL_Color retColor;
        retColor.r = r;
        retColor.g = g;
        retColor.b = b;
        return retColor;
    }

    inline bool isEmpty() {
        return !(r || g || b || texture);
    }
};

#endif // MAPBLOCK_H_INCLUDED
