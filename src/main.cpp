#include <raylib.h>
#include <iostream>
#include <array>

// #include "game.h"

#define SAFE_CALL(expr) \
    try { expr; } \
    catch (const std::exception& e) { \
        std::cerr << "Error: " << e.what() << '\n'; \
    }

namespace Game
{
    constexpr unsigned int WINDOW_WIDTH = 800;
    constexpr unsigned int WINDOW_HEIGHT = 600;
    constexpr unsigned int TARGET_FRAMERATE = 60;

    struct Tile
    {
        unsigned int type;
    };

    template <size_t rows, size_t cols>
    class Grid
    {
    private:
    public:
        std::array<std::array<Tile, cols>, rows> grid;
    };

    class Player
    {
    public:
        Texture2D texture;
        Vector2 position;
        float speed;

        Player(Texture2D texture) : texture(texture), position({0.0f, 0.0f}), speed(200.0f)
        {
        }
    };
};



int main()
{

    InitWindow(Game::WINDOW_HEIGHT, Game::WINDOW_HEIGHT, "raylib basic window");
    SetTargetFPS(Game::TARGET_FRAMERATE);

    float delta_time;

    Texture2D player_texture;

    SAFE_CALL(player_texture = LoadTexture("assets/player.png"));
    Game::Player player(player_texture);

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