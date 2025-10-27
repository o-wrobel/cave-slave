#pragma once

#include "model.h"
#include "grid.h"
#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

struct Player{
    Sprite sprite;
    float max_horizontal_speed;
    Vector2 velocity = {0.0f, 0.0f};
    bool is_grounded = false;

    void SetVelocity(float horizontal_input, bool jump_key_held);

    void ResolveCollision(Rectangle tile_rect);

    void CheckCollision(const Grid& grid, uint16_t tile_resolution);

    void ApplyVelocityFree(float horizontal, float vertical);

    void ApplyVelocity(float delta_time);

    void ApplyGravity(float gravity, float delta_time);

    Vector2 GetCenterPosition() const;

    static Player New(
        Texture2D texture,
        Vector2 size = {8.0f, 8.0f},
        float max_horizontal_speed = 100.0f
    );

    void Update(GameMode game_type, const Input& input, const Grid& grid, float gravity, float delta_time);
};
