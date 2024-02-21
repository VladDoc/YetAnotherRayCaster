#include "rendering.h"

#include <cmath>
#include <climits>

#include "Utility.h"
#include "RenderUtils.h"


struct RenderData
{
    Uint32 skyLightColor;
    Uint32 fogColor;
    Uint32 wallColorPixel;

    SDL_Color floorColor;
    SDL_Color skyColor;

    SDL_Surface* texture;
    SDL_Surface* lightmap;

    SDL_Surface* screen;

    float skyScaleCoef = 1.0f;
    int skyTextureIndex;
    int skyWidthIndex;
    int ceilingHeight;
    int floorHeight;

    int j {};

    int wallSizeOnScreen {};

    float distanceToAWall;
    float scalingVar;

    bool isTextured;
    bool isLightMap;
    bool shouldStarsBeRendered;
    bool shouldTextureBeMirrored;
    bool isHorisontal;

    Constants& c = Constants::get();
};


void rayTraversal(GameData& gamedata, float ray, float* distArray,
                  Vector2D<float>* rayPosArray, Vector2D<float>* eyes, ControlState& ctrls, int j)
{
        Constants& c = Constants::get();
        using namespace util;
        float distanceToAWall = 0.0f;

        Vector2D<float> eye;

        eye.x = std::sin(ray);
        eye.y = std::cos(ray);

        Vector2D<float> test;
        test.x = gamedata.player.x;
        test.y = gamedata.player.y;

        bool wasWallHit = false;

        int quarter = ray / (c.pi / 2);

        while(!wasWallHit) // Ray traversal
        {
            if(!ctrls.naiveApproach) {
                distanceToAWall += intersectDist(test,
                                   eye.x, eye.y, quarter, c.nonNaiveBBS);
            } else {
                distanceToAWall += c.naiveBlockBitSize;
            }
            test.x = gamedata.player.x + eye.x * distanceToAWall;
            test.y = gamedata.player.y + eye.y * distanceToAWall;

            if(!withinRange(test.x, 0.0f, (float)c.mapWidth) ||
               !withinRange(test.y, 0.0f, (float)c.mapHeight))
            {
                wasWallHit = 1;
                distanceToAWall = c.offMapDepth;
           } else {
                wasWallHit = !(int)gamedata.map[(int)test.y][(int)test.x].isEmpty();
            }
        }
        test.x = clamp(test.x, 0.0f, (float)c.mapWidth);
        test.y = clamp(test.y, 0.0f, (float)c.mapHeight);

        // Constant gives slightly better fish-eye correction. Without it walls are a little bit more 'rounded'
        distanceToAWall *= cosf(ray - gamedata.player.angle - (c.FOV / (c.screenWidth * 8)));

        if(distArray) distArray[j] = distanceToAWall;
        if(rayPosArray) rayPosArray[j] = test;
        if(eyes) eyes[j] = eye;
}


inline Uint32 renderCeiling(const ControlState& ctrls, const GameData& gamedata,
                            const RenderData& r_data, int i)
{
    Constants& c = r_data.c;
    using namespace RenderUtils;

    Uint32 pixelColor;
    // If star exists in the stars map for the current location set white flickering pixel
    if(ctrls.shouldStarsBeRendered &&
       gamedata.stars[(i + (c.horizonCap - gamedata.horizonLine)) * c.starsWidth + r_data.skyWidthIndex]) {
       pixelColor = getStarColorPixel();
    } else {
        // Do gradiented color sky or retrieve pixel out of sky box
        if(!ctrls.texturedSky) {
            pixelColor = getSkyGradientedColor(r_data.skyColor, i, gamedata.horizonLine);
        } else {
                pixelColor = *getTransposedScaledTexturePixel(gamedata.sky_textures[0],
                              c.starsWidth, c.starsHeight,
                              (i + (c.horizonCap - gamedata.horizonLine)),
                              r_data.skyTextureIndex);
//              pixelColor = *getSpherePixelOld(gamedata.sky_textures[0],
//                                           getSkyTextureHeight(i, gamedata.horizonLine),
//                                           gamedata.eyes[r_data.j].x,
//                                           gamedata.eyes[r_data.j].y,
//                                           i, r_data.j);
//                pixelColor = *getSpherePixel(gamedata.sky_textures[0],
//                                             getSkyTextureHeightRad(i, gamedata.horizonLine),
//                                             gamedata.rays[r_data.j]);
        }
        if(ctrls.fog) {
            pixelColor = doCeilingFog(pixelColor, r_data.fogColor, i, gamedata);
        }
    }
    return pixelColor;
}

