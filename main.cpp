#include <cstdlib>

#include <iostream>
#include <ctime>
#include <cmath>

#include <SDL/SDL.h>

#include <stdio.h>


const int screenWidth = 1024;
const int screenHeight = 600;
const int screenBits = 32;

const int mapHeight = 16;
const int mapWidth = 16;

const float pi = 3.14159f;
constexpr const float depth = sqrt(pow(mapHeight, 2) + pow(mapWidth, 2));

const float defWalkingSpeed = 0.2f;
float walkingSpeed = 0.2f;
const float rotatingSpeed = 0.1f;

const float mouseSensitivity = 20.0f; // Works the opposite way. The bigger the value the less actual sensitivity gets.

constexpr const float FOV = pi / 4.0f;

bool isUpHeld = false;
bool isDownHeld = false;
bool isLeftHeld = false;
bool isRightHeld = false;
bool isLStrafeHeld = false;
bool isRStrafeHeld = false;


template <typename T>
inline T clamp(T value, T min, T max) {
    if(value > max) {
            return max;
    }
    if(value < min) {
            return min;
    }

    return value;
}

Uint32 ColorToUint(int R, int G, int B)
{
	return (Uint32)((R << 16) + (G << 8) + (B << 0));
}

SDL_Color UintToColor(Uint32 color)
{
	SDL_Color retColor;

	retColor.unused = 255; // Alpha

	retColor.r = (color >> 16) & 0xFF; // Takes second byte
	retColor.g = (color >> 8) & 0xFF; // Takes third byte
	retColor.b = color & 0xFF; // Takes last one

	return retColor;
}


Uint32 map[mapHeight][mapWidth] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, ColorToUint(20, 0, 50), ColorToUint(20, 0, 50), ColorToUint(20, 0, 50), ColorToUint(20, 0, 50), ColorToUint(20, 0, 50), ColorToUint(20, 0, 50), 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, ColorToUint(50, 0, 0), ColorToUint(50, 0, 0), ColorToUint(50, 0, 0), ColorToUint(50, 0, 0), ColorToUint(50, 0, 0), 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

bool stars[screenHeight / 2][screenWidth];

Uint32 defWallColor = ColorToUint(50, 20, 0);

struct Player
{
    float x;
    float y;
    float angle;
    Player() = default;
    Player(float xx, float yy, float a): x(xx), y(yy), angle(a) {};
} player(2.0f, 2.0f, pi / 2);


template <typename T>
struct Vector2D
{
    T x;
    T y;
};

void doActions() {
    if(isUpHeld) {
        player.x += sinf(player.angle) * walkingSpeed;
        player.y += cosf(player.angle) * walkingSpeed;
        if(map[(int)player.y][(int)player.x] == 1)  {
            player.x -= sinf(player.angle) * walkingSpeed;
            player.y -= cosf(player.angle) * walkingSpeed;
        }
    }
    if(isDownHeld) {
        player.x -= sinf(player.angle) * walkingSpeed;
        player.y -= cosf(player.angle) * walkingSpeed;
        if(map[(int)player.y][(int)player.x] == 1)  {
            player.x += sinf(player.angle) * walkingSpeed;
            player.y += cosf(player.angle) * walkingSpeed;
        }
    }
    if(isLeftHeld) {
        player.angle -= rotatingSpeed;
    }
    if(isRightHeld) {
        player.angle += rotatingSpeed;
    }
    if(isLStrafeHeld) {
        player.x -= sinf(player.angle + pi / 2) * walkingSpeed;
        player.y -= cosf(player.angle + pi / 2) * walkingSpeed;
        if(map[(int)player.y][(int)player.x] == 1)  {
            player.x += sinf(player.angle + pi / 2) * walkingSpeed;
            player.y += cosf(player.angle + pi / 2) * walkingSpeed;
        }
    }
    if(isRStrafeHeld) {
        player.x += sinf(player.angle + pi / 2) * walkingSpeed;
        player.y += cosf(player.angle + pi / 2) * walkingSpeed;
        if(map[(int)player.y][(int)player.x] == 1)  {
            player.x -= sinf(player.angle + pi / 2) * walkingSpeed;
            player.y -= cosf(player.angle + pi / 2) * walkingSpeed;
        }
    }
}

void fillUpTheMapToBeBox()
{
    for(int i = 0; i < mapWidth; ++i)
    {
        map[0][i] = true;
    }

    for(int i = 1; i < mapHeight-1; ++i)
    {
        for(int j = 0; j < mapWidth; ++j)
        {
            if(j == 0 || j == mapWidth-1) map[i][j] = true;
            else map[i][j] = false;
        }
    }

    for(int i = 0; i < mapWidth; ++i)
    {
        map[mapHeight-1][i] = true;
    }
}

void fillUpTheStars() {
    srand(1);
    for(int i = 0; i < screenHeight / 2; ++i) {
        for(int j = 0; j < screenWidth; ++j) {
            if(!(rand() % 256)) {
                stars[i][j] = true;
            }
        }
    }
}


