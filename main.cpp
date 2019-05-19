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


const float walkingSpeed = 0.2f;
const float rotatingSpeed = 0.1f;

constexpr const float FOV = pi / 4.0f;


template <typename T>
constexpr T clamp(T& value, T& min, T& max) {
    if(value > max) return max;
    else if(value < min) return min;
    else return value;
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

bool** fillUpTheMapToBeBox(int xSize = mapHeight, int ySize = mapWidth)
{
    bool* buffer = new bool[xSize * ySize];
    bool** map = new bool*[ySize];

    for(int i = 0; i < ySize; ++i) {
        map[i] = &buffer[i * xSize];
    }

    for(int i = 0; i < xSize; ++i)
    {
        map[0][i] = true;
    }

    for(int i = 0; i < ySize-2; ++i)
    {
        for(int j = 0; j < xSize; ++j)
        {
            if(j == 0 || j == xSize-1) map[i][j] = true;
            else map[i][j] = false;
        }
    }

    for(int i = 0; i < xSize; ++i)
    {
        map[ySize-1][i] = true;
    }

    return map;

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

    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        return 1;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    const bool map[mapHeight][mapWidth] = {
                            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
                                          };

    bool done = false;
    while (!done)
    {
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
                    player.angle -= rotatingSpeed;
                }
                if (event.key.keysym.sym == SDLK_d)
                {
                    player.angle += rotatingSpeed;
                }
                if (event.key.keysym.sym == SDLK_s)
                {
                    player.x -= sinf(player.angle) * walkingSpeed;
                    player.y -= cosf(player.angle) * walkingSpeed;
                    if(map[(int)player.y][(int)player.x] == 1)
                    {
                        player.x += sinf(player.angle) * walkingSpeed;
                        player.y += cosf(player.angle) * walkingSpeed;
                    }
                }
                if(event.key.keysym.sym == SDLK_w)
                {
                    player.x += sinf(player.angle) * walkingSpeed;
                    player.y += cosf(player.angle) * walkingSpeed;
                    if(map[(int)player.y][(int)player.x] == 1)
                    {
                        player.x -= sinf(player.angle) * walkingSpeed;
                        player.y -= cosf(player.angle) * walkingSpeed;
                    }
                }
                break;
            }
        }

        for(int j = 0; j < screenWidth; ++j)
        {
            float ray = (player.angle - FOV / 2.0f) + ((float)j / (float)screenWidth) * FOV;

            float distanceToAWall = 0.0f;

            Vector2D<float> eye;

            eye.x = sinf(ray);
            eye.y = cosf(ray);

            bool wasWallHit = false;

            while(!wasWallHit && distanceToAWall < depth)
            {
                distanceToAWall += 0.01f;

                int testX = (int)(player.x + eye.x * distanceToAWall);
                int testY = (int)(player.y + eye.y * distanceToAWall);

                wasWallHit = map[testY][testX]; // Apparently compiler doesn't give a crap
                                                // about your if statement, so it's the only way around.
                                                // Possible gcc bug?

                if(testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
                {
                    wasWallHit = true;
                    distanceToAWall = depth;
                }
            }

            int ceilingHeight = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToAWall);
            int floorHeight = screenHeight - ceilingHeight;

            for(int i = 0; i < screenHeight; ++i)
            {
                if(i < ceilingHeight)
                {
                    Uint32* pixel = (Uint32*)(screen->pixels + (i * screen->pitch + j * sizeof(Uint32)));
                    float ceilingDistance = 1.0f + (((float)i - screenHeight / 2.0f) / (float)screenHeight / 0.8f);
                    Uint32 shade = ColorToUint(0 / (ceilingDistance * 2),
                                               0 / (ceilingDistance * 2),
                                               60 / (ceilingDistance * 2));
                    *pixel = shade;
                }
                else if(i >= ceilingHeight && i <= floorHeight)
                {
                    Uint32 shade = ColorToUint(50 * (1 + (distanceToAWall / 8)),
                                               22 * (1 + (distanceToAWall / 8)),
                                               5 * (1 + (distanceToAWall / 8)));
                    Uint32* pixel = (Uint32*)(screen->pixels + (i * screen->pitch + j * sizeof(Uint32)));
                    *pixel = (Uint32)shade;
                }
                else
                {
                    float floorDistance = 1.0f - (((float)i - screenHeight / 3.0f) / (float)screenHeight / 0.8f);
                    Uint32 shade = ColorToUint(20 / (floorDistance * 2),
                                               20 / (floorDistance * 2),
                                               70 / (floorDistance * 2));
                    Uint32* pixel = (Uint32*)(screen->pixels + (i * screen->pitch + j * sizeof(Uint32)));
                    *pixel = (Uint32)shade;
                }
            }
        }

        SDL_Delay(1000 / 60);
        SDL_Flip(screen);
    }

return 0;
}
