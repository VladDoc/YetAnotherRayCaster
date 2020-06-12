#ifndef RENDERING_H_INCLUDED
#define RENDERING_H_INCLUDED

#include "Vector2D.h"
#include "ControlsState.h"
#include "GameData.h"

struct GameData;

void rayTraversal(GameData& gamedata, float ray, float* distArray,
                  Vector2D<float>* rayPosArray, Vector2D<float>* eyes, ControlState& ctrls, int j = 0);



void renderColumn(float ray,  int j, SDL_Surface* screen,
                  const Vector2D<float>& test, float distanceToAWall,
                  const GameData& gamedata, const ControlState& ctrls);

#endif // RENDERING_H_INCLUDED
