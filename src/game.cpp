#include "game.h"

#include <raylib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

namespace Game {

//PLAYER
    void PlayerState::MoveBy(Vector2 offset) {
        Vector2 new_position = {position.x + offset.x, position.y + offset.y};
        position.x += offset.x;
        position.y += offset.y;
    }

    Vector2 PlayerState::GetCenter() const {
        return {position.x + size.x / 2, position.y + size.y / 2};
    }

    PlayerState PlayerState::New(
        Vector2 position,
        Vector2 size,
        float move_speed
    )
    {
        return {position, size, move_speed};
    }

    void PlayerState::Update(
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


//FUNCTIONS
    Vector2 GetMouseGridPosition(
        Vector2 mouse_position,
        const CenteredCamera& camera,
        unsigned int tile_resolution,
        unsigned int window_width,
        unsigned int window_height
    ){
        Vector2 world_position = GetScreenToWorld2D(mouse_position, camera.GetCamera2D(window_width, window_height));
        return {
            std::floor(world_position.x / tile_resolution),
            std::floor(world_position.y / tile_resolution),
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
        unsigned int framerate
    ){
        InitWindow(window_width, window_height, name.c_str());
        SetExitKey(KEY_NULL);
        IsWindowResized();
        if (framerate != 0){
            SetTargetFPS(framerate);
        }

    }

    Assets InitAssets(unsigned int tile_resolution, unsigned int tile_type_count){
        Assets assets;

        assets.tile_spritesheet = LoadImage("assets/tiles.png");
        assets.tile_textures = GetTileTextures(assets.tile_spritesheet, tile_resolution, tile_type_count);
        assets.player_texture = LoadTexture("assets/player.png");

        return assets;

    }


//UPDATE
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
            auto mouse_grid_position = GetMouseGridPosition(state.input.mouse_position, state.camera, Config::TILE_RESOLUTION, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
            state.grid.Place(mouse_grid_position.x, mouse_grid_position.y, state.tile_place_type);
        }
        if (state.input.held.rmb){
            auto mouse_grid_position = GetMouseGridPosition(state.input.mouse_position, state.camera, Config::TILE_RESOLUTION, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);
            state.grid.Place(mouse_grid_position.x, mouse_grid_position.y, 0);
        }

    }

    void UpdateLevel(const Input& input, Grid& grid){
        if (input.pressed.f5){
            std::cout << std::endl <<"LOADING LEVEL: Enter a level name: ";
            std::string level_name;
            std::cin >> level_name;
            auto new_grid = Grid::LoadFromFile(level_name);
            if (new_grid.has_value()){
                grid = new_grid.value();
            }
        } else if (input.pressed.f6){
            std::cout << std::endl <<"SAVING LEVEL: Enter a level name (null for none): ";
            std::string level_name;
            std::cin >> level_name;
            if (level_name.empty()){
                std::cout << std::endl << "Level not saved.";
            } else {
                grid.SaveToFile(level_name);
            }
        }
    }

    void Update(GameState& state){
        state.delta_time = GetFrameTime();
        state.input = Input::Capture();

        if (WindowShouldClose() || state.input.pressed.escape) {
            state.exit_requested = true;
        }

        if (state.exit_requested){
            if (state.input.pressed.y){
                state.exiting = true;
            }
            if (state.input.pressed.n){
                state.exit_requested = false;
            }
        }

        UpdateLevel(state.input, state.grid);
        UpdateTilePlacing(state);

        state.player.Update(state.input, state.delta_time);

        state.camera.Update(state.player.GetCenter(), state.input, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);

    }


//RENDER
    void RenderGrid(const Grid& grid, const std::vector<Texture2D>& tile_textures, Rectangle bounds, unsigned int tile_resolution){
        int start_x = bounds.x / tile_resolution;
        int start_y = bounds.y / tile_resolution;
        int end_x = (bounds.x + bounds.width) / tile_resolution + 1;
        int end_y = (bounds.y + bounds.height) / tile_resolution + 1;

        start_x = std::max(0, start_x);
        start_y = std::max(0, start_y);
        end_x = std::min((int)grid.size_x, end_x);
        end_y = std::min((int)grid.size_y, end_y);;

        for (int y = start_y; y < end_y; y++)
        {
            for (unsigned int x = start_x; x < end_x; x++)
            {
                const Texture2D& tile_texture = tile_textures.at(grid.GetTile(x, y).type);

                DrawTexture(tile_texture, x * tile_resolution, y * tile_resolution, WHITE);
            }
        }

    }

    void RenderTilePreview(unsigned int tile_type, Vector2 position, const std::vector<Texture2D>& tile_textures){
        Texture2D texture = tile_textures.at(tile_type);
        DrawTextureEx(texture, position, 0, 6, {255, 255, 255, 100});

    }

    void RenderTileGhost(
        unsigned int tile_type,
        Vector2 position,
        const std::vector<Texture2D>& tile_textures,
        unsigned int tile_resolution
    )
    {
        Texture2D texture = tile_textures.at(tile_type);

        Rectangle rectangle{
            position.x * tile_resolution,
            position.y * tile_resolution,
            static_cast<float>(tile_resolution),
            static_cast<float>(tile_resolution)
        };
        DrawRectangleRec(rectangle, {0, 0, 0, 130});
        DrawTextureV(texture, {rectangle.x, rectangle.y}, {255, 255, 255, 130});
        DrawRectangleLinesEx(rectangle, 1, {255, 255, 255, 130});

    }

    void RenderPlayer(const PlayerState& state, const Texture2D& texture){
        DrawTextureV(texture, state.position, WHITE);

    }

    void RenderExitScreen(const GameState& state, const Assets& assets){
        DrawRectangle(0 , 0, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, {0, 0, 0, 130});
        DrawText("Exit game? [y/n]", 0.5 * (Config::WINDOW_WIDTH - MeasureText("Exit game? [y/n]", 32)), 32, 32, WHITE);

    }

    void Render(const GameState& state, const Assets& assets){
        BeginDrawing();
        ClearBackground(BLACK);

        //START DRAWING
        BeginMode2D(state.camera.GetCamera2D(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT));

        RenderGrid(state.grid,  assets.tile_textures, state.camera.GetBounds(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), Config::TILE_RESOLUTION);
        RenderTileGhost(
            state.tile_place_type,
            GetMouseGridPosition(state.input.mouse_position, state.camera),
            assets.tile_textures,
            Config::TILE_RESOLUTION
        );

        RenderPlayer(state.player , assets.player_texture);

        EndMode2D();

        //Draw UI
        RenderTilePreview(state.tile_place_type, {Config::WINDOW_WIDTH - 80, 30}, assets.tile_textures);
        DrawText("CaveSlave", 32, 32, 32, WHITE);
        DrawFPS(60, 60);

        if (state.exit_requested){
            RenderExitScreen(state, assets);
        }

        EndDrawing();

    }

void Run(){
    Init("CaveSlave"); //Has to be first to be called

    auto assets = InitAssets();
    GameState state{};
    if (IsWindowReady()){
        while (true){
            Update(state);
            Render(state, assets);
            if (state.exiting){
                break;
            }
        }
    }

    CloseWindow();

}


} // namespace Game
