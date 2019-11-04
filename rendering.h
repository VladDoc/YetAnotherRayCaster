#pragma once

#ifndef RENDERING_H_INCLUDED
#define RENDERING_H_INCLUDED

#include "Utility.h"

void rayTraversal(float ray, float* distArray, Vector2D<float>* rayPosArray, int j = 0)
{
        float distanceToAWall = 0.0f;

        Vector2D<float> eye;

        eye.x = sinf(ray);
        eye.y = cosf(ray);

        Vector2D<float> test;
        test.x = player.x;
        test.y = player.y;

        int wasWallHit = 0;

        while(!wasWallHit) // Ray traversal
        {
            distanceToAWall += getDistanceToTheNearestIntersection(test, ray);

            test.x = player.x + eye.x * distanceToAWall;
            test.y = player.y + eye.y * distanceToAWall;

            if(!withinRange(test.x, 0.0f, (float)mapWidth) ||
               !withinRange(test.y, 0.0f, (float)mapHeight))
            {
                wasWallHit = 1;
                distanceToAWall = offMapDepth;
           } else {
                wasWallHit = !(int)map[(int)test.y][(int)test.x].isEmpty();
            }
        }
//        test.x = clamp(test.x, 0.0f, (float)mapWidth);
//        test.y = clamp(test.y, 0.0f, (float)mapHeight);

        // Constant gives slightly better fish-eye correction. Without it walls are a little bit more 'rounded'
        distanceToAWall *= cosf(ray - player.angle - (FOV / (screenWidth * 8)));

        if(distArray) distArray[j] = distanceToAWall;
        if(rayPosArray) rayPosArray[j] = test;
}


#endif // RENDERING_H_INCLUDED
