#pragma once
#include "raylib.h"
#include <vector>

struct DashEffect {
    Vector2 position;
    float radius;
    float life;
};

class Player {
public:
    Vector2 position;
    Vector2 velocity;
    float speed;
    float size;
    bool isDashing;
    float dashTime;
    float dashCooldown;
    std::vector<DashEffect> dashEffects; // 💥 hiệu ứng nổ

    Player(Vector2 startPos);
    void Update();
    void Draw();
};