inline Uint32 renderFloor(const ControlState& ctrls, const GameData& gamedata, const RenderData& r_data, int i)
{
    Constants& c = r_data.c;
    using namespace RenderUtils;

    Uint32 pixelColor;

    if(!ctrls.isFloorASky) {
        pixelColor = getFloorGradientedColor(r_data.floorColor, i, gamedata.horizonLine);
        if(ctrls.night) pixelColor = fastPixelShadowing(pixelColor);
        if(ctrls.fog) {
            pixelColor = doFloorFog(pixelColor, r_data.fogColor, i, gamedata);
        }
        if(ctrls.coloredLight) pixelColor = blend(pixelColor, r_data.skyLightColor, 92);
//        Uint32 skyPixel;
//
//        int indexI = starsHeight / 2 - (i + (horizonCap - gamedata.horizonLine) - starsHeight / 2);
//        int indexJ = r_data.skyTextureIndex;
//
//        if(ctrls.texturedSky) {
//            skyPixel = *getTransposedScaledTexturePixel(gamedata.sky_textures[0],
//                                                       starsWidth, starsHeight,
//                                                       indexI,
//                                                       indexJ);
//        } else {
//            skyPixel = getSkyGradientedColor(r_data.skyColor, i, gamedata.horizonLine);
//        }
//        pixelColor = blend(pixelColor, doFloorFog(skyPixel, 0x7F7F7F, i, gamedata), 64);
    } else {
        if(ctrls.shouldStarsBeRendered &&
           gamedata.stars[(i + (c.horizonCap - gamedata.horizonLine))
                           * c.starsWidth + r_data.skyWidthIndex]) {

            pixelColor = getStarColorPixel();
        } else {
            if(!ctrls.texturedSky) {
                pixelColor = getSkyGradientedColor(r_data.skyColor, i, gamedata.horizonLine);
            } else {

                pixelColor = *getTransposedScaledTexturePixel(gamedata.sky_textures[0],
                                c.starsWidth, c.starsHeight,
                                i + (c.horizonCap - gamedata.horizonLine),
                                r_data.skyTextureIndex);

//              pixelColor = *getSpherePixel(gamedata.sky_textures[0],
//                                           getSkyTextureHeightRad(i, gamedata.horizonLine),
//                                           gamedata.rays[r_data.j]);
            }
            if(ctrls.fog) {
                pixelColor = doFloorFog(pixelColor, r_data.fogColor, i, gamedata);
            }
        }
    }

    return pixelColor;
}



