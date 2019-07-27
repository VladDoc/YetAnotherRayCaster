#include <cstdlib>

#include <iostream>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <vector>

#include <SDL/SDL.h>


//#define TEXTURE_GRADIENT 1

const int screenWidth = 640;
const int screenHeight = 480;
const int screenBits = 32;

const int mapHeight = 16;
const int mapWidth = 16;

const float pi = 3.14159f;
constexpr const float depth = sqrt(pow(mapHeight, 2) + pow(mapWidth, 2));

const float defWalkingSpeed = 0.2f;
float walkingSpeed = 0.2f;
const float rotatingSpeed = 0.1f;

const float mouseSensitivity = 20.0f; // Works the opposite way. The bigger the value the less actual sensitivity gets.

constexpr const float FOV = pi / (6.4f * ((float)screenHeight / (float)screenWidth));

const float targetSpeed = 40.0f;

const float blockSize = 64.0f;
const float blockBitSize = 1.0f / blockSize;
const int targetFPS = 60;

bool isUpHeld = false;
bool isDownHeld = false;
bool isLeftHeld = false;
bool isRightHeld = false;
bool isLStrafeHeld = false;
bool isRStrafeHeld = false;

bool shouldStarsBeRendered = true;
bool isFloorASky = false;
bool isFullScreen = false;

bool done = false;


std::vector<SDL_Surface*> textures;
std::vector<SDL_Surface*> lightmaps;

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

