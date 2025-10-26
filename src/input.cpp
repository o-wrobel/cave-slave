#include "input.h"

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
            .f5 = IsKeyPressed(KEY_F5),
            .f6 = IsKeyPressed(KEY_F6)
        }
    };
}
