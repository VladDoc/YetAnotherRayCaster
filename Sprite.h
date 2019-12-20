#pragma once

#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include "Rectangle.h"
#include "Utility.h"

Uint8 getAlpha(Uint32); // Circular references

struct Sprite
{
public:
    Rectangle<int> rect;

    SDL_Surface* pixels;

    Sprite(Rectangle<int>&& r, SDL_Surface* surf):
        rect(r),  pixels(surf) {}
    Sprite(Rectangle<int>&& r, const char* filename): rect(r)
    {
        SDL_Surface* surf = SDL_LoadBMP(filename);
        pixels = SDL_DisplayFormat(surf);
        if(!surf || !pixels) {
            printf("Unable to load textures. Exiting.....\n");
            system("PAUSE");
            exit(-1);
        }

        rect.h = pixels->h;
        rect.w = pixels->w;
    }

    void renderOnScreen(SDL_Surface* screen)
    {
        for(int i = rect.y; i < rect.y + rect.h; ++i) {
            if(i < 0 || i >= screen->h) continue;
            for(int j = rect.x; j < rect.x + rect.w; ++j) {
                if(j < 0 || j >= screen->w) continue;
                Uint32* screenPixel = getTexturePixel(screen, i, j);
                Uint32* spritePixel = getTexturePixel(pixels, i - rect.h, j - rect.x);
                *screenPixel = blend(*screenPixel, *spritePixel, getAlpha(*spritePixel));
            }
        }
    }
};


#endif // SPRITE_H_INCLUDED
