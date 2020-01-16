#pragma once

#ifndef CONTROLSSTATE_H_INCLUDED
#define CONTROLSSTATE_H_INCLUDED
struct ControlState
{
    bool isUpHeld = false;
    bool isDownHeld = false;
    bool isLeftHeld = false;
    bool isRightHeld = false;
    bool isLStrafeHeld = false;
    bool isRStrafeHeld = false;

    bool textureGradient = false;
    bool shouldStarsBeRendered = false;
    bool texturedSky = true;
    bool isFloorASky = false;
    bool isFullScreen = false;
    bool easterEgg = false;
    bool naiveApproach = false;
    bool vSync = false;
    bool night = false;
    bool multithreaded = true;

    bool fog = false;
    bool coloredLight = true;

    bool logging = false;
};
#endif // CONTROLSSTATE_H_INCLUDED
