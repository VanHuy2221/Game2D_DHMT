#pragma once
#include "raylib.h"

class Boss {
public:
    Vector2 position;
    float size;
    float bobTime;
    float rotation;

    Boss(Vector2 startPos, float bossSize = 180.0f);
    void Update();
    void Draw();
};