#pragma once

#include <raylib.h>

class Game
{
private:
    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    const unsigned int TARGET_FRAMERATE = 60;

    float delta_time;

    Texture2D player_texture;

    public:
        Game(unsigned int WINDOW_WIDTH, unsigned int WINDOW_HEIGHT, unsigned int TARGET_FRAMERATE);
};

class Player
{
public:
    Texture2D texture;
    Vector2 position;
    float speed;

    Player(Texture2D texture);
};