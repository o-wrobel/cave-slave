#pragma once

#include "model.h"
#include <raylib.h>

struct Player{
    Sprite sprite;
    Vector2 size;
    Vector2 velocity;
    float speed_factor;

    void Move(float horizontal, bool space_held);

    void FreeMove(float horizontal, float vertical);

    void ApplyVelocity(float delta_time);

    void ApplyGravity(float gravity, float delta_time);

    Vector2 GetCenter() const;

    static Player New(
        Texture2D texture,
        Vector2 size = {8.0f, 8.0f},
        float move_speed = 400.0f
    );

    void Update(GameMode game_type, const Input& input, float gravity, float delta_time);
};
