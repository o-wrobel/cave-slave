#pragma once

#include <raylib.h>
#include <stdint.h>
#include <vector>

typedef struct Vector2u {
    uint32_t x;
    uint32_t y;
} Vector2u;

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