inline Uint32 renderWall(const ControlState& ctrls, const GameData& gamedata, const RenderData& r_data, int i)
{
    Constants& c = Constants::get();
    using namespace util;
    using namespace RenderUtils;

    Uint32 pixelColor;
    Uint32* texturePixel = NULL;
    Uint32* lightmapPixel = NULL;

    // Textured wall routine
    if(r_data.isTextured) {
        // Swaped w and h because the texture is transposed. Same with lightmaps.
        texturePixel = getTransposedTexturePixel(r_data.texture,
                (int)((i - r_data.ceilingHeight) * ((float)r_data.texture->w / (float)r_data.wallSizeOnScreen)),
                (int)(getFractialPart(r_data.scalingVar) * (float)r_data.texture->h));

            pixelColor = *texturePixel;

        if(ctrls.textureGradient) {
            pixelColor = applyWallGradientToPixel(pixelColor, r_data.distanceToAWall);
        }

        // Darkens walls for an illusion of directed light
        if(whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::WEST   ||
           whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::NORTH    ) {
            // Doing fast pixel transformation for the texture, so the performance won't suffer
                pixelColor = fastPixelShadowing(pixelColor);
        }

    } else {
        pixelColor = r_data.wallColorPixel;
    }
    if(r_data.isLightMap) {
            // If current wall is lightmapped(on runtime) then transform pixel accordingly.
            // Very slow so it's never used.
            lightmapPixel = getTransposedTexturePixel(r_data.lightmap,
                    (int)((i - r_data.ceilingHeight) * ((float)r_data.lightmap->w /
                    (float)r_data.wallSizeOnScreen)),
                    (int)(getFractialPart(r_data.scalingVar) * (float)r_data.lightmap->h));

            SDL_Color texColor = UintToColor(pixelColor);
            SDL_Color lightColor = UintToColor(*lightmapPixel);
            SDL_Color finalColor = transformColorByLightMap(texColor, lightColor);

            pixelColor = ColorToUint(finalColor.r, finalColor.g, finalColor.b);
        }

     // If night mode enabled darken pixel even more
    if(ctrls.night) {
            pixelColor = fastPixelShadowing(pixelColor);
    }

    if(ctrls.fog) {
        pixelColor = blend(pixelColor, r_data.fogColor,
                     (Uint8)clamp(r_data.distanceToAWall * 12, 0.0f, 255.0f));
    }

    if(ctrls.coloredLight) pixelColor = blend(pixelColor, r_data.skyLightColor, 92);

    return pixelColor;
}

