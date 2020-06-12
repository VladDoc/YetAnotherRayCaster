#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include <SDL/SDL.h>

#include "GameData.h"
#include "ControlsState.h"

void destroyAWallThatPlayerIsFacing(GameData& gamedata, ControlState& ctrls);

void createRandomColorWallNearby(GameData& d);

void changeResolution(SDL_Surface** screen, const Vector2D<int> res,
                       GameData& gamedata, ControlState& controls);

void checkControls(const SDL_Event& event, SDL_Surface** screen,
                   GameData& gamedata, ControlState& controls);


void doActions(const int frameTime, GameData& gamedata, ControlState& ctrls);

#endif // INPUT_H_INCLUDED
