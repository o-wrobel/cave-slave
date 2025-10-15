#include <raylib.h>
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>

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

namespace Input
{
    bool right_pressed;
    bool left_pressed;
    bool up_pressed;
    bool down_pressed;

    void Update()
    {
        right_pressed = IsKeyDown(KEY_A);
        left_pressed = IsKeyDown(KEY_D);
        up_pressed = IsKeyDown(KEY_W);
        down_pressed = IsKeyDown(KEY_S);
    }
}

namespace Game
{
    // CONSTANTS
    constexpr unsigned int WINDOW_WIDTH = 800;
    constexpr unsigned int WINDOW_HEIGHT = 600;
    constexpr unsigned int TARGET_FRAMERATE = 60;

    constexpr unsigned int TILE_RESOLUTION = 8;
    constexpr unsigned int TILE_COUNT = 4;

    const unsigned int grid_size_x = 32;
    const unsigned int grid_size_y = 32;

    constexpr bool ZOOM_INTO_MOUSE = false;

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
            for (int y = 0; y < rows; y++)
            {
                grid.at(y).at(0).type = 3;
            }
            for (int x = 0; x < cols; x++)
            {
                grid.at(0).at(x).type = 3;
            }
            for (int y = 0; y < rows; y++)
            {
                grid.at(y).at(cols - 1).type = 1;
            }
            for (int x = 0; x < cols; x++)
            {
                grid.at(rows - 1).at(x).type = 2;
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
        Vector2 size;
        float speed;

        Player(Texture2D texture) : texture(texture), position({0.0f, 0.0f}), size({8.0f, 8.0f}), speed(400.0f)
        {
        }

        Vector2 GetCenter()
        {
            return {position.x + size.x / 2, position.y + size.y / 2};
        }
    };

    // NON-CONSTANTS
    Image tile_spritesheet;
    std::array<Texture2D, TILE_COUNT> tile_textures;

    Grid<grid_size_x, grid_size_y> grid;

    Camera2D camera = {0};

    float delta_time;

    // FUNCTIONS
    template <size_t tile_count>
    void InitTileTextures(
        std::array<Texture2D, tile_count> &tile_textures = tile_textures,
        const Image &tile_spritesheet = tile_spritesheet,
        unsigned int tile_resolution = TILE_RESOLUTION)
    {

        Rectangle source_rect = {0.0f, 0.0f, (float)tile_resolution, (float)tile_resolution};
        for (unsigned int i = 0; i < tile_count; i++)
        {
            source_rect.x = i * tile_resolution % (tile_spritesheet.width);
            source_rect.y = floor(i / (tile_spritesheet.width / tile_resolution)) * tile_resolution;

            Image sub_image = ImageFromImage(tile_spritesheet, source_rect);
            Game::tile_textures.at(i) = LoadTextureFromImage(sub_image);
            UnloadImage(sub_image);
        }
    }

    template <size_t grid_size_x, size_t grid_size_y>
    void Init()
    {
        InitWindow(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, "raylib basic window");
        SetTargetFPS(Game::TARGET_FRAMERATE);

        Game::tile_spritesheet = LoadImage("assets/tiles.png");
        Game::InitTileTextures(Game::tile_textures);

        camera.offset = {0.0f, 0.0f};
        camera.rotation = 0.0f;
        camera.zoom = 2.0f;
    }

};

int main()
{
    using Game::delta_time;

    Game::Init<32, 32>();

    Texture2D player_texture;
    SAFE_CALL(player_texture = LoadTexture("assets/player.png"));
    Game::Player player(player_texture);

    while (!WindowShouldClose())
    {
        // UPDATE
        delta_time = GetFrameTime();

        player.position.x += player.speed * delta_time * (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
        player.position.y -= player.speed * delta_time * (IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN));

        using Game::camera;
        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            if (Game::ZOOM_INTO_MOUSE)
            {
                // Set the offset to where the mouse is
                camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point
                // under the cursor to the screen space point under the cursor at any zoom
                camera.target = mouseWorldPos;

            }

            // Zoom increment
            // Uses log scaling to provide consistent zoom speed
            float scale = 0.2f * wheel;
            camera.zoom = std::clamp(std::expf(std::logf(camera.zoom) + scale), 1 / 8.f, 64.0f);
        }
        camera.target = player.GetCenter();
        camera.offset = {Game::WINDOW_WIDTH / 2, 1.f * Game::WINDOW_HEIGHT / 2};


        // DRAW
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(Game::camera);

        Game::grid.DrawGrid(Game::tile_textures);

        DrawTextureEx(player.texture, player.position, 0.0f, 1.0f, WHITE);

        DrawText("It works!", 32, 32, 32, WHITE);

        EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}