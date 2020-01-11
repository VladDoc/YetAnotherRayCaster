#pragma once

#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "ControlsState.h"
#include "player.h"
#include "rendering.h"

void destroyAWallThatPlayerIsFacing(GameData& gamedata)
{
    using namespace Constants;
    float distanceToAWall = 0.0f;

    Vector2D<float> test;
    test.x = gamedata.player.x;
    test.y = gamedata.player.y;

    rayTraversal(gamedata, gamedata.player.angle, &distanceToAWall, &test);

    if(withinRange((float)test.x, 0.0f, (float)mapWidth) &&
       withinRange((float)test.y, 0.0f, (float)mapHeight)) {
            gamedata.map[(int)test.y][(int)test.x].setEmpty();
    }
}

void createRandomColorWallNearby(GameData& d)
{
    using namespace Constants;
    Vector2D<int> wallLocation;
    wallLocation.x = (int)(d.player.x + sinf(d.player.angle) * 3.0f);
    wallLocation.y = (int)(d.player.y + cosf(d.player.angle) * 3.0f);

    wallLocation.x = clamp(wallLocation.x, 0, mapWidth);
    wallLocation.y = clamp(wallLocation.y, 0, mapHeight);

    if(d.map[wallLocation.y][wallLocation.x].isEmpty()) {
        d.map[wallLocation.y][wallLocation.x].r = rand() % 64;
        d.map[wallLocation.y][wallLocation.x].g = rand() % 64;
        d.map[wallLocation.y][wallLocation.x].b = rand() % 64;
    }
}

