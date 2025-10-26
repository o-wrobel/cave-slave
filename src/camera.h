#pragma once
#include <raylib.h>

#include "input.h"

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
        float window_width,
        float window_height
    );
};
