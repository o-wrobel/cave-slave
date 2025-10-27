#include "game.h"

#include <raylib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

namespace Game {

//FUNCTIONS
    Vector2 GetMouseGridPosition(
        Vector2 mouse_position,
        const CenteredCamera& camera,
        uint16_t tile_resolution,
        Vector2u window_size
    ){
        Vector2 world_position = GetScreenToWorld2D(mouse_position, camera.GetCamera2D(window_size));
        return {
            std::floor(world_position.x / tile_resolution),
            std::floor(world_position.y / tile_resolution),
        };

    }

    std::vector<Texture2D> GetTileTextures(const Image &spritesheet, uint16_t tile_resolution, uint16_t tile_type_count){
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
        Vector2u window_size,
        uint16_t framerate
    ){
        InitWindow(window_size.x, window_size.y, name.c_str());
        SetExitKey(KEY_NULL);
        IsWindowResized();
        if (framerate != 0){
            SetTargetFPS(framerate);
        }

    }

    Assets InitAssets(uint16_t tile_resolution, uint16_t tile_type_count){
        Assets assets;

        assets.tile_spritesheet = LoadImage("assets/tiles.png");
        assets.tile_textures = GetTileTextures(assets.tile_spritesheet, tile_resolution, tile_type_count);
        assets.player_texture = LoadTexture("assets/player.png");

        return assets;

    }


//UPDATE
    void UpdateTilePlacing(GameState& state){
        if (state.input.pressed.space && state.game_mode == EDITOR){
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
            auto mouse_grid_position = GetMouseGridPosition(
                state.input.mouse_position,
                state.camera,
                Config::TILE_RESOLUTION,
                Config::WINDOW_SIZE
            );
            state.grid.Place(mouse_grid_position.x, mouse_grid_position.y, state.tile_place_type);
        }
        if (state.input.held.rmb){
            auto mouse_grid_position = GetMouseGridPosition(
                state.input.mouse_position,
                state.camera,
                Config::TILE_RESOLUTION,
                Config::WINDOW_SIZE
            );
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

        if(state.input.pressed.f4){
            state.game_mode = (state.game_mode == PLAY) ? EDITOR : PLAY;
        }

        UpdateLevel(state.input, state.grid);
        UpdateTilePlacing(state);

        state.player.Update(state.game_mode, state.input, state.grid, Config::GRAVITY, state.delta_time);

        state.camera.Update(state.player.GetCenterPosition(), state.input, Config::WINDOW_SIZE);

    }


//RENDER
    void RenderGrid(const Grid& grid, const std::vector<Texture2D>& tile_textures, Rectangle bounds, uint16_t tile_resolution){
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
            for (uint16_t x = start_x; x < end_x; x++)
            {
                const Texture2D& tile_texture = tile_textures.at(grid.GetTile(x, y).type);

                DrawTexture(tile_texture, x * tile_resolution, y * tile_resolution, WHITE);
            }
        }

    }

    void RenderTilePreview(uint16_t tile_type, Vector2 position, const std::vector<Texture2D>& tile_textures){
        Texture2D texture = tile_textures.at(tile_type);
        DrawTextureEx(texture, position, 0, 6, {255, 255, 255, 100});

    }

    void RenderTileGhost(
        uint16_t tile_type,
        Vector2 position,
        const std::vector<Texture2D>& tile_textures,
        uint16_t tile_resolution
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

    void RenderPlayer(const Player& player, const Texture2D& texture){
        player.sprite.Draw();
        // DrawTextureV(texture, player.position, WHITE);

    }

    void RenderExitScreen(const GameState& state, const Assets& assets){
        DrawRectangle(0 , 0, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, {0, 0, 0, 130});
        DrawText("Exit game? [y/n]", 0.5 * (Config::WINDOW_WIDTH - MeasureText("Exit game? [y/n]", 32)), 32, 32, WHITE);

    }

    void Render(const GameState& state, const Assets& assets){
        BeginDrawing();
        ClearBackground(BLACK);

        //START DRAWING
        BeginMode2D(state.camera.GetCamera2D(Config::WINDOW_SIZE));

        RenderGrid(state.grid,  assets.tile_textures, state.camera.GetBounds(Config::WINDOW_SIZE), Config::TILE_RESOLUTION);
        RenderTileGhost(
            state.tile_place_type,
            GetMouseGridPosition(state.input.mouse_position, state.camera, Config::TILE_RESOLUTION, Config::WINDOW_SIZE),
            assets.tile_textures,
            Config::TILE_RESOLUTION
        );
        if (state.game_mode == PLAY){
            RenderPlayer(state.player , assets.player_texture);
        }

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
    Init("CaveSlave", Config::WINDOW_SIZE, Config::TARGET_FRAMERATE); //Has to be first to be called

    auto assets = InitAssets();
    GameState state{};
    state.player = Player::New(assets.player_texture) ;
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
