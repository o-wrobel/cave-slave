#include <raylib.h>
#include <array>
#include <cmath>
#include <algorithm>
#include <vector>


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
    struct Config {
        static constexpr unsigned int WINDOW_WIDTH = 800;
        static constexpr unsigned int WINDOW_HEIGHT = 600;
        static constexpr unsigned int TARGET_FRAMERATE = 0;

        static constexpr unsigned int TILE_RESOLUTION = 8;
        static constexpr unsigned int TILE_COUNT = 8;

        static constexpr unsigned int GRID_SIZE_X = 64;
        static constexpr unsigned int GRID_SIZE_Y = 64;

        static constexpr bool ZOOM_INTO_MOUSE = false;

        static constexpr unsigned int TILE_SIZE = TILE_RESOLUTION * TILE_COUNT;
    };


    struct Input {
        struct Pressed{
            bool space;
        };

        struct Held{
            bool right, left, up, down, space;
            bool lmb, rmb;
        };

        Vector2 mouse_position;
        float mouse_wheel;
        Held held;
        Pressed pressed;

        static Input Capture()
        {
            return Input{
                GetMousePosition(),
                GetMouseWheelMove(),

                Held{
                    IsKeyDown(KEY_D),
                    IsKeyDown(KEY_A),
                    IsKeyDown(KEY_W),
                    IsKeyDown(KEY_S),
                    IsKeyDown(KEY_SPACE),
                    IsMouseButtonDown(0),
                    IsMouseButtonDown(1)
                },

                Pressed{IsKeyPressed(KEY_SPACE)}
            };
        }

    };


    namespace Level {
        struct Tile {
            unsigned int type;
        };

        struct Grid{
            const unsigned int size_x;
            const unsigned int size_y;

            std::vector<std::vector<Tile>> tiles;

            Grid(size_t width, size_t height) :
            size_x(width),
            size_y(height),
            tiles(height, std::vector<Tile>(width))
            {

            }

            void Place(unsigned int x, unsigned int y, unsigned int type){
                if (0 <= x && x < size_x && 0 <= y && y < size_y){
                    tiles.at(y).at(x).type = type;

                }
            }

            Tile GetTile(unsigned int x, unsigned int y) const { //TODO: Maybe make this a reference idk
                return tiles.at(y).at(x);
            }


            static Grid NewDefault(unsigned int width = Config::GRID_SIZE_X, unsigned int height = Config::GRID_SIZE_Y){
                Grid grid(width, height);
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

        };

        void Render(const Grid& grid, const std::array<Texture2D, Config::TILE_COUNT>& tile_textures){
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

    namespace Player {
        struct State {
            Vector2 position;
            Vector2 size;
            float move_speed;

            void MoveBy(Vector2 offset) {
                Vector2 new_position = {position.x + offset.x, position.y + offset.y};
                position.x += offset.x;
                position.y += offset.y;
            }

            Vector2 GetCenter() const {
                return {position.x + size.x / 2, position.y + size.y / 2};
            }
            static State New(
                        Vector2 position = {0., 0.},
                        Vector2 size = {8.0f, 8.0f},
                        float move_speed = 400.0f){
                        return {position, size, move_speed};
                    }

            void Update(
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
        };

        void Render(const State& state, const Texture2D& texture){
            DrawTextureV(texture, state.position, WHITE);
        }
    }

    namespace Camera {
        struct State {
            Vector2 offset = {0., 0.};
            Vector2 target = {0., 0.};
            float rotation = 0.;
            float zoom = 2.;

            Camera2D ToCamera2D() const {
                return {offset, target, rotation, zoom};
            }

            void SetZoom(float mouse_wheel_input){

                    if (mouse_wheel_input != 0){
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

            void FollowPlayer(Vector2 player_center, float window_width, float window_height){
                target = player_center;
                offset = {window_width / 2, window_height / 2};
            }

            void Update(Vector2 player_center, float mouse_wheel_input, float window_width, float window_height){
                FollowPlayer(player_center, window_width, window_height);
                SetZoom(mouse_wheel_input);
            }
        };

    }

    struct GameState{
        float delta_time;
        Input input;
        unsigned int tile_place_type = 1;
        Level::Grid grid = Level::Grid::NewDefault();
        Player::State player = Player::State::New({0, 0});
        Camera::State camera;

    }; //TODO: Remove unnecessary passing of state instead of it's variables

    // NON-CONSTANTS
    GameState state{};
    Image tile_spritesheet;
    std::array<Texture2D, Config::TILE_COUNT> tile_textures;

    Texture2D player_texture;



    // FUNCTIONS

    Vector2 GetMouseGridPosition(Vector2 mouse_position, const Camera::State& camera, unsigned int resolution = Config::TILE_RESOLUTION){
        Vector2 world_position = GetScreenToWorld2D(mouse_position, camera.ToCamera2D());
        return {
            std::floor(world_position.x / Config::TILE_RESOLUTION),
            std::floor(world_position.y / Config::TILE_RESOLUTION),
        };
    }


    template <size_t tile_count>
    std::array<Texture2D, tile_count> InitTileTextures(
        const Image &spritesheet,
        unsigned int tile_resolution)
    {
        std::array<Texture2D, tile_count> textures;

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

    void Init(){
        InitWindow(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, "raylib basic window");
        if (Config::TARGET_FRAMERATE != 0){
            SetTargetFPS(Config::TARGET_FRAMERATE);

        }

        tile_spritesheet = LoadImage("assets/tiles.png");
        tile_textures = InitTileTextures<Config::TILE_COUNT>(tile_spritesheet, Config::TILE_RESOLUTION);

        player_texture = LoadTexture("assets/player.png");

    }


    void UpdateTilePlacing(GameState& state){
        if (state.input.pressed.space){
            state.tile_place_type++;
            if (state.tile_place_type >= Config::TILE_COUNT){
                state.tile_place_type = 0;
            }
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

        state.camera.Update(state.player.GetCenter(), state.input.mouse_wheel, Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT);

    }

    void RenderTilePreview(unsigned int tile_type, Vector2 position){
        Texture2D texture = tile_textures.at(tile_type);
        DrawTextureEx(texture, position, 0, 6, {255, 255, 255, 100});
    }

    void RenderTileGhost(unsigned int tile_type, Vector2 position){
        Texture2D texture = tile_textures.at(tile_type);

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

    void Render(const GameState& state){
        BeginDrawing();
        ClearBackground(BLACK);

        //START DRAWING
        BeginMode2D(state.camera.ToCamera2D());

        Level::Render(state.grid, tile_textures);
        RenderTileGhost(state.tile_place_type, GetMouseGridPosition(state.input.mouse_position, state.camera));

        Player::Render(state.player , player_texture);

        EndMode2D();

        //Draw UI
        RenderTilePreview(state.tile_place_type, {Config::WINDOW_WIDTH - 80, 30});
        DrawText("It works!", 32, 32, 32, WHITE);
        DrawFPS(60, 60);

        EndDrawing();
    }

    void Game(){
        Game::Init();

        while (!WindowShouldClose()){
            Update(state);
            Render(state);
        }
        CloseWindow();

    }

}


int main(){
    Game::Game();
    return 0;
}
