#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#ifndef CONTROLSSTATE_H_INCLUDED
#include "ControlsState.h"
#endif // CONTROLSSTATE_H_INCLUDED

#ifndef PLAYER_H_INCLUDED
#include "player.h"
#endif // PLAYER_H_INCLUDED

void checkControls(SDL_Event event, SDL_Surface* screen) {
    static bool wasSkyColorChangePressed = false;
    static bool wasSkyIsAFloorPressed = false;
    static bool wasFullScreenTogglePressed = false;

    switch (event.type) {
        case SDL_QUIT:
            done = true;
            break;

        case SDL_KEYDOWN:
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                done = true;
            }
            if (event.key.keysym.sym == SDLK_a)
            {
                isLStrafeHeld = true;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                isRStrafeHeld = true;
            }
            if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
            {
                isDownHeld = true;
            }
            if(event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
            {
                isUpHeld = true;
            }
            if(event.key.keysym.sym == SDLK_LEFT)
            {
                isLeftHeld = true;
            }
            if(event.key.keysym.sym == SDLK_RIGHT)
            {
                isRightHeld = true;
            }
            if(event.key.keysym.sym == SDLK_LSHIFT)
            {
                walkingSpeed = defWalkingSpeed * 2;
            }
            if(event.key.keysym.sym == SDLK_PAGEUP)
            {
                if(!wasSkyColorChangePressed)
                {
                    skyColor.r = rand() % 40;
                    skyColor.g = rand() % 40;
                    skyColor.b = rand() % 40;
                    wasSkyColorChangePressed = true;
                }
            }
            if(event.key.keysym.sym == SDLK_HOME)
            {
                skyColor = defSkyColor;
            }
            if(event.key.keysym.sym == SDLK_END)
            {
                skyColor.r = 0;
                skyColor.g = 0;
                skyColor.b = 0;
            }
            if(event.key.keysym.sym == SDLK_DELETE) {
                shouldStarsBeRendered = false;
            }
            if(event.key.keysym.sym == SDLK_INSERT) {
                shouldStarsBeRendered = true;
            }
            if(event.key.keysym.sym == SDLK_PAGEDOWN) {
                 if(!wasSkyIsAFloorPressed) {
                    isFloorASky ? isFloorASky = false : isFloorASky = true;
                    wasSkyIsAFloorPressed = true;
                 }
            }
            if(event.key.keysym.sym == SDLK_F4) {
                if(!wasFullScreenTogglePressed) {
                    if(!isFullScreen) {
                        screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
                        isFullScreen = true;
                    } else {
                        screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
                        isFullScreen = false;
                    }
                wasFullScreenTogglePressed = true;
                }
            }
            if(event.key.keysym.sym == SDLK_c) {
                horizonLine = 0;
            }
            break;
        }
        case SDL_KEYUP:
        {
            if (event.key.keysym.sym == SDLK_a)
            {
                isLStrafeHeld = false;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                isRStrafeHeld = false;
            }
            if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
            {
                isDownHeld = false;
            }
            if(event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
            {
                isUpHeld = false;
            }
            if(event.key.keysym.sym == SDLK_LEFT)
            {
                isLeftHeld = false;
            }
            if(event.key.keysym.sym == SDLK_RIGHT)
            {
                isRightHeld = false;
            }
            if(event.key.keysym.sym == SDLK_LSHIFT)
            {
                walkingSpeed = defWalkingSpeed;
            }
            if(event.key.keysym.sym == SDLK_PAGEUP)
            {
                wasSkyColorChangePressed = false;
            }
            if(event.key.keysym.sym == SDLK_PAGEDOWN) {
                wasSkyIsAFloorPressed = false;
            }
            if(event.key.keysym.sym == SDLK_F4) {
                wasFullScreenTogglePressed = false;
            }
            break;
        }
        case SDL_MOUSEMOTION:
            player.angle -= rotatingSpeed * (float)(screenWidth / 2 - event.motion.x) / mouseSensitivity;
            horizonLine += (screenHeight / 2 - event.motion.y);
            horizonLine = clamp(horizonLine, -horizonCap, horizonCap);
            break;
        }
        SDL_WarpMouse(screenWidth / 2, screenHeight / 2);
}

void doActions(int frameTime) {
    if(isUpHeld) {
        player.x += sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.x < 0.0f || player.x > mapWidth)  {
            player.x -= sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
        player.y += cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.y < 0.0f || player.y > mapHeight)  {
            player.y -= cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
    }
    if(isDownHeld) {
        player.x -= sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.x < 0.0f || player.x > mapWidth)  {
            player.x += sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
        player.y -= cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.y < 0.0f || player.y > mapHeight)  {
            player.y += cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
    }
    if(isLeftHeld) {
        player.angle -= rotatingSpeed  * (frameTime / targetSpeed);
    }
    if(isRightHeld) {
        player.angle += rotatingSpeed  * (frameTime / targetSpeed);
    }
    if(isLStrafeHeld) {
        player.x -= sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.x < 0.0f || player.x > mapWidth)  {
            player.x += sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
        player.y -= cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.y < 0.0f || player.y > mapHeight)  {
            player.y += cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
    }
    if(isRStrafeHeld) {
        player.x += sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.x < 0.0f || player.x > mapWidth)  {
            player.x -= sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
        player.y += cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty() || player.y < 0.0f || player.y > mapHeight)  {
            player.y -= cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
    }
}

#endif // INPUT_H_INCLUDED
