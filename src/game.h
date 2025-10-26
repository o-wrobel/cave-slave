#pragma once

#include "grid.h"
#include "input.h"
#include "camera.h"

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

    static constexpr float GRAVITY = 800;
};
typedef enum Direction{
    RIGHT = 1,
    LEFT = -1
} Direction;

enum GameMode{
    EDITOR,
    PLAY
};

typedef struct Sprite {
    Texture2D texture;
    Rectangle dest_rect;
    Direction direction = RIGHT;

    void Draw() const;

} Sprite;


struct Player{
    Sprite sprite;
    Vector2 size;
    Vector2 velocity;
    float speed_factor;

    void Move(float horizontal, bool space_held);

    void FreeMove(float horizontal, float vertical);

    void ApplyVelocity(float delta_time);

    void ApplyGravity(float delta_time);

    Vector2 GetCenter() const;

    static Player New(
        Texture2D texture,
        Vector2 size = {8.0f, 8.0f},
        float move_speed = 400.0f
    );

    void Update(GameMode game_type, const Input& input, float delta_time);
};

struct Assets{
    Image tile_spritesheet;
    std::vector<Texture2D> tile_textures;

    Texture2D player_texture;

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

void RenderPlayer(const Player& player, const Texture2D& texture);

void Render(const GameState& state, const Assets& assets);

void Run();

} //Game
