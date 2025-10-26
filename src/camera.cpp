#include "camera.h"

#include <algorithm>
#include <cmath>

Camera2D CenteredCamera::GetCamera2D(uint16_t window_width, uint16_t window_height) const {
    return {
        {(float)window_width / 2, (float)window_height / 2},
        center,
        rotation,
        zoom
    };
}

Rectangle CenteredCamera::GetBounds(uint16_t window_width, uint16_t window_height) const {
    float half_width = window_width / (2 * zoom);
    float half_height = window_height / (2 * zoom);
    return {
        center.x - half_width,
        center.y - half_height,
        half_width * 2,
        half_height * 2
    };
}

void CenteredCamera::UpdateZoom(float mouse_wheel_input, bool ctrl_held, uint16_t window_width, uint16_t window_height){
    if (mouse_wheel_input != 0 && ctrl_held){
        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        float scale = 0.2f * mouse_wheel_input;
        zoom = std::clamp(std::expf(std::logf(zoom) + scale), 1 / 8.f, 64.0f);

    }
}

void CenteredCamera::UpdatePosition(Vector2 player_center, float window_width, float window_height){
    center = player_center;
}

void CenteredCamera::Update(Vector2 player_center, Input input, float window_width, float window_height){
    UpdatePosition(player_center, window_width, window_height);
    UpdateZoom(input.mouse_wheel, input.held.ctrl, window_width, window_height);
}
