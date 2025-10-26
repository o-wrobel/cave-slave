#include "camera.h"
#include "model.h"

#include <algorithm>
#include <cmath>

Camera2D CenteredCamera::GetCamera2D(Vector2u window_size) const {
    return {
        {(float)window_size.x / 2, (float)window_size.y / 2},
        center,
        rotation,
        zoom
    };
}

Rectangle CenteredCamera::GetBounds(Vector2u window_size) const {
    float half_width = window_size.x / (2 * zoom);
    float half_height = window_size.y / (2 * zoom);
    return {
        center.x - half_width,
        center.y - half_height,
        half_width * 2,
        half_height * 2
    };
}

void CenteredCamera::UpdateZoom(float mouse_wheel_input, bool ctrl_held, Vector2u window_size){
    if (mouse_wheel_input != 0 && ctrl_held){
        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        float scale = 0.2f * mouse_wheel_input;
        zoom = std::clamp(std::expf(std::logf(zoom) + scale), 1 / 8.f, 64.0f);

    }
}

void CenteredCamera::UpdatePosition(Vector2 player_center, Vector2u window_size){
    center = player_center;
}

void CenteredCamera::Update(Vector2 player_center, Input input, Vector2u window_size){
    UpdatePosition(player_center, window_size);
    UpdateZoom(input.mouse_wheel, input.held.ctrl, window_size);
}
