#include "Boss.h"
#include "DrawUtils.h"
#include <cmath>

Boss::Boss(Vector2 startPos, float bossSize) {
    position = startPos;
    size = bossSize;
    bobTime = 0.0f;
    rotation = 0.0f;
}

void Boss::Update() {
    bobTime += 0.042f;
    rotation = sinf(bobTime * 0.8f) * 5.0f;
}

void Boss::Draw() {
    Vector2 c = position;
    float bob = sinf(bobTime) * 8.0f;

    // === 1. HAO QUANG (dung DrawCircleFilledMidpoint theo lop voi alpha giam dan) ===
    // Thay vi DrawCircle + Fade (dung ham co san), dung ham tu cai de the hien TP2
    // Ve 5 lop hao quang, moi lop lon hon va mo hon (giong code cu nhung dung ham moi)
    for (int i = 0; i < 5; i++) {
        float alpha  = 0.28f - i * 0.055f;
        float radius = size * 0.92f + i * 22.0f;

        // Lop cam ngoai (outline day de tao hieu ung glow)
        DrawCircleThick((int)c.x, (int)(c.y + bob), (int)radius, 3, Fade(ORANGE, alpha));
        // Lop vang ben trong (nho hon mot chut)
        DrawCircleThick((int)c.x, (int)(c.y + bob), (int)(radius * 0.92f), 2, Fade(YELLOW, alpha * 0.65f));
    }

    // === 2. THAN TAM GIAC (dung DrawTriangleBresenham - scanline fill + Bresenham outline) ===
    // Tinh toa do 3 dinh (giong y het code cu, giu nguyen ngoai hinh)
    Vector2 top   = { c.x + rotation * 0.35f, c.y - size * 0.62f + bob };
    Vector2 left  = { c.x - size * 0.53f,     c.y + size * 0.44f + bob };
    Vector2 right = { c.x + size * 0.53f,     c.y + size * 0.44f + bob };

    // Fill vang (GOLD) bang scanline, vien den bang Bresenham
    DrawTriangleBresenham(top, left, right, GOLD, Fade(BLACK, 0.9f));

    // Lop sang nhe tren than (lop thu 2 cua code cu - fill mau vang nhat)
    DrawTriangleScanline(top, left, right, Fade(YELLOW, 0.35f));

    // === 3. CORE NANG LUONG (dung DrawCircleFilledMidpoint + DrawCircleMidpoint) ===
    float coreX = c.x + rotation * 0.15f;
    float coreY = c.y + bob * 0.4f;
    float coreRadius = size * 0.19f;

    // Vong sang quanh core (outline day)
    DrawCircleThick((int)coreX, (int)coreY, (int)(coreRadius * 1.55f), 3, Fade(YELLOW, 0.55f));
    DrawCircleThick((int)coreX, (int)coreY, (int)(coreRadius * 1.25f), 2, Fade(ORANGE, 0.45f));

    // Core chinh: to dac do
    DrawCircleFilledMidpoint((int)coreX, (int)coreY, (int)coreRadius, RED);
    // Lop vang ben trong core
    DrawCircleFilledMidpoint((int)coreX, (int)coreY, (int)(coreRadius * 0.68f), YELLOW);

    // Diem sang tren core (highlight)
    DrawCircleFilledMidpoint(
        (int)(coreX - coreRadius * 0.38f),
        (int)(coreY - coreRadius * 0.38f),
        (int)(coreRadius * 0.28f),
        WHITE
    );
}