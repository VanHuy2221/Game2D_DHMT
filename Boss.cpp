#include "Boss.h"
#include <cmath>

Boss::Boss(Vector2 startPos, float bossSize) {
    position = startPos;
    size = bossSize;
    bobTime = 0.0f;
    rotation = 0.0f;
}

void Boss::Update() {
    bobTime += 0.042f;
    rotation = sinf(bobTime * 0.8f) * 5.0f;     // xoay nhẹ
}

void Boss::Draw() {
    Vector2 c = position;
    float bob = sinf(bobTime) * 8.0f;

    // === 1. HÀO QUANG (thu nhỏ lại, đẹp và tinh tế hơn) ===
    for (int i = 0; i < 5; i++) {
        float alpha = 0.28f - i * 0.055f;
        float radius = size * 0.92f + i * 22.0f;     // thu nhỏ đáng kể
        DrawCircle(c.x, c.y + bob, radius, Fade(ORANGE, alpha));
        DrawCircle(c.x, c.y + bob, radius * 0.92f, Fade(YELLOW, alpha * 0.65f));
    }

    // === 2. THÂN TAM GIÁC (to và rõ hơn) ===
    Vector2 top   = { c.x + rotation * 0.35f, c.y - size * 0.62f + bob };
    Vector2 left  = { c.x - size * 0.53f,     c.y + size * 0.44f + bob };
    Vector2 right = { c.x + size * 0.53f,     c.y + size * 0.44f + bob };

    DrawTriangle(top, left, right, GOLD);
    DrawTriangle(top, left, right, Fade(YELLOW, 0.35f));  // lớp sáng nhẹ
    DrawTriangleLines(top, left, right, Fade(BLACK, 0.9f));

    // === 3. CORE NĂNG LƯỢNG Ở GIỮA (nổi bật hơn) ===
    float coreX = c.x + rotation * 0.15f;
    float coreY = c.y + bob * 0.4f;
    float coreRadius = size * 0.19f;

    // Vòng sáng quanh core
    DrawCircle(coreX, coreY, coreRadius * 1.55f, Fade(YELLOW, 0.55f));
    DrawCircle(coreX, coreY, coreRadius * 1.25f, Fade(ORANGE, 0.45f));

    // Core chính
    DrawCircle(coreX, coreY, coreRadius, RED);           // lõi đỏ mạnh
    DrawCircle(coreX, coreY, coreRadius * 0.68f, YELLOW);

    // Điểm sáng trên core
    DrawCircle(coreX - coreRadius * 0.38f, coreY - coreRadius * 0.38f, 
               coreRadius * 0.28f, WHITE);
}