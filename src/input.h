#pragma once

#include <raylib.h>

struct Input {
    struct Pressed{
        bool space;
        bool escape;
        bool y;
        bool n;
        bool f4;
        bool f5;
        bool f6;
    };

    struct Held{
        bool ctrl;
        bool right, left, up, down, space;
        bool lmb, rmb;
    };

    Vector2 mouse_position;
    float mouse_wheel;
    Held held;
    Pressed pressed;

    static Input Capture();

};
