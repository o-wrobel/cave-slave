#include <raylib.h>
#include <iostream>
#include <array>

#include "game.h"

template <size_t rows, size_t cols>

constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;
constexpr unsigned int TARGET_FRAMERATE = 60;

int main()
{

    Texture2D my_texture;

    InitWindow(WINDOW_HEIGHT, WINDOW_HEIGHT, "raylib basic window");
    SetTargetFPS(TARGET_FRAMERATE);

    float delta_time;

    Texture2D player_texture;
    try
    {
        player_texture = LoadTexture("assets/player.png");
    }
    catch (const std::exception &e)
    {
        std::printf("Failed to load texture: %s\n", e.what());
    }

    Player player(player_texture);

    

    struct Tile
    {
        unsigned int type;
    };

    std::array<std::array<Tile, 10>, 10> grid;

    grid.at(0).at(0).type = 1;
    grid.at(1).at(1).type = 1;

    void DrawTiles(std::array<std::array<Tile, 10>, 10> grid);

    while (!WindowShouldClose())
    {
        // UPDATE

        delta_time = GetFrameTime();

        player.position.x += player.speed * delta_time * (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
        player.position.y -= player.speed * delta_time * (IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN));

        // DRAW
        BeginDrawing();
            ClearBackground(BLACK);

            DrawTextureEx(player.texture, player.position, 0.0f, 8.0f, WHITE);

            DrawText("It works!", 20, 20, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}