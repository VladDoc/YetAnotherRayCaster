#include "Input.h"

#include <cmath>

#include "Utility.h"
#include "rendering.h"

void destroyAWallThatPlayerIsFacing(GameData& gamedata, ControlState& ctrls)
{
    Constants& c = Constants::get();
    float distanceToAWall = 0.0f;

    Vector2D<float> test;
    test.x = gamedata.player.x;
    test.y = gamedata.player.y;

    rayTraversal(gamedata, gamedata.player.angle,
                 &distanceToAWall, &test, nullptr, ctrls);

    if(util::withinRange((float)test.x, 0.0f, (float)c.mapWidth) &&
       util::withinRange((float)test.y, 0.0f, (float)c.mapHeight)) {
            gamedata.map[(int)test.y][(int)test.x].setEmpty();
    }
}

void createRandomColorWallNearby(GameData& d)
{
    Constants& c = Constants::get();
    Vector2D<int> wallLocation;
    wallLocation.x = (int)(d.player.x + sinf(d.player.angle) * 3.0f);
    wallLocation.y = (int)(d.player.y + cosf(d.player.angle) * 3.0f);

    wallLocation.x = util::clamp(wallLocation.x, 0, c.mapWidth-1);
    wallLocation.y = util::clamp(wallLocation.y, 0, c.mapHeight-1);

    if(d.map[wallLocation.y][wallLocation.x].isEmpty()) {
        d.map[wallLocation.y][wallLocation.x].r = rand() % 64;
        d.map[wallLocation.y][wallLocation.x].g = rand() % 64;
        d.map[wallLocation.y][wallLocation.x].b = rand() % 64;
    }
}

void changeResolution(SDL_Surface** screen, const Vector2D<int> res,
                       GameData& gamedata, ControlState& controls)
{
    Constants& c = Constants::get();
    if(controls.isFullScreen) {
        *screen = SDL_SetVideoMode(res.x, res.y, c.screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_FULLSCREEN);
    } else {
        *screen = SDL_SetVideoMode(res.x, res.y, c.screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
    }
    c.screenWidth = res.x;
    c.screenHeight = res.y;

    c.horizonCap = c.calcHorizonCap();

    c.starsWidth = c.calcStarsWidth();
    c.starsHeight = c.calcStarsHeight();

    c.FOV = c.calcFOV();
    gamedata.allocateScreenSizeSensitiveData();
}

void checkControls(const SDL_Event& event, SDL_Surface** screen,
                   GameData& gamedata, ControlState& controls)
{
    Constants& c = Constants::get();
    using namespace util;

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
                gamedata.walkingSpeed = c.defWalkingSpeed * 2;
            }
            if(event.key.keysym.sym == SDLK_PAGEUP)
            {
                if(!wasSkyColorChangePressed)
                {
                    c.skyColor.r = rand() % 64;
                    c.skyColor.g = rand() % 64;
                    c.skyColor.b = rand() % 64;
                    wasSkyColorChangePressed = true;
                }
            }
            if(event.key.keysym.sym == SDLK_HOME)
            {
                c.skyColor = c.defSkyColor;
            }
            if(event.key.keysym.sym == SDLK_END)
            {
                c.skyColor.r = 0;
                c.skyColor.g = 0;
                c.skyColor.b = 0;
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
                        *screen = SDL_SetVideoMode(c.screenWidth, c.screenHeight,
                                                   c.screenBits, SDL_DOUBLEBUF |
                                                   SDL_HWSURFACE | SDL_FULLSCREEN);
                        controls.isFullScreen = true;
                    } else {
                        *screen = SDL_SetVideoMode(c.screenWidth, c.screenHeight,
                                                c.screenBits, SDL_DOUBLEBUF | SDL_HWSURFACE);
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
                    c.blockBitSize = c.nonNaiveBBS;
                    c.horisontalBlockCheckStep = c.nonNaiveBBS * 2;
                } else {
                    controls.naiveApproach = true;
                    c.blockBitSize = c.naiveBlockBitSize;
                    c.horisontalBlockCheckStep = c.naiveBlockBitSize;
                }
            }
            if(event.key.keysym.sym == SDLK_F2) {
                c.currentRes = clamp(c.currentRes - 1, 0, c.resArraySize-1);
                changeResolution(screen, c.resolutions[c.currentRes], gamedata, controls);
            }
            if(event.key.keysym.sym == SDLK_F3) {
                c.currentRes = clamp(c.currentRes + 1, 0, c.resArraySize-1);
                changeResolution(screen, c.resolutions[c.currentRes], gamedata, controls);
            }
            if(event.key.keysym.sym == SDLK_F5) {
                if(!controls.vSync) {
                    c.targetFPS = 60;
                    controls.vSync = true;
                    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
                } else {
                    c.targetFPS = 1000;
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
                gamedata.walkingSpeed = c.defWalkingSpeed;
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
            gamedata.player.angle -= c.rotatingSpeed * (float)(c.screenWidth / 2 - event.motion.x) / c.mouseSensitivity;
            gamedata.horizonLine += (c.screenHeight / 2 - event.motion.y) * ((float)c.screenHeight / 400);
            gamedata.horizonLine = clamp(gamedata.horizonLine,
                                        -(c.horizonCap),
                                         (c.horizonCap));
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event.button.button == SDL_BUTTON_LEFT) {
                destroyAWallThatPlayerIsFacing(gamedata, controls);
            }
            if(event.button.button == SDL_BUTTON_RIGHT) {
                createRandomColorWallNearby(gamedata);
            }
            if(event.button.button == SDL_BUTTON_WHEELDOWN) {
                c.FOV -= 0.05f;
                c.FOV = clamp(c.FOV, c.pi / 8, c.pi - 0.01f);
            }
            if(event.button.button == SDL_BUTTON_WHEELUP) {
                c.FOV += 0.05f;
                c.FOV = clamp(c.FOV, c.pi / 8, c.pi - 0.01f);
            }
        }
        SDL_WarpMouse(c.screenWidth / 2, c.screenHeight / 2);
}


