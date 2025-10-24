#include "game.h"

#include <string>
#include <algorithm>
#include <cmath>

namespace Game {

Input Input::Capture()
{
    return Input{
        GetMousePosition(),
        GetMouseWheelMove(),

        Input::Held{
            .ctrl = IsKeyDown(KEY_LEFT_CONTROL),
            .right = IsKeyDown(KEY_D),
            .left = IsKeyDown(KEY_A),
            .up = IsKeyDown(KEY_W),
            .down = IsKeyDown(KEY_S),
            .space = IsKeyDown(KEY_SPACE),
            .lmb = IsMouseButtonDown(0),
            .rmb = IsMouseButtonDown(1)
        },

        Input::Pressed{IsKeyPressed(KEY_SPACE)}
    };
}

//LEVEL
namespace Level {

    Grid::Grid(size_t width, size_t height) :
        size_x(width),
        size_y(height),
        tiles(height, std::vector<Tile>(width))
    {

    }

    void Grid::Place(unsigned int x, unsigned int y, unsigned int type){
        if (0 <= x && x < size_x && 0 <= y && y < size_y){
            tiles.at(y).at(x).type = type;

        }
    }

    Tile Grid::GetTile(unsigned int x, unsigned int y) const { //TODO: Maybe make this a reference idk
        return tiles.at(y).at(x);
    }


    Grid Grid::NewDefault(unsigned int width, unsigned int height){
        Level::Grid grid(width, height);
        for (int y = 0; y < height; y++)
        {
            grid.Place(0, y, 6);
        }
        for (int x = 0; x < width; x++)
        {
            grid.Place(x, 0, 6);
        }
        for (int y = 0; y < height; y++)
        {
            grid.Place(width - 1, y, 6);
        }
        for (int x = 0; x < width; x++)
        {
            grid.Place(x, height - 1, 6);
        }

        return grid;
    }


    void Render(const Level::Grid& grid, const std::vector<Texture2D>& tile_textures){
        for (unsigned int y = 0; y < grid.size_y; y++)
        {
            for (unsigned int x = 0; x < grid.size_x; x++)
            {
                const Texture2D tile_texture = tile_textures.at(grid.GetTile(x, y).type);

                DrawTexture(tile_texture, x * Config::TILE_RESOLUTION, y * Config::TILE_RESOLUTION, WHITE);
            }
        }
    }
}

// PLAYER
//
namespace Player {
    void State::MoveBy(Vector2 offset) {
        Vector2 new_position = {position.x + offset.x, position.y + offset.y};
        position.x += offset.x;
        position.y += offset.y;
    }

    Vector2 State::GetCenter() const {
        return {position.x + size.x / 2, position.y + size.y / 2};
    }

    State State::New(
        Vector2 position,
        Vector2 size,
        float move_speed
    )
    {
        return {position, size, move_speed};
    }

    void State::Update(
        const Input& input,
        float delta_time)
    {
        // Calculate movement based on input
        float horizontal = (input.held.right ? 1.0f : 0.0f) - (input.held.left ? 1.0f : 0.0f);
        float vertical = (input.held.up ? 1.0f : 0.0f) - (input.held.down ? 1.0f : 0.0f);

        Vector2 offset = {
            horizontal * move_speed * delta_time,
            -vertical * move_speed * delta_time
        };

        MoveBy(offset);

    }

    void Render(const State& state, const Texture2D& texture){
        DrawTextureV(texture, state.position, WHITE);
    }
}

//CAMERA
namespace Camera{
    Camera2D State::ToCamera2D() const {
        return {offset, target, rotation, zoom};
    }

    void State::UpdateZoom(float mouse_wheel_input, bool ctrl_held){
        if (mouse_wheel_input != 0 && ctrl_held){
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), ToCamera2D());

            if (Config::ZOOM_INTO_MOUSE){
                // Set the offset to where the mouse is
                offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point
                // under the cursor to the screen space point under the cursor at any zoom
                target = mouseWorldPos;

            }

            // Zoom increment
            // Uses log scaling to provide consistent zoom speed
            float scale = 0.2f * mouse_wheel_input;
            zoom = std::clamp(std::expf(std::logf(zoom) + scale), 1 / 8.f, 64.0f);

        }
    }

    void State::UpdatePosition(Vector2 player_center, float window_width, float window_height){
        target = player_center;
        offset = {window_width / 2, window_height / 2};
    }

    void State::Update(Vector2 player_center, Input input, float window_width, float window_height){
        UpdatePosition(player_center, window_width, window_height);
        UpdateZoom(input.mouse_wheel, input.held.ctrl);
    }
}

//GLOBALS
Game::GameState state{};
Game::Assets assets{};

