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


namespace Game{
    // CONSTANTS
    constexpr unsigned int WINDOW_WIDTH = 800;
    constexpr unsigned int WINDOW_HEIGHT = 600;
    constexpr unsigned int TARGET_FRAMERATE = 60;

    constexpr unsigned int TILE_RESOLUTION = 8;
    constexpr unsigned int TILE_COUNT = 8;

    const unsigned int grid_size_x = 32;
    const unsigned int grid_size_y = 32;

    constexpr bool ZOOM_INTO_MOUSE = false;

    struct Input {
        const bool right, left, up, down;

        static Input Capture()
        {
            return Input{
                IsKeyDown(KEY_D),
                IsKeyDown(KEY_A),
                IsKeyDown(KEY_W),
                IsKeyDown(KEY_S)
            };
        }
    };
    // STRUCTS
    struct Tile {
        unsigned int type;
    };

    template <size_t rows, size_t cols>
    class Grid{
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

            void PlaceTile(unsigned int x, unsigned int y, unsigned int type)
            {
                grid.at(y).at(x).type = type;
            }

            void BuildGrid()
            {
                for (int y = 0; y < rows; y++)
                {
                    PlaceTile(0, y, 6);
                }
                for (int x = 0; x < cols; x++)
                {
                    PlaceTile(x, 0, 6);
                }
                for (int y = 0; y < rows; y++)
                {
                    PlaceTile(cols - 1, y, 6);
                }
                for (int x = 0; x < cols; x++)
                {
                    PlaceTile(x, rows - 1, 6);
                }

                PlaceTile(9, 9, 7);

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

    class Player {
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
        std::array<Texture2D, tile_count> &tile_textures,
        const Image &tile_spritesheet,
        unsigned int tile_resolution)
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
        Game::InitTileTextures(Game::tile_textures, Game::tile_spritesheet, Game::TILE_RESOLUTION);

        camera.offset = {0.0f, 0.0f};
        camera.rotation = 0.0f;
        camera.zoom = 2.0f;
    }

    Vector2 UpdatePlayerPosition(
        Vector2 currentPos,
        const Input& input,
        float speed,
        float deltaTime)
    {
        Vector2 newPos = currentPos;

        // Calculate movement based on input
        float horizontal = (input.right ? 1.0f : 0.0f) - (input.left ? 1.0f : 0.0f);
        float vertical = (input.up ? 1.0f : 0.0f) - (input.down ? 1.0f : 0.0f);

        newPos.x += horizontal * speed * deltaTime;
        newPos.y -= vertical * speed * deltaTime;  // Y is inverted in your original

        return newPos;
    }
};


int main(){
    using Game::delta_time;

    Game::Init<32, 32>();

    Texture2D player_texture;
    SAFE_CALL(player_texture = LoadTexture("assets/player.png"));
    Game::Player player(player_texture);

    using Game::camera;
    while (!WindowShouldClose()){
       // UPDATE
        delta_time = GetFrameTime();
        const Game::Input input = Game::Input::Capture();

        player.position = Game::UpdatePlayerPosition(player.position, input, player.speed, delta_time);
        float wheel = GetMouseWheelMove();
        if (wheel != 0){
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            if (Game::ZOOM_INTO_MOUSE){
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
