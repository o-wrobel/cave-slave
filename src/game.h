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
    static constexpr uint16_t TARGET_FRAMERATE = 0;

    static constexpr uint16_t TILE_RESOLUTION = 8;
    static constexpr uint16_t TILE_COUNT = 8;

    static constexpr uint16_t GRID_SIZE_X = 64;
    static constexpr uint16_t GRID_SIZE_Y = 64;

    static constexpr float GRAVITY = 800;
};

struct GameState{
    GameMode game_mode = GameMode::EDITOR;
    float delta_time;
    Input input;
    bool exit_requested = false;
    bool exiting = false;
    Grid grid = Grid::NewDefault(Config::GRID_SIZE_X, Config::GRID_SIZE_Y);
    unsigned int tile_place_type = 1;
    Player player = Player::New({0, 0});
    Sprite player_sprite;
    CenteredCamera camera;

};

//FUNCTIONS
Vector2 GetMouseGridPosition(
    Vector2 mouse_position,
    const CenteredCamera& camera,
    unsigned int resolution = Config::TILE_RESOLUTION,
    unsigned int window_width = Config::WINDOW_WIDTH,
    unsigned int window_height = Config::WINDOW_HEIGHT
);

std::vector<Texture2D> GetTileTextures(const Image &spritesheet, unsigned int tile_resolution, unsigned int tile_type_count);

void Init(
    std::string name,
    unsigned int window_width = Config::WINDOW_WIDTH,
    unsigned int window_height = Config::WINDOW_HEIGHT,
    unsigned int framerate = Config::TARGET_FRAMERATE
);

Assets InitAssets(
    unsigned int tile_resolution = Config::TILE_RESOLUTION,
    unsigned int tile_type_count = Config::TILE_COUNT
);

void UpdateTilePlacing(GameState& state);

void UpdateLevel(const Input& input, Grid& grid);

void Update(GameState& state);

void RenderGrid(const GameState& state, const Assets& assets, Rectangle bounds, unsigned int tile_resolution);

void RenderTilePreview(unsigned int tile_type, Vector2 position, std::vector<Texture2D>& tile_textures);

void RenderTileGhost(
    unsigned int tile_type,
    Vector2 position,
    const std::vector<Texture2D>& tile_textures,
    const unsigned int tile_resolution
);

void RenderPlayer(const Player& player, const Texture2D& texture);

void Render(const GameState& state, const Assets& assets);

void Run();

} //Game