//FUNCTIONS
Vector2 GetMouseGridPosition(Vector2 mouse_position, const Game::Camera::State& camera, unsigned int resolution){
    Vector2 world_position = GetScreenToWorld2D(mouse_position, camera.ToCamera2D());
    return {
        std::floor(world_position.x / Game::Config::TILE_RESOLUTION),
        std::floor(world_position.y / Game::Config::TILE_RESOLUTION),
    };

}

std::vector<Texture2D> GetTileTextures(const Image &spritesheet, unsigned int tile_resolution, unsigned int tile_type_count){
    std::vector<Texture2D> textures(tile_type_count);

    Rectangle source_rect = {0.0f, 0.0f, (float)tile_resolution, (float)tile_resolution};

    size_t index = 0;
    std::generate(textures.begin(), textures.end(), [&]() -> Texture2D {

        source_rect.x = index * tile_resolution % (spritesheet.width);
        source_rect.y = floor((float)index / ((float)spritesheet.width / tile_resolution)) * tile_resolution;

        Image sub_image = ImageFromImage(spritesheet, source_rect);
        Texture2D texture = LoadTextureFromImage(sub_image);
        UnloadImage(sub_image);
        index++;
        return texture;
    });

    return textures;

}

void Init(
    std::string name,
    unsigned int window_width,
    unsigned int window_height,
    unsigned int framerate,
    unsigned int tile_resolution,
    const unsigned int tile_type_count
){
    InitWindow(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "raylib basic window");
    if (framerate != 0){
        SetTargetFPS(framerate);

    }

    assets.tile_spritesheet = LoadImage("assets/tiles.png");
    assets.tile_textures = GetTileTextures(assets.tile_spritesheet, tile_resolution, tile_type_count);

    assets.player_texture = LoadTexture("assets/player.png");

}

void UpdateTilePlacing(GameState& state){
    if (state.input.pressed.space){
        state.tile_place_type++;
    }

    if (state.input.mouse_wheel && !state.input.held.ctrl){
        state.tile_place_type += state.input.mouse_wheel;
    }

    if (state.tile_place_type >= Config::TILE_COUNT){
        state.tile_place_type = 1;
    } else if (state.tile_place_type == 0){
        state.tile_place_type = Config::TILE_COUNT - 1;
    }

    if (state.input.held.lmb){
        auto mouse_grid_position = GetMouseGridPosition(state.input.mouse_position, state.camera);
        state.grid.Place(mouse_grid_position.x, mouse_grid_position.y, state.tile_place_type);
    }
    if (state.input.held.rmb){
        auto mouse_grid_position = GetMouseGridPosition(state.input.mouse_position, state.camera);
        state.grid.Place(mouse_grid_position.x, mouse_grid_position.y, 0);
    }

}

void Update(GameState& state){
    state.delta_time = GetFrameTime();
    state.input = Input::Capture();

    UpdateTilePlacing(state);

    state.player.Update(state.input, state.delta_time);

    state.camera.Update(state.player.GetCenter(), state.input);

}

void RenderTilePreview(unsigned int tile_type, Vector2 position){
    Texture2D texture = assets.tile_textures.at(tile_type);
    DrawTextureEx(texture, position, 0, 6, {255, 255, 255, 100});

}

void RenderTileGhost(unsigned int tile_type, Vector2 position){
    Texture2D texture = assets.tile_textures.at(tile_type);

    Rectangle rectangle{
        position.x * Config::TILE_RESOLUTION,
        position.y * Config::TILE_RESOLUTION,
        Config::TILE_RESOLUTION,
        Config::TILE_RESOLUTION
    };
    DrawRectangleRec(rectangle, {0, 0, 0, 130});
    DrawTextureV(texture, {rectangle.x, rectangle.y}, {255, 255, 255, 130});
    DrawRectangleLinesEx(rectangle, 1, {255, 255, 255, 130});

}

void Render(const GameState& state, const Assets& assets){
    BeginDrawing();
    ClearBackground(BLACK);

    //START DRAWING
    BeginMode2D(state.camera.ToCamera2D());

    Level::Render(state.grid, assets.tile_textures);
    RenderTileGhost(state.tile_place_type, GetMouseGridPosition(state.input.mouse_position, state.camera));

    Player::Render(state.player , assets.player_texture);

    EndMode2D();

    //Draw UI
    RenderTilePreview(state.tile_place_type, {Config::WINDOW_WIDTH - 80, 30});
    DrawText("It works!", 32, 32, 32, WHITE);
    DrawFPS(60, 60);

    EndDrawing();

}

void Run(){
    Game::Init("CaveSlave");

    while (!WindowShouldClose()){
        Update(state);
        Render(state, assets);
    }
    CloseWindow();

}

} // namespace Game