void doActions(const int frameTime, GameData& gamedata, ControlState& ctrls) {
    Constants& c = Constants::get();
    using namespace util;

    Player& player = gamedata.player;
    float walkingSpeed = gamedata.walkingSpeed;
    float action;

    if(ctrls.isUpHeld) {
        action = sinf(player.angle) * walkingSpeed * (frameTime / c.targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)c.mapWidth))  {
            player.x -= action;
        }
        action = cosf(player.angle) * walkingSpeed * (frameTime / c.targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)c.mapHeight))  {
            player.y -= action;
        }
    }
    if(ctrls.isDownHeld) {
        action = -sinf(player.angle) * walkingSpeed * (frameTime / c.targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)c.mapWidth))  {
            player.x -= action;
        }
        action = -cosf(player.angle) * walkingSpeed * (frameTime / c.targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)c.mapHeight))  {
            player.y -= action;
        }
    }
    if(ctrls.isLeftHeld) {
        player.angle -= c.rotatingSpeed  * (frameTime / c.targetSpeed);
    }
    if(ctrls.isRightHeld) {
        player.angle += c.rotatingSpeed  * (frameTime / c.targetSpeed);
    }
    if(ctrls.isLStrafeHeld) {
        action = -sinf(player.angle + c.pi / 2) * walkingSpeed * (frameTime / c.targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)c.mapWidth))  {
            player.x -= action;
        }
        action = -cosf(player.angle + c.pi / 2) * walkingSpeed * (frameTime / c.targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)c.mapHeight))  {
            player.y -= action;
        }
    }
    if(ctrls.isRStrafeHeld) {
        action = sinf(player.angle + c.pi / 2) * walkingSpeed * (frameTime / c.targetSpeed);
        player.x += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.x, 0.0f, (float)c.mapWidth))  {
            player.x -= action;
        }
        action = cosf(player.angle + c.pi / 2) * walkingSpeed * (frameTime / c.targetSpeed);
        player.y += action;
        if(!gamedata.map[(int)player.y][(int)player.x].isEmpty() ||
           !withinRange(player.y, 0.0f, (float)c.mapHeight))  {
            player.y -= action;
        }
    }
}
