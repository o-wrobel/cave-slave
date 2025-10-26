#pragma once

#include "model.h"
#include "input.h"
#include "grid.h"
#include "camera.h"
#include "player.h"

#include <cstdint>
#include <raylib.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace Game{

struct Config {
    static constexpr uint16_t WINDOW_WIDTH = 800;
    static constexpr uint16_t WINDOW_HEIGHT = 600;
    static constexpr Vector2u WINDOW_SIZE = {WINDOW_WIDTH, WINDOW_HEIGHT};

    static constexpr uint16_t TARGET_FRAMERATE = 0;

    static constexpr uint16_t TILE_RESOLUTION = 8;
    static constexpr uint16_t TILE_COUNT = 8;

    static constexpr uint16_t GRID_WIDTH = 64;
    static constexpr uint16_t GRID_HEIGHT = 64;
    static constexpr Vector2u GRID_SIZE = {GRID_WIDTH, GRID_HEIGHT};

    static constexpr float GRAVITY = 800;
};

struct GameState{
    GameMode game_mode = GameMode::EDITOR;
    float delta_time;
    Input input;
    bool exit_requested = false;
    bool exiting = false;
    Grid grid = Grid::NewDefault(Config::GRID_WIDTH, Config::GRID_HEIGHT);
    uint16_t tile_place_type = 1;
    Player player = Player::New({0, 0});
    Sprite player_sprite;
    CenteredCamera camera;

};

//FUNCTIONS
Vector2 GetMouseGridPosition(
    Vector2 mouse_position,
    const CenteredCamera& camera,
    uint16_t resolution,
    Vector2u window_size
);

std::vector<Texture2D> GetTileTextures(const Image &spritesheet, uint16_t tile_resolution, uint16_t tile_type_count);

void Init(
    std::string name,
    Vector2u window_size,
    uint16_t framerate
);

Assets InitAssets(
    uint16_t tile_resolution = Config::TILE_RESOLUTION,
    uint16_t tile_type_count = Config::TILE_COUNT
);

void UpdateTilePlacing(GameState& state);

void UpdateLevel(const Input& input, Grid& grid);

void Update(GameState& state);

void RenderGrid(const GameState& state, const Assets& assets, Rectangle bounds, uint16_t tile_resolution);

void RenderTilePreview(uint16_t tile_type, Vector2 position, std::vector<Texture2D>& tile_textures);

void RenderTileGhost(
    uint16_t tile_type,
    Vector2 position,
    const std::vector<Texture2D>& tile_textures,
    const uint16_t tile_resolution
);

void RenderPlayer(const Player& player, const Texture2D& texture);

void Render(const GameState& state, const Assets& assets);

void Run();

} //Game
