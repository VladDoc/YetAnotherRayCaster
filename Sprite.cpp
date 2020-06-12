#include "Sprite.h"

#include "RenderUtils.h"

void Sprite::renderOnScreen(SDL_Surface* screen)
{
    using namespace RenderUtils;
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
