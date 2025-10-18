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
    struct Config {
        static constexpr unsigned int WINDOW_WIDTH = 800;
        static constexpr unsigned int WINDOW_HEIGHT = 600;
        static constexpr unsigned int TARGET_FRAMERATE = 60;

        static constexpr unsigned int TILE_RESOLUTION = 8;
        static constexpr unsigned int TILE_COUNT = 8;

        static constexpr unsigned int GRID_SIZE_X = 32;
        static constexpr unsigned int GRID_SIZE_Y = 32;

        static constexpr bool ZOOM_INTO_MOUSE = false;

        static constexpr unsigned int TILE_SIZE = TILE_RESOLUTION * TILE_COUNT;
    };

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
    struct Grid{
        std::array<std::array<Tile, cols>, rows> tiles;

        void Place(unsigned int x, unsigned int y, unsigned int type){
            tiles.at(y).at(x).type = type;
        }

        Tile GetTile(unsigned int x, unsigned int y){
            return tiles.at(y).at(x);
        }

        static Grid NewBox(){
            Grid grid;
            for (int y = 0; y < rows; y++)
            {
                grid.Place(0, y, 6);
            }
            for (int x = 0; x < cols; x++)
            {
                grid.Place(x, 0, 6);
            }
            for (int y = 0; y < rows; y++)
            {
                grid.Place(cols - 1, y, 6);
            }
            for (int x = 0; x < cols; x++)
            {
                grid.Place(x, rows - 1, 6);
            }

            return grid;
        }

    };

    template <size_t rows, size_t cols>
    void RenderGrid(Grid<rows, cols> grid, std::array<Texture2D, Config::TILE_COUNT> &tile_textures)
    {
        for (unsigned int y = 0; y < rows; y++)
        {
            for (unsigned int x = 0; x < cols; x++)
            {
                const Tile tile = grid.GetTile(x, y);
                const Texture2D tile_texture = tile_textures.at(tile.type);

                DrawTexture(tile_texture, x * Config::TILE_RESOLUTION, y * Config::TILE_RESOLUTION, WHITE);
            }
        }
    }

    namespace Player {
        struct State {
            Vector2 position = {0.0f, 0.0f};
            Vector2 size = {8.0f, 8.0f};
            float move_speed = 400.0f;

            State MoveBy(Vector2 offset) const {
                Vector2 new_position = {position.x + offset.x, position.y + offset.y};
                return {new_position, size, move_speed};
            }

            Vector2 GetCenter() const {
                return {position.x + size.x / 2, position.y + size.y / 2};
            }
            static State New(
                        Vector2 position,
                        Vector2 size = {8.0f, 8.0f},
                        float move_speed = 400.0f){
                        return {position, size, move_speed};
                    }

            static State Update(
                State& current_state,
                const Input& input,
                float deltaTime)
            {
                // Calculate movement based on input
                float horizontal = (input.right ? 1.0f : 0.0f) - (input.left ? 1.0f : 0.0f);
                float vertical = (input.up ? 1.0f : 0.0f) - (input.down ? 1.0f : 0.0f);

                Vector2 offset = {
                    horizontal * current_state.move_speed * deltaTime,
                    -vertical * current_state.move_speed * deltaTime
                };

                return current_state.MoveBy(offset);

            }
        };

        void Render(const State& state, const Texture2D& texture){
            DrawTextureV(texture, state.position, WHITE);
        }
    }

    // NON-CONSTANTS
    Image tile_spritesheet;
    std::array<Texture2D, Config::TILE_COUNT> tile_textures;

    auto grid = Grid<Config::GRID_SIZE_X, Config::GRID_SIZE_Y>::NewBox();

    Camera2D camera;

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
            tile_textures.at(i) = LoadTextureFromImage(sub_image);
            UnloadImage(sub_image);
        }
    }

    template <size_t grid_size_x, size_t grid_size_y>
    void Init(){
        InitWindow(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "raylib basic window");
        SetTargetFPS(Config::TARGET_FRAMERATE);

        tile_spritesheet = LoadImage("assets/tiles.png");
        InitTileTextures(tile_textures, tile_spritesheet, Config::TILE_RESOLUTION);

        camera.offset = {0.0f, 0.0f};
        camera.rotation = 0.0f;
        camera.zoom = 2.0f;
    }

}


int main(){
    using Game::delta_time;

    Game::Init<32, 32>();

    Game::Player::State player = Game::Player::State::New({0.0f, 0.0f});
    Texture2D player_texture;
    SAFE_CALL(player_texture = LoadTexture("assets/player.png"));

    using Game::camera;
    while (!WindowShouldClose()){
       // UPDATE
        delta_time = GetFrameTime();
        const Game::Input input = Game::Input::Capture();

        player = Game::Player::State::Update(player, input, delta_time);

        float wheel = GetMouseWheelMove();
        if (wheel != 0){
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            if (Game::Config::ZOOM_INTO_MOUSE){
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
        camera.offset = {Game::Config::WINDOW_WIDTH / 2, 1.f * Game::Config::WINDOW_HEIGHT / 2};


        // DRAW
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(Game::camera);

        Game::RenderGrid(Game::grid, Game::tile_textures);

        Game::Player::Render(player , player_texture);
        DrawText("It works!", 32, 32, 32, WHITE);

        EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
