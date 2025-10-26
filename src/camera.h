#pragma once

#include "model.h"
#include "input.h"

#include <raylib.h>
#include <cstdint>
#include <sys/types.h>


struct CenteredCamera {
    Vector2 center = {0., 0.};
    Vector2 offset = {0., 0.};
    float rotation = 0.;
    float zoom = 2.;

    Camera2D GetCamera2D(Vector2u window_size) const;

    Rectangle GetBounds(Vector2u window_size) const;

    void UpdateZoom(float mouse_wheel_input, bool ctrl_held, Vector2u window_size);

    void UpdatePosition(Vector2 player_center, Vector2u window_size);

    void Update(Vector2 player_center, Input input, Vector2u window_size);
};
