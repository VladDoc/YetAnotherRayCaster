#ifndef MIPMAPTEX_H_INCLUDED
#define MIPMAPTEX_H_INCLUDED

struct SDL_Surface;

struct MipmapTex
{
    static const int levels = 8;
    static const int bias = 0;
    SDL_Surface* mipmaps[levels];
};

#endif // MIPMAPTEX_H_INCLUDED