inline float clampLooping(float value, float min, float max) {
    if(value > max) {
        return min + (max - fmod(value, max));
    }
    if(value < min) {
        return max - (min - fmod(value, max));
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

float getFractialPart(float arg) {
    int wholePart = (int)arg;
    return arg - wholePart;
}



int getBiggerNearestInt(float arg) {
    return (int)arg + 1;
}

SDL_Color defSkyColor;
SDL_Color skyColor;

class MapBlock
{
public:
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint16 texture;
    Uint16 lightmap;
    MapBlock(int red, int green, int blue) :
        r((Uint8)red), g((Uint8)green), b((Uint8)blue), texture(0), lightmap(0) { }

    MapBlock(int textureType) : texture((Uint16)textureType), lightmap(0) { }

    MapBlock(int textureType, int lightmapType) :
        texture((Uint16)textureType), lightmap((Uint16)lightmapType) { }

    inline bool getIsTextured() {
        return texture >= 2;
    }

    inline bool getIsLightMapped() {
        return lightmap >= 1;
    }

    void setDefault() {
        r = 0;
        g = 0;
        b = 0;
        texture = 1;
        lightmap = 0;
    }

    void setEmpty() {
        r = 0;
        g = 0;
        b = 0;
        texture = 0;
        lightmap = 0;
    }

    inline int getTextureIndex() {
        return (int)(texture)-2;
    }

    inline int getLightMapIndex() {
        return (int)(lightmap)-1;
    }

    SDL_Color getColor(){
        SDL_Color retColor;
        retColor.r = r;
        retColor.g = g;
        retColor.b = b;
        return retColor;
    }

    inline bool isEmpty() {
        return !(r || g || b || texture);
    }
};

MapBlock map[mapHeight][mapWidth] =
{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {1, 0, 0, 0, 0, 0, 0, 0, MapBlock(0, 0, 50), MapBlock(0, 0, 50), MapBlock(20, 0, 50), MapBlock(20, 0, 50), MapBlock(20, 0, 50), MapBlock(20, 0, 50), 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, MapBlock(50, 0, 0), MapBlock(50, 0, 0), MapBlock(50, 0, 0), MapBlock(50, 0, 0), MapBlock(50, 0, 0), 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const int starsWidth = screenWidth * (int)((pi * 2) / FOV);
const int starsHeight = screenHeight;
bool stars[starsHeight][starsWidth];

Uint32 defWallColor = ColorToUint(45, 20, 0);

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

void doActions(int frameTime) {
    if(isUpHeld) {
        player.x += sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.x -= sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
        player.y += cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.y -= cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
    }
    if(isDownHeld) {
        player.x -= sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.x += sinf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        }
        player.y -= cosf(player.angle) * walkingSpeed * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
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
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.x += sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
        player.y -= cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.y += cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
    }
    if(isRStrafeHeld) {
        player.x += sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.x -= sinf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
        player.y += cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        if(!map[(int)player.y][(int)player.x].isEmpty())  {
            player.y -= cosf(player.angle + pi / 2) * walkingSpeed  * (frameTime / targetSpeed);
        }
    }
}

void fillUpTheMapToBeBox(MapBlock** aMap)
{
    for(int i = 0; i < mapWidth; ++i)
    {
        aMap[0][i].setDefault();
    }

    for(int i = 1; i < mapHeight-1; ++i)
    {
        for(int j = 0; j < mapWidth; ++j)
        {
            if(j == 0 || j == mapWidth-1) aMap[i][j].setDefault();
            else aMap[i][j].setEmpty();
        }
    }

    for(int i = 0; i < mapWidth; ++i)
    {
        map[mapHeight-1][i].setDefault();
    }
}

void fillUpTheStars() {
    srand(1);
    for(int i = 0; i < starsHeight; ++i) {
        for(int j = 0; j < starsWidth; ++j) {
            if(!(rand() % 256)) {
                stars[i][j] = true;
            }
        }
    }
}

void loadTexture(std::vector<SDL_Surface*>& txt, const char* filename)
{
    SDL_Surface* surf = SDL_LoadBMP(filename);
    SDL_Surface* texture = SDL_DisplayFormat(surf);
    if(!surf || !texture) {
        printf("Unable to load textures. Exiting.....");
        system("PAUSE");
        exit(-1);
    }
    txt.push_back(texture);
}

inline Uint32* getTexturePixel(SDL_Surface* surf, int i, int j) {
    return (Uint32*)(surf->pixels + i * surf->pitch + j * sizeof(Uint32));
}

void loadTextures() {
    loadTexture(textures, "Desert.bmp");
    loadTexture(textures, "wall.bmp");
}

void loadLightmaps() {
    loadTexture(lightmaps, "checkerBoard.bmp");
}

SDL_Color transformColorByLightMap(SDL_Color color, const SDL_Color lightmapColor) {
        color.r = clamp(color.r + lightmapColor.r - 128, 0, 255);
        color.g = clamp(color.g + lightmapColor.g - 128, 0, 255);
        color.b = clamp(color.b + lightmapColor.b - 128, 0, 255);

        return color;
}

void freeTextures() {
    for(auto i = textures.begin(); i != textures.end(); i++) {
        SDL_FreeSurface(*i);
    }
    for(auto i = lightmaps.begin(); i != lightmaps.end(); i++) {
        SDL_FreeSurface(*i);
    }
}

void checkControls(SDL_Event event, SDL_Surface* screen) {
    static bool wasSkyColorChangePressed = false;
    static bool wasSkyIsAFloorPressed = false;
    static bool wasFullScreenTogglePressed = false;

    switch (event.type)
        {
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
            break;
        }
}


void renderColumn(int j, SDL_Surface* screen) {

        float ray = (player.angle - FOV / 2.0f) + ((float)j / (float)screenWidth) * FOV;

        float distanceToAWall = 0.0f;

        Vector2D<float> eye;

        eye.x = sinf(ray);
        eye.y = cosf(ray);

        Vector2D<float> test;

        int wasWallHit = 0;
        SDL_Color wallColor;

        while(!wasWallHit && distanceToAWall < depth) // Ray traversal
        {
            distanceToAWall += blockBitSize;


            test.x = player.x + eye.x * distanceToAWall;
            test.y = player.y + eye.y * distanceToAWall;

            if(test.x < 0 || test.x >= mapWidth || test.y < 0 || test.y >= mapHeight)
            {
                wasWallHit = 1;
                distanceToAWall = depth;
                wallColor = UintToColor(defWallColor);
           } else {
                wasWallHit = !(int)map[(int)test.y][(int)test.x].isEmpty();


                if(map[(int)test.y][(int)test.x].texture == 1) {
                    wallColor = UintToColor(defWallColor);
                } else {
                    wallColor = map[(int)test.y][(int)test.x].getColor();
                }
            }
        }

        distanceToAWall *= cosf(ray - player.angle - (FOV / (screenWidth * 8))); // You might ask: 'why 8?'. But it seems to give best results yet.

        int ceilingHeight = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToAWall);// + abs(j  - screenWidth / 2);
        int floorHeight = screenHeight - ceilingHeight;

        float bufferRay = clampLooping(ray, 0.0f, pi * 2);
        int skyWidthIndex = (int)(screenWidth * (bufferRay / FOV));

        for(int i = 0; i < screenHeight; ++i)
        {
            if(i < ceilingHeight)
            {
                Uint32 shade;
                if(shouldStarsBeRendered && stars[i][skyWidthIndex]) {
                    shade = ColorToUint(clamp(rand() % 256, 165, 255),
                                        clamp(rand() % 256, 165, 255),
                                        clamp(rand() % 256, 165, 255));
                } else {
                    shade = ColorToUint(clamp((int)(skyColor.r * (float)(i + 64) / 128), 0, 255),
                                        clamp((int)(skyColor.g * (float)(i + 64) / 128), 0, 255),
                                        clamp((int)(skyColor.b * (float)(i + 64) / 128), 0, 255));
                }
                Uint32* pixel = getTexturePixel(screen, i, j);
                *pixel = shade;
            }
            else if(i >= ceilingHeight && i < floorHeight)
            {
                int wallSizeOnScreen = floorHeight - ceilingHeight;
                Uint32* pixel = getTexturePixel(screen, i, j);
                Uint32 shade;
                MapBlock currentBlock = map[(int)test.y][(int)test.x];

                    if(currentBlock.getIsTextured()) {
                    bool isHorisontal = false;
                    bool isLightMap = currentBlock.getIsLightMapped();
                    float checkY = test.y;
                    SDL_Surface* texture = textures.at(currentBlock.getTextureIndex());
                    SDL_Surface* lightmap = NULL;
                    if(isLightMap) {
                        lightmap = lightmaps.at(currentBlock.getLightMapIndex());
                    }

                    Uint32* texturePixel = &defWallColor;
                    Uint32* lightmapPixel;

                    if(map[(int)(checkY - 1.0f / 128.0f)][(int)test.x].isEmpty() ||
                       map[(int)(checkY + 1.0f / 128.0f)][(int)test.x].isEmpty()   ) {
                            isHorisontal = true;
                        }
                    if(isHorisontal) {
                            texturePixel = getTexturePixel(texture, (int)(i - ceilingHeight) * ((float)texture->h / (float)wallSizeOnScreen),
                                                        (int)(getFractialPart(test.x) * (float)texture->w));

                        if(isLightMap) {
                            lightmapPixel = getTexturePixel(lightmap, (int)(i - ceilingHeight) * ((float)lightmap->h / (float)wallSizeOnScreen),
                                                        (int)(getFractialPart(test.x) * (float)lightmap->w));
                        }
                    } else {
                            texturePixel = getTexturePixel(texture, (int)(i - ceilingHeight) * ((float)texture->h / (float)wallSizeOnScreen),
                                                        (int)(getFractialPart(test.y) * (float)texture->w));

                        if(isLightMap) {
                            lightmapPixel = getTexturePixel(lightmap, (int)(i - ceilingHeight) * ((float)lightmap->h / (float)wallSizeOnScreen),
                                                        (int)(getFractialPart(test.y) * (float)lightmap->w));
                        }
                    }
                    #ifdef TEXTURE_GRADIENT
                    SDL_Color pixelRGB = UintToColor(*texturePixel);

                    shade = ColorToUint(clamp((int)((pixelRGB.r / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.r / 3, clamp((int)(pixelRGB.r * 1.2), 0, 255)),
                                        clamp((int)((pixelRGB.g / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.g / 3, clamp((int)(pixelRGB.g * 1.2), 0, 255)),
                                        clamp((int)((pixelRGB.b / 3) * (distanceToAWall * 16) / 32), (int)pixelRGB.b / 3, clamp((int)(pixelRGB.b * 1.2), 0, 255)));
                    #else
                    if(isLightMap) {
                        SDL_Color texColor = UintToColor(*texturePixel);
                        SDL_Color lightColor = UintToColor(*lightmapPixel);
                        SDL_Color finalColor = transformColorByLightMap(texColor, lightColor);

                        shade = ColorToUint(finalColor.r, finalColor.g, finalColor.b);
                    } else {
                        shade = *texturePixel;
                    }
                    #endif
                } else {
                shade = ColorToUint(clamp((int)(wallColor.r * (distanceToAWall * 16) / 32), (int)wallColor.r, 255),
                                    clamp((int)(wallColor.g * (distanceToAWall * 16) / 32), (int)wallColor.g, 255),
                                    clamp((int)(wallColor.b * (distanceToAWall * 16) / 32), (int)wallColor.b, 255));
                }
                *pixel = shade;
            }
            else
            {
                Uint32 shade;
                if(!isFloorASky) {
                    shade = ColorToUint(clamp((int)(0  * (float)(screenHeight - i + 128) / 128), 0, 200),
                                        clamp((int)(50 * (float)(screenHeight - i + 128) / 128), 0, 200),
                                        clamp((int)(20 * (float)(screenHeight - i + 128) / 128), 0, 200));
                } else {
                    if(shouldStarsBeRendered && stars[i][skyWidthIndex]) {
                        shade = ColorToUint(clamp(rand() % 256, 165, 255),
                                            clamp(rand() % 256, 165, 255),
                                            clamp(rand() % 256, 165, 255));
                    } else {
                        shade = ColorToUint(clamp((int)(skyColor.r * (float)(i + 64) / 128), 0, 255),
                                            clamp((int)(skyColor.g * (float)(i + 64) / 128), 0, 255),
                                            clamp((int)(skyColor.b * (float)(i + 64) / 128), 0, 255));
                    }
                }
                Uint32* pixel = getTexturePixel(screen, i, j);
                *pixel = (Uint32)shade;
            }
        }
}

int main(int argc, char** argv)
{

    defSkyColor.r = 0;
    defSkyColor.g = 10;
    defSkyColor.b = 50;

    skyColor = defSkyColor;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    atexit(SDL_Quit);

    char env[80];
    sprintf(env, "SDL_VIDEO_WINDOW_POS=%d,%d", (SDL_GetVideoInfo()->current_w - screenWidth) / 2, 27);

    SDL_putenv(env);
    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, screenBits,
                                           SDL_HWSURFACE | SDL_DOUBLEBUF);

    if (!screen)
    {
        printf("Unable to set %dx%dx%d video mode: %s\n", screenWidth, screenHeight, screenBits, SDL_GetError());
        return 1;
    }

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    loadTextures();
    loadLightmaps();

    if(textures.empty() || lightmaps.empty())
    {
        printf("\nCould not load textures.\n");
        system("PAUSE");
        exit(-1);
    }

    char fps[80];
    int frameTime = 20;
    fillUpTheStars();
    SDL_ShowCursor(SDL_DISABLE);
    int count = 0;

    while (!done)
    {
        int start = SDL_GetTicks();
        SDL_Event event;
        SDL_PollEvent(&event);

        checkControls(event, screen);

        SDL_WarpMouse(screenWidth / 2, screenHeight / 2);
        doActions(frameTime);

        for(int j = 0; j < screenWidth; ++j)
        {
            renderColumn(j, screen);
        }

        int end = SDL_GetTicks();
        frameTime = end - start;

        if(frameTime < (1000 / targetFPS))
        {
            SDL_Delay((1000 / targetFPS) - frameTime);
        }

        end = SDL_GetTicks();
        frameTime = end - start;

        if(count == 16)
        {
            sprintf(fps, "%d", 1000 / clamp(end - start, 1, INT_MAX));
            SDL_WM_SetCaption(fps, NULL);
            count = 0;
        }
        SDL_Flip(screen);
        ++count;
    }

    freeTextures();
    SDL_free(screen);
    return 0;
}
