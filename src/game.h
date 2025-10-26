#pragma once

#include "grid.h"

#include <raylib.h>
#include <string>
#include <vector>

namespace Game{

struct Config {
    static constexpr unsigned int WINDOW_WIDTH = 800;
    static constexpr unsigned int WINDOW_HEIGHT = 600;
    static constexpr unsigned int TARGET_FRAMERATE = 0;

    static constexpr unsigned int TILE_RESOLUTION = 8;
    static constexpr unsigned int TILE_COUNT = 8;

    static constexpr unsigned int GRID_SIZE_X = 64;
    static constexpr unsigned int GRID_SIZE_Y = 64;

    static constexpr unsigned int TILE_SIZE = TILE_RESOLUTION * TILE_COUNT;
};

struct Input {
    struct Pressed{
        bool space;
        bool escape;
        bool y;
        bool n;
        bool f5;
        bool f6;
    };

    struct Held{
        bool ctrl;
        bool right, left, up, down, space;
        bool lmb, rmb;
    };

    Vector2 mouse_position;
    float mouse_wheel;
    Held held;
    Pressed pressed;

    static Input Capture();

};


struct PlayerState {
    Vector2 position;
    Vector2 size;
    float move_speed;

    void MoveBy(Vector2 offset);

    Vector2 GetCenter() const;
    static PlayerState New(
        Vector2 position = {0., 0.},
        Vector2 size = {8.0f, 8.0f},
        float move_speed = 400.0f
    );

    void Update(const Input& input, float delta_time);
};

//CAMERA
struct CenteredCamera {
    Vector2 center = {0., 0.};
    Vector2 offset = {0., 0.};
    Vector2 target = {0., 0.};
    float rotation = 0.;
    float zoom = 2.;

    Camera2D GetCamera2D(unsigned int window_width, unsigned int window_height) const;

    Rectangle GetBounds(unsigned int window_width, unsigned int window_height) const;

    void UpdateZoom(float mouse_wheel_input, bool ctrl_held, unsigned int window_width, unsigned int window_height);

    void UpdatePosition(Vector2 player_center, float window_width, float window_height);

    void Update(Vector2 player_center,
        Input input,
        float window_width = Config::WINDOW_WIDTH,
        float window_height = Config::WINDOW_HEIGHT
    );
};

struct Assets{
    Image tile_spritesheet;
    std::vector<Texture2D> tile_textures;

    Texture2D player_texture;

};

struct GameState{
    float delta_time;
    Input input;
    bool exit_requested = false;
    bool exiting = false;
    Grid grid = Grid::NewDefault(Config::GRID_SIZE_X, Config::GRID_SIZE_Y);
    unsigned int tile_place_type = 1;
    PlayerState player = PlayerState::New({0, 0});
    CenteredCamera camera;

}; //TODO: Remove unnecessary passing of state instead of its variables

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

void RenderPlayer(const PlayerState& player, const Texture2D& texture);

void Render(const GameState& state, const Assets& assets);

void Run();

} //Game
