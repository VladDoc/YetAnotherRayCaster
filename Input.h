#pragma once

#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "ControlsState.h"
#include "player.h"
#include "rendering.h"

void destroyAWallThatPlayerIsFacing()
{
    float distanceToAWall = 0.0f;

    Vector2D<float> test;
    test.x = player.x;
    test.y = player.y;

    rayTraversal(player.angle, &distanceToAWall, &test);

    if(withinRange((float)test.x, 0.0f, (float)mapWidth) &&
       withinRange((float)test.y, 0.0f, (float)mapHeight)) {
            map[(int)test.y][(int)test.x].setEmpty();
    }
}

void createRandomColorWallNearby()
{
    Vector2D<int> wallLocation;
    wallLocation.x = (int)(player.x + sinf(player.angle) * 3.0f);
    wallLocation.y = (int)(player.y + cosf(player.angle) * 3.0f);

    wallLocation.x = clamp(wallLocation.x, 0, mapWidth);
    wallLocation.y = clamp(wallLocation.y, 0, mapHeight);

    if(map[wallLocation.y][wallLocation.x].isEmpty()) {
        map[wallLocation.y][wallLocation.x].r = rand() % 64;
        map[wallLocation.y][wallLocation.x].g = rand() % 64;
        map[wallLocation.y][wallLocation.x].b = rand() % 64;
    }
}

void changeResolution(SDL_Surface** screen, const Vector2D<int> res)
{
    if(isFullScreen) {
        *screen = SDL_SetVideoMode(res.x, res.y, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
    } else {
        *screen = SDL_SetVideoMode(res.x, res.y, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
    }
    screenWidth = res.x;
    screenHeight = res.y;

    horizonCap = calcHorizonCap();

    starsWidth = calcStarsWidth();
    starsHeight = calcStarsHeight();

    fillUpTheStars();
    FOV = calcFOV();
    allocateScreenSizeSensitiveData();
}

void checkControls(const SDL_Event event, SDL_Surface** screen) {
    static bool wasSkyColorChangePressed = false;
    static bool wasSkyIsAFloorPressed = false;
    static bool wasFullScreenTogglePressed = false;

    Uint8 *keyState = SDL_GetKeyState(NULL);

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
                    skyColor.r = rand() % 64;
                    skyColor.g = rand() % 64;
                    skyColor.b = rand() % 64;
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
                        *screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
                        isFullScreen = true;
                    } else {
                        *screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
                        isFullScreen = false;
                    }
                wasFullScreenTogglePressed = true;
                }
            }
            if(event.key.keysym.sym == SDLK_c) {
                horizonLine = 0;
            }
            if(keyState[SDLK_F6] && keyState[SDLK_F9]) {
                easterEgg = easterEgg ? false : true;
            }
            if(event.key.keysym.sym == SDLK_F12) {
                textureGradient = textureGradient ? false : true;
            }
            if(event.key.keysym.sym == SDLK_F11) {
                if(naiveApproach) {
                    naiveApproach = false;
                    blockBitSize = nonNaiveBBS;
                    horisontalBlockCheckStep = nonNaiveBBS * 2;
                } else {
                    naiveApproach = true;
                    blockBitSize = naiveBlockBitSize;
                    horisontalBlockCheckStep = naiveBlockBitSize;
                }
            }
            if(event.key.keysym.sym == SDLK_F2) {
                currentRes = clamp(currentRes - 1, 0, resArraySize-1);
                changeResolution(screen, resolutions[currentRes]);
            }
            if(event.key.keysym.sym == SDLK_F3) {
                currentRes = clamp(currentRes + 1, 0, resArraySize-1);
                changeResolution(screen, resolutions[currentRes]);
            }
            if(event.key.keysym.sym == SDLK_F5) {
                if(!vSync) {
                    targetFPS = 60;
                    vSync = true;
                } else {
                    targetFPS = 1000;
                    vSync = false;
                }
            }
            if(event.key.keysym.sym == SDLK_F10) {
                night = night ? false : true;
            }
            if(event.key.keysym.sym == SDLK_F9) {
                texturedSky = texturedSky ? false : true;
            }
            if(event.key.keysym.sym == SDLK_F6) {
                multithreaded = multithreaded ? false : true;
            }
            if(event.key.keysym.sym == SDLK_F7) {
                fog = fog ? false : true;
            }
            if(event.key.keysym.sym == SDLK_F8) {
                coloredLight = coloredLight ? false : true;
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
            horizonLine += (screenHeight / 2 - event.motion.y) * ((float)screenHeight / 400);
            horizonLine = clamp(horizonLine, -horizonCap, horizonCap);
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT) {
                destroyAWallThatPlayerIsFacing();
            }
            if(event.button.button == SDL_BUTTON_RIGHT) {
                createRandomColorWallNearby();
            }
            if(event.button.button == SDL_BUTTON_WHEELDOWN) {
                FOV -= 0.05f;
                FOV = clamp(FOV, pi / 8, pi);
            }
            if(event.button.button == SDL_BUTTON_WHEELUP) {
                FOV += 0.05f;
                FOV = clamp(FOV, pi / 8, pi);
            }
        }
        SDL_WarpMouse(screenWidth / 2, screenHeight / 2);
}


void doActions(const int frameTime) {
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