void changeResolution(SDL_Surface** screen, const Vector2D<int> res, GameData& gamedata)
{
    using namespace Constants;
    if(controls.isFullScreen) {
        *screen = SDL_SetVideoMode(res.x, res.y, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
    } else {
        *screen = SDL_SetVideoMode(res.x, res.y, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
    }
    screenWidth = res.x;
    screenHeight = res.y;

    horizonCap = calcHorizonCap();

    starsWidth = calcStarsWidth();
    starsHeight = calcStarsHeight();

    FOV = calcFOV();
    gamedata.allocateScreenSizeSensitiveData();
}

void checkControls(const SDL_Event event, SDL_Surface** screen, GameData& gamedata) {
    using namespace Constants;
    static bool wasSkyColorChangePressed = false;
    static bool wasSkyIsAFloorPressed = false;
    static bool wasFullScreenTogglePressed = false;

    Uint8 *keyState = SDL_GetKeyState(NULL);

    switch (event.type) {
        case SDL_QUIT:
            gamedata.done = true;
            break;

        case SDL_KEYDOWN:
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                gamedata.done = true;
            }
            if (event.key.keysym.sym == SDLK_a)
            {
                controls.isLStrafeHeld = true;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                controls.isRStrafeHeld = true;
            }
            if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
            {
                controls.isDownHeld = true;
            }
            if(event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
            {
                controls.isUpHeld = true;
            }
            if(event.key.keysym.sym == SDLK_LEFT)
            {
                controls.isLeftHeld = true;
            }
            if(event.key.keysym.sym == SDLK_RIGHT)
            {
                controls.isRightHeld = true;
            }
            if(event.key.keysym.sym == SDLK_LSHIFT)
            {
                gamedata.walkingSpeed = defWalkingSpeed * 2;
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
                controls.shouldStarsBeRendered = false;
            }
            if(event.key.keysym.sym == SDLK_INSERT) {
                controls.shouldStarsBeRendered = true;
            }
            if(event.key.keysym.sym == SDLK_PAGEDOWN) {
                 if(!wasSkyIsAFloorPressed) {
                    controls.isFloorASky = !controls.isFloorASky;
                    wasSkyIsAFloorPressed = true;
                 }
            }
            if(event.key.keysym.sym == SDLK_F4) {
                if(!wasFullScreenTogglePressed) {
                    if(!controls.isFullScreen) {
                        *screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
                        controls.isFullScreen = true;
                    } else {
                        *screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
                        controls.isFullScreen = false;
                    }
                wasFullScreenTogglePressed = true;
                }
            }
            if(event.key.keysym.sym == SDLK_c) {
                gamedata.horizonLine = 0;
            }
            if(keyState[SDLK_F6] && keyState[SDLK_F9]) {
                controls.easterEgg = !controls.easterEgg;
            }
            if(event.key.keysym.sym == SDLK_F12) {
                controls.textureGradient = !controls.textureGradient;
            }
            if(event.key.keysym.sym == SDLK_F11) {
                if(controls.naiveApproach) {
                    controls.naiveApproach = false;
                    blockBitSize = nonNaiveBBS;
                    horisontalBlockCheckStep = nonNaiveBBS * 2;
                } else {
                    controls.naiveApproach = true;
                    blockBitSize = naiveBlockBitSize;
                    horisontalBlockCheckStep = naiveBlockBitSize;
                }
            }
            if(event.key.keysym.sym == SDLK_F2) {
                currentRes = clamp(currentRes - 1, 0, resArraySize-1);
                changeResolution(screen, resolutions[currentRes], gamedata);
            }
            if(event.key.keysym.sym == SDLK_F3) {
                currentRes = clamp(currentRes + 1, 0, resArraySize-1);
                changeResolution(screen, resolutions[currentRes], gamedata);
            }
            if(event.key.keysym.sym == SDLK_F5) {
                if(!controls.vSync) {
                    targetFPS = 60;
                    controls.vSync = true;
                    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
                } else {
                    targetFPS = 1000;
                    controls.vSync = false;
                    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
                }
            }
            if(event.key.keysym.sym == SDLK_F10) {
                controls.night = !controls.night;
            }
            if(event.key.keysym.sym == SDLK_F9) {
                controls.texturedSky = !controls.texturedSky;
            }
            if(event.key.keysym.sym == SDLK_F6) {
                controls.multithreaded = !controls.multithreaded;
            }
            if(event.key.keysym.sym == SDLK_F7) {
                controls.fog = !controls.fog;
            }
            if(event.key.keysym.sym == SDLK_F8) {
                controls.coloredLight = !controls.coloredLight;
            }
            break;
        }
        case SDL_KEYUP:
        {
            if (event.key.keysym.sym == SDLK_a)
            {
                controls.isLStrafeHeld = false;
            }
            if (event.key.keysym.sym == SDLK_d)
            {
                controls.isRStrafeHeld = false;
            }
            if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
            {
                controls.isDownHeld = false;
            }
            if(event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
            {
                controls.isUpHeld = false;
            }
            if(event.key.keysym.sym == SDLK_LEFT)
            {
                controls.isLeftHeld = false;
            }
            if(event.key.keysym.sym == SDLK_RIGHT)
            {
                controls.isRightHeld = false;
            }
            if(event.key.keysym.sym == SDLK_LSHIFT)
            {
                gamedata.walkingSpeed = defWalkingSpeed;
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
            gamedata.player.angle -= rotatingSpeed * (float)(screenWidth / 2 - event.motion.x) / mouseSensitivity;
            gamedata.horizonLine += (screenHeight / 2 - event.motion.y) * ((float)screenHeight / 400);
            gamedata.horizonLine = clamp(gamedata.horizonLine, -horizonCap, horizonCap);
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT) {
                destroyAWallThatPlayerIsFacing(gamedata);
            }
            if(event.button.button == SDL_BUTTON_RIGHT) {
                createRandomColorWallNearby(gamedata);
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


void doActions(const int frameTime, GameData& gamedata, ControlState ctrls = controls) {
    using namespace Constants;
    Player& player = gamedata.player;
    float walkingSpeed = gamedata.walkingSpeed;
    float action;

    if(ctrls.isUpHeld) {
        action = sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)Constants::mapWidth))  {
            player.x -= action;
        }
        action = cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)Constants::mapHeight))  {
            player.y -= action;
        }
    }
    if(ctrls.isDownHeld) {
        action = -sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)Constants::mapWidth))  {
            player.x -= action;
        }
        action = -cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)Constants::mapHeight))  {
            player.y -= action;
        }
    }
    if(ctrls.isLeftHeld) {
        player.angle -= rotatingSpeed  * (frameTime / targetSpeed);
    }
    if(ctrls.isRightHeld) {
        player.angle += rotatingSpeed  * (frameTime / targetSpeed);
    }
    if(ctrls.isLStrafeHeld) {
        action = -sinf(player.angle + pi / 2) * walkingSpeed * (frameTime / targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)Constants::mapWidth))  {
            player.x -= action;
        }
        action = -cosf(player.angle + pi / 2) * walkingSpeed * (frameTime / targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)Constants::mapHeight))  {
            player.y -= action;
        }
    }
    if(ctrls.isRStrafeHeld) {
        action = sinf(player.angle + pi / 2) * walkingSpeed * (frameTime / targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)Constants::mapWidth))  {
            player.x -= action;
        }
        action = cosf(player.angle + pi / 2) * walkingSpeed * (frameTime / targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)Constants::mapHeight))  {
            player.y -= action;
        }
    }
}

#endif // INPUT_H_INCLUDED