void renderColumn(float ray,  int j, SDL_Surface* screen,
                  const Vector2D<float>& test, float distanceToAWall,
                  const GameData& gamedata, const ControlState& ctrls)
{
    using namespace util;
    using namespace RenderUtils;

    SDL_Color wallColor;
    RenderData r_data;

    Constants& c = r_data.c;

    r_data.distanceToAWall = distanceToAWall;
    r_data.skyScaleCoef = cosf(gamedata.player.angle - ray);

    r_data.floorColor = c.floorColor;
    r_data.skyColor = c.skyColor;
    r_data.screen = screen;
    r_data.j = j;

    if(!ctrls.easterEgg) {
        r_data.ceilingHeight = (float)(c.screenHeight / 2.0) -
                                c.screenHeight / ((float)distanceToAWall);
    } else {
        r_data.ceilingHeight = (float)(c.screenHeight / 2.0) -
                                c.screenHeight / ((float)distanceToAWall) +
                                abs(j  - c.screenWidth / 2);
    }

    r_data.floorHeight = c.screenHeight - r_data.ceilingHeight;

    r_data.ceilingHeight += gamedata.horizonLine;
    r_data.floorHeight += gamedata.horizonLine;

    r_data.wallSizeOnScreen = r_data.floorHeight - r_data.ceilingHeight;

    r_data.floorHeight = clamp(r_data.floorHeight, 0, INT_MAX);

    float bufferRay = clampLooping(ray, 0.0f, c.pi * 2);
    r_data.skyTextureIndex = clamp((int)(c.starsWidth * (bufferRay / (c.pi * 2))), 0, c.starsWidth-1);
    r_data.skyWidthIndex = (int)(c.screenWidth * (bufferRay / c.FOV));

    if(ctrls.texturedSky) {
        r_data.skyLightColor = gamedata.avgSkyColor;
    } else {
        r_data.skyLightColor = ColorToUint(c.skyColor.r,
                                           c.skyColor.g, c.skyColor.b);
    }

    if(ctrls.night) {
        r_data.fogColor = blend(fastPixelShadowing(r_data.skyLightColor),
                                c.nightFogColor, 127);
    } else {
        r_data.fogColor = c.dayFogColor;
    }

    if(!withinRange(test.x, 0.0f, (float)c.mapWidth) ||
       !withinRange(test.y, 0.0f, (float)c.mapHeight)) {
        wallColor = MapBlock::defWallColor;
        distanceToAWall = c.offMapDepth;
    } else {
        wallColor = gamedata.map[(int)test.y][(int)test.x].getColor();
        r_data.ceilingHeight -= (r_data.floorHeight - r_data.ceilingHeight) *
                                gamedata.map[(int)test.y][(int)test.x].height -
                                (r_data.floorHeight - r_data.ceilingHeight);


        r_data.shouldTextureBeMirrored = false;
        r_data.isHorisontal = false;

        float checkY = test.y;

        if(gamedata.map[(int)(checkY - c.horisontalBlockCheckStep)][(int)test.x].isEmpty() ||
           gamedata.map[(int)(checkY + c.horisontalBlockCheckStep)][(int)test.x].isEmpty()   ) {
                r_data.isHorisontal = true;
                r_data.scalingVar = test.x; // Then wall is along horizontal axis
                if(getFractialPart(test.y) > 0.5f) { // If y component greater than a half then it's a north wall
                    r_data.shouldTextureBeMirrored = true;
                }
        } else {
                r_data.scalingVar = test.y;
                if(getFractialPart(test.x) < 0.5f) {
                    r_data.shouldTextureBeMirrored = true;
                }
        }

        MapBlock currentBlock = gamedata.map[(int)test.y][(int)test.x];

        r_data.texture = nullptr;
        r_data.lightmap = nullptr;

        r_data.isTextured = currentBlock.getIsTextured();
        r_data.isLightMap = currentBlock.getIsLightMapped();



        if(!r_data.shouldTextureBeMirrored) {
            // Sometimes if works with false boolean, which causes game to segfault, to prevent that I clamp index.
            if(r_data.isTextured) r_data.texture =
                RenderUtils::chooseMip(
                    gamedata.textures[clamp(currentBlock.getTextureIndex(), 0, (int)gamedata.textures.size()-1)],
                    distanceToAWall
                );
            if(r_data.isLightMap) r_data.lightmap =
                RenderUtils::chooseMip(
                    gamedata.lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)gamedata.lightmaps.size()-1)],
                    distanceToAWall
                );
        } else {
            if(r_data.isTextured) r_data.texture =
                RenderUtils::chooseMip(
                    gamedata.m_textures[clamp(currentBlock.getTextureIndex(), 0, (int)gamedata.m_textures.size()-1)],
                    distanceToAWall
                );
            if(r_data.isLightMap) r_data.lightmap =
                RenderUtils::chooseMip(
                    gamedata.m_lightmaps[clamp(currentBlock.getLightMapIndex(), 0, (int)gamedata.m_lightmaps.size()-1)],
                    distanceToAWall
                );
        }
    }

    // Non textured wall routine
        if(whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::WEST  ||
           whichSide(r_data.shouldTextureBeMirrored, r_data.isHorisontal) == SideOfAWall::NORTH    )
        {
            r_data.wallColorPixel = getShadowedWallColor(wallColor, distanceToAWall);
        } else {
            r_data.wallColorPixel = getGradientedWallColor(wallColor, distanceToAWall);
        }

    for(int i = 0; i < c.screenHeight; ++i)
    {
        Uint32* pixel = getTexturePixel(screen, i, j);
        Uint32 pixelColor;
        if(i < r_data.ceilingHeight)
        {
            pixelColor = renderCeiling(ctrls, gamedata, r_data, i);
        }
        else if(i >= r_data.ceilingHeight && i < r_data.floorHeight)
        {
            pixelColor = renderWall(ctrls, gamedata, r_data, i);
        }
        else
        {
            pixelColor = renderFloor(ctrls, gamedata, r_data, i);
        }
        *pixel = pixelColor;
    }
}
