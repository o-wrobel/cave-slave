#include <raylib.h>
#include <iostream>
#include <array>

// #include "game.h"

#define SAFE_CALL(expr)                             \
    try                                             \
    {                                               \
        expr;                                       \
    }                                               \
    catch (const std::exception &e)                 \
    {                                               \
        std::cerr << "Error: " << e.what() << '\n'; \
    }

namespace Game
{

    // CONSTANTS
    constexpr unsigned int WINDOW_WIDTH = 800;
    constexpr unsigned int WINDOW_HEIGHT = 600;
    constexpr unsigned int TARGET_FRAMERATE = 60;
    const unsigned int TILE_SIZE = 32;

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

        Grid() : grid()
        {
            for (auto& row : grid)
            {
                for (auto& tile : row)
                {
                    tile.type = 0;
                }
            }

            BuildGrid();
        }

        void BuildGrid()
        {
            int i = 1;
            for (auto& row : grid)
            {
                for (auto& tile : row)
                {
                    tile.type = i % 2;
                    i++;
                }
            }
        }

        void DrawGrid()
        {
            for (unsigned int y = 0; y < rows; y++)
            {
                for (unsigned int x = 0; x < cols; x++)
                {
                    const Tile tile = grid.at(y).at(x);
                    switch (tile.type)
                    {
                    case 1:
                        DrawRectangle(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
                        break;

                    case 2:
                        DrawRectangle(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    };

    class Player
    {
    public:
        Texture2D texture;
        Vector2 position;
        float speed;

        Player(Texture2D texture) : texture(texture), position({0.0f, 0.0f}), speed(400.0f)
        {
        }
    };

    float delta_time;

};

int main()
{
    using Game::delta_time;

    InitWindow(Game::WINDOW_HEIGHT, Game::WINDOW_HEIGHT, "raylib basic window");
    SetTargetFPS(Game::TARGET_FRAMERATE);

    Texture2D player_texture;

    Game::Grid<10, 10> grid;

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

        grid.DrawGrid();

        DrawTextureEx(player.texture, player.position, 0.0f, 8.0f, WHITE);

        DrawText("It works!", 20, 20, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}