#include "model.h"

Input Input::Capture()
{
    return Input{
        GetMousePosition(),
        GetMouseWheelMove(),

        Input::Held{
            .ctrl = IsKeyDown(KEY_LEFT_CONTROL),
            .right = IsKeyDown(KEY_D),
            .left = IsKeyDown(KEY_A),
            .up = IsKeyDown(KEY_W),
            .down = IsKeyDown(KEY_S),
            .space = IsKeyDown(KEY_SPACE),
            .lmb = IsMouseButtonDown(0),
            .rmb = IsMouseButtonDown(1)
        },

        Input::Pressed{
            .space = IsKeyPressed(KEY_SPACE),
            .escape = IsKeyPressed(KEY_ESCAPE),
            .y = IsKeyPressed(KEY_Y),
            .n = IsKeyPressed(KEY_N),
            .f4 = IsKeyPressed(KEY_F4),
            .f5 = IsKeyPressed(KEY_F5),
            .f6 = IsKeyPressed(KEY_F6)
        }
    };
}

void Sprite::Draw() const{
    DrawTexturePro(texture, {0, 0, 8 * (float)direction, 8}, dest_rect, {0, 0}, 0, WHITE);
};
