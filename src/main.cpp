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
    constexpr unsigned int TILE_RESOLUTION = 8;
    constexpr unsigned int TILE_COUNT = 3;

    Image tile_spritesheet;
    std::array<Texture2D, TILE_COUNT> tile_textures;

    float delta_time;

    // STRUCTS
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
            for (auto &row : grid)
            {
                for (auto &tile : row)
                {
                    tile.type = 0;
                }
            }

            BuildGrid();
        }

        void BuildGrid()
        {
            int i = 1;
            for (auto &row : grid)
            {
                for (auto &tile : row)
                {
                    tile.type = i % 2;
                    i++;
                }
            }
        }

        void DrawGrid(std::array<Texture2D, TILE_COUNT> &tile_textures)
        {
            for (unsigned int y = 0; y < rows; y++)
            {
                for (unsigned int x = 0; x < cols; x++)
                {
                    const Tile tile = grid.at(y).at(x);
                    const Texture2D tile_texture = tile_textures.at(tile.type);

                    DrawTexture(tile_texture, x * TILE_RESOLUTION, y * TILE_RESOLUTION, WHITE);
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

    template <size_t tile_count>
    void InitTileTextures(
        std::array<Texture2D, tile_count> &tile_textures = tile_textures,
        const Image &tile_spritesheet = tile_spritesheet,
        unsigned int tile_resolution = TILE_RESOLUTION)
    {
        for (unsigned int i = 0; i < tile_count; i++)
        {
            Rectangle source_rect = {0.0f, 0.0f, (float)tile_resolution, (float)tile_resolution};
            switch (i)
            {
            case 1:
                source_rect.x = 1 * tile_count;

            case 2:
                source_rect.x = 2 * tile_count;
            }
            Image sub_image = ImageFromImage(tile_spritesheet, source_rect);
            Game::tile_textures.at(i) = LoadTextureFromImage(sub_image);
            UnloadImage(sub_image);
        }
    }

};

int main()
{
    using Game::delta_time;

    InitWindow(Game::WINDOW_HEIGHT, Game::WINDOW_HEIGHT, "raylib basic window");
    SetTargetFPS(Game::TARGET_FRAMERATE);

    Game::tile_spritesheet = LoadImage("assets/tiles.png");
    Game::InitTileTextures(Game::tile_textures);

    Game::Grid<10, 10> grid;

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

        grid.DrawGrid(Game::tile_textures);

        DrawTexture(Game::tile_textures.at(1), 0, 0, WHITE);

        DrawTextureEx(player.texture, player.position, 0.0f, 8.0f, WHITE);

        DrawText("It works!", 20, 20, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}