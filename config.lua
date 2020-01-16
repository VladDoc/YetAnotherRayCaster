config {
	    
	player = {
         X = 2,
         Y = 2,
	};

	map = "maze.txt";

	screenWidth = 800;
    screenHeight = 480;
    screenBits = 32;

    walkingSpeed = 0.2;
    rotatingSpeed = 0.1;

    targetSpeed = 40.0;

    targetFPS = 1000; -- 1000 is max

    mouseSensitivity = 20.0; -- works opposite way. bigger value means less sensitivity.


    skyColor = {0, 20, 100};

    floorColor = {0, 90, 30};

    dayFogColor = {127, 127, 127};
    nightFogColor = {0, 0, 0};


    textureGradient = false;
    shouldStarsBeRendered = false;
    texturedSky = true;
    isFloorASky = false;
    isFullScreen = false;
    naiveApproach = false;
    vSync = false;
    night = false;
    multithreaded = true;

    fog = false;
    coloredLight = true;

}