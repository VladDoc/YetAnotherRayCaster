config = {

	map = "maze.txt";

	textures = {
		tex1 = "wall2.bmp",
    	tex2 = "wall.bmp",
    	tex3 = "wall3.bmp",
    	tex4 = "wall2.bmp",
    	tex5 = "hellaworld.bmp";
	};

	texturesSize = 5;

	lightmaps = {
		tex1 = "wall2bumpmap.bmp";
	};

	lightmapsSize = 1;

	skyTextures = {
		tex1 = "sky.bmp";
	};

	skyTexturesSize = 1;
    
	player = {
         X = 2,
         Y = 2,
         angle = 3.14 / 2; -- in radians
	}; -- Those will be ignored if a map has a starting point(except for angle)

	findPath = false;
	time_at_each_cell = 0.5; -- in seconds. Will be ignored if findPath = false.


	-- Game has built in resolutions, but if you need to mess around feel free to change those.
	-- Game has a limit of 180 degree fov, so if aspect ratio is too wide it will cause image to be stretched
	screenWidth = 840;
    screenHeight = 480;
    screenBits = 32;

    walkingSpeed = 0.2;
    rotatingSpeed = 0.1; -- speed of camera rotate

    targetSpeed = 40.0; -- This is a constant that defines game speed of action(walking, etc)

    targetFPS = 1000; -- 1000 is max(internal clock in milliseconds)

    mouseSensitivity = 20.0; -- works opposite way. bigger value means less sensitivity.


    -- You might change a sky color into random one by pressing Page Up
    -- Home key will restore it to default
    -- and End key will make it black
    skyColor = {
    	R = 0,
    	G = 20,
    	B = 100
	};

    floorColor = {
    	R = 0,
    	G = 90,
    	B = 30
    };

    dayFogColor = {
    	R = 127,
    	G = 127,
    	B = 127
    };

    nightFogColor = {
    	R = 0, 
		G = 0, 
		B = 0
	};


    -- Controls in the game are in comments.
    textureGradient = false; -- F12(very slow. leftover from the dark ages)
    shouldStarsBeRendered = true; -- insert/del
    texturedSky = false; -- F9
    isFloorASky = false; -- Page Down
    isFullScreen = false; -- F4
    naiveApproach = false; -- F11
    vSync = false; -- F5
    night = true; -- F10
    multithreaded = true; -- F6(it will occupy all hardware threads in your system)

    -- having them both on reduces speed by ~40%
    fog = false; -- F7
    coloredLight = true; -- F8

}