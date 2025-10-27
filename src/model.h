#pragma once

#include <raylib.h>
#include <stdint.h>
#include <vector>

typedef struct Vector2u {
    uint32_t x;
    uint32_t y;

    Vector2 ToVector2() const;
} Vector2u;

Vector2u Vector2uAdd(Vector2u a, Vector2u b);
Vector2u Vector2uScale(Vector2u a, float scale);

struct Input {
    struct Pressed{
        bool space;
        bool escape;
        bool y;
        bool n;
        bool f4;
        bool f5;
        bool f6;
    };

    struct Held{
        bool ctrl;
        bool right, left, up, down, space;
        bool lmb, rmb;
    };

    Vector2 mouse_position;
    float mouse_wheel;
    Held held;
    Pressed pressed;

    static Input Capture();

};

enum GameMode{
    EDITOR,
    PLAY
};

struct Assets{
    Image tile_spritesheet;
    std::vector<Texture2D> tile_textures;

    Texture2D player_texture;

};

typedef enum Direction{
    RIGHT = 1,
    LEFT = -1
} Direction;

typedef struct Sprite {
    Texture2D texture;
    Rectangle dest_rect;
    Direction direction = RIGHT;

    void Draw() const;

} Sprite;