int main( int argc, char** argv )
{
    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit(SDL_Quit);
    SDL_putenv("SDL_VIDEO_WINDOW_POS=center");
    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        return 1;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    //bool** map = fillUpTheMapToBeBox();

    fillUpTheStars();
    SDL_ShowCursor(SDL_DISABLE);
    bool done = false;
    int count = 0;
    while (!done)
    {
        int start = SDL_GetTicks();
        SDL_Event event;
        SDL_PollEvent(&event);

        switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;

            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
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
                if(event.key.keysym.sym == SDLK_LEFT) {
                    isLeftHeld = true;
                }
                if(event.key.keysym.sym == SDLK_RIGHT) {
                    isRightHeld = true;
                }
                if(event.key.keysym.sym == SDLK_LSHIFT) {
                    walkingSpeed = defWalkingSpeed * 2;
                }
                break;
                }
                case SDL_KEYUP: {
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
                if(event.key.keysym.sym == SDLK_LEFT) {
                    isLeftHeld = false;
                }
                if(event.key.keysym.sym == SDLK_RIGHT) {
                    isRightHeld = false;
                }
                if(event.key.keysym.sym == SDLK_LSHIFT) {
                    walkingSpeed = defWalkingSpeed;
                }
                break;
                }
                case SDL_MOUSEMOTION:
                player.angle -= rotatingSpeed * (float)(screenWidth / 2 - event.motion.x) / mouseSensitivity;
                break;
            }
        SDL_WarpMouse(screenWidth / 2, screenHeight / 2);
        doActions();
        for(int j = 0; j < screenWidth; ++j)
        {
            float ray = (player.angle - FOV / 2.0f) + ((float)j / (float)screenWidth) * FOV;

            float distanceToAWall = 0.0f;

            Vector2D<float> eye;

            eye.x = sinf(ray);
            eye.y = cosf(ray);

            int wasWallHit = 0;
            SDL_Color wallColor;

            while(!wasWallHit && distanceToAWall < depth)
            {
                distanceToAWall += 1.0f / 64.0f;

                Vector2D<int> test;
                test.x = (int)(player.x + eye.x * distanceToAWall);
                test.y = (int)(player.y + eye.y * distanceToAWall);

                if(test.x < 0 || test.x >= mapWidth || test.y < 0 || test.y >= mapHeight)
                {
                    wasWallHit = 1;
                    distanceToAWall = depth;
                    wallColor = UintToColor(defWallColor);
                }   else {
                    wasWallHit = (int)map[test.y][test.x]; // Apparently compiler doesn't give a crap
                                                           // about your if statement, so it's the only way around.
                                                           // Possible gcc bug?
                    if(wasWallHit == 1) {
                        wallColor = UintToColor(defWallColor);
                    } else {
                        wallColor = UintToColor(map[test.y][test.x]);
                    }
                }
            }

            int ceilingHeight = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToAWall);
            int floorHeight = screenHeight - ceilingHeight;

            for(int i = 0; i < screenHeight; ++i)
            {
                if(i < ceilingHeight)
                {
                    //float ceilingDistance = 1.0f + (((float)i - screenHeight / 2.0f) / (float)screenHeight / 0.8f);
                    Uint32 shade;
                    if(stars[i][j]) {
                        shade = ColorToUint(clamp(rand() % 256, 170, 255),
                                            clamp(rand() % 256, 170, 255),
                                            clamp(rand() % 256, 170, 255));
                    } else {
                        shade = ColorToUint(clamp((int)(0  * (float)(i + 64) / 128), 0, 255),
                                            clamp((int)(10 * (float)(i + 64) / 128), 0, 255),
                                            clamp((int)(50 * (float)(i + 64) / 128), 0, 255));
                    }
                    Uint32* pixel = (Uint32*)(screen->pixels + (i * screen->pitch + j * sizeof(Uint32)));
                    *pixel = shade;
                }
                else if(i >= ceilingHeight && i < floorHeight)
                {
                    Uint32 shade = ColorToUint(clamp((int)(wallColor.r * (1 + (distanceToAWall / 3))), 0, 255),
                                               clamp((int)(wallColor.g * (1 + (distanceToAWall / 3))), 0, 255),
                                               clamp((int)(wallColor.b * (1 + (distanceToAWall / 3))), 0, 255));

                    Uint32* pixel = (Uint32*)(screen->pixels + (i * screen->pitch + j * sizeof(Uint32)));
                    *pixel = (Uint32)shade;
                }
                else
                {

                    Uint32 shade = ColorToUint(clamp((int)(0  * (float)(screenHeight - i + 128) / 128), 0, 200),
                                               clamp((int)(50 * (float)(screenHeight - i + 128) / 128), 0, 200),
                                               clamp((int)(20 * (float)(screenHeight - i + 128) / 128), 0, 200));

                    Uint32* pixel = (Uint32*)(screen->pixels + (i * screen->pitch + j * sizeof(Uint32)));
                    *pixel = (Uint32)shade;
                }
            }
        }
        int end = SDL_GetTicks();
        //SDL_Delay(1000 / 60 - clamp((int)(end - start), 0, 1000 / 60));
        char fps[128];
        sprintf(fps, "%d", 1000 / clamp(end - start, 1, INT_MAX));
        if(count == 16) {
            SDL_WM_SetCaption(fps, NULL);
            count = 0;
        }
        SDL_Flip(screen);
        ++count;
    }

return 0;
}
