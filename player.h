#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

struct Player
{
    float x;
    float y;
    float angle;
    Player() = default;
    Player(float xx, float yy, float a): x(xx), y(yy), angle(a) {};
} player(2.0f, 2.0f, pi / 2);


#endif // PLAYER_H_INCLUDED
