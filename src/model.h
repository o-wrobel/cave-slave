#pragma once

#include <raylib.h>
#include <vector>

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
