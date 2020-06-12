#pragma once

#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED

#include <SDL/SDL.h>

#include "Rectangle.h"

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

    void renderOnScreen(SDL_Surface* screen);
};


#endif // SPRITE_H_INCLUDED
