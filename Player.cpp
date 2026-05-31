#include "Player.h"
#include "DrawUtils.h"
#include <cmath>

Player::Player(Vector2 startPos) {
    position = startPos;
    velocity = {0, 0};
    speed = 1.0f;
    size = 30.0f;
    isDashing = false;
    dashTime = 0;
    dashCooldown = 0;
    justDashed = false;  // TP4
}

void Player::Update() {
    // ===== INPUT =====
    Vector2 input = {0, 0};
    if (IsKeyDown(KEY_W)) input.y -= 1;
    if (IsKeyDown(KEY_S)) input.y += 1;
    if (IsKeyDown(KEY_A)) input.x -= 1;
    if (IsKeyDown(KEY_D)) input.x += 1;

    // ===== DASH =====
    justDashed = false;  // Reset flag moi frame
    
    if (IsKeyPressed(KEY_SPACE) && dashCooldown <= 0) {
        isDashing = true;
        dashTime = 10;
        dashCooldown = 30;
        justDashed = true;  // TP4: danh dau vua dash

        if (input.x != 0 || input.y != 0) {
            float len = sqrtf(input.x*input.x + input.y*input.y);
            input.x /= len;
            input.y /= len;
            velocity.x = input.x * 15;
            velocity.y = input.y * 15;

            // Hieu ung no khi dash
            dashEffects.push_back({position, 10, 20});
        }
    }

    // ===== DI CHUYEN =====
    if (!isDashing) {
        velocity.x += input.x * speed;
        velocity.y += input.y * speed;
        // ma sat
        velocity.x *= 0.85f;
        velocity.y *= 0.85f;
    }

    // ===== UPDATE DASH =====
    if (isDashing) {
        dashTime--;
        if (dashTime <= 0) isDashing = false;
    }
    if (dashCooldown > 0) dashCooldown--;

    // ===== CAP NHAT VI TRI =====
    position.x += velocity.x;
    position.y += velocity.y;

    // ===== GIOI HAN MAN HINH =====
    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    if (position.x < size/2)                  position.x = size/2;
    if (position.x > screenWidth  - size/2)   position.x = screenWidth  - size/2;
    if (position.y < size/2)                  position.y = size/2;
    if (position.y > screenHeight - size/2)   position.y = screenHeight - size/2;

    // ===== UPDATE DASH EFFECT =====
    for (int i = 0; i < (int)dashEffects.size(); i++) {
        dashEffects[i].radius += 2.5f;
        dashEffects[i].life--;
        if (dashEffects[i].life <= 0) {
            dashEffects.erase(dashEffects.begin() + i);
            i--;
        }
    }
}

// ... phan con lai cua Player.cpp giu nguyen (RotatedRectCorners, Draw, etc.)
static void RotatedRectCorners(
    float cx, float cy,
    float w,  float h,
    float angleDeg,
    Vector2 out[4]
) {
    float rad = angleDeg * DEG2RAD;
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    float lx[4] = { -w/2,  w/2,  w/2, -w/2 };
    float ly[4] = { -h/2, -h/2,  h/2,  h/2 };

    for (int i = 0; i < 4; i++) {
        out[i].x = cx + lx[i] * cosA - ly[i] * sinA;
        out[i].y = cy + lx[i] * sinA + ly[i] * cosA;
    }
}

static void DrawQuadScanline(Vector2 v[4], Color color) {
    DrawTriangleScanline(v[0], v[1], v[2], color);
    DrawTriangleScanline(v[0], v[2], v[3], color);
}

static void DrawQuadOutlineBresenham(Vector2 v[4], Color color) {
    DrawLineBresenham((int)v[0].x, (int)v[0].y, (int)v[1].x, (int)v[1].y, color);
    DrawLineBresenham((int)v[1].x, (int)v[1].y, (int)v[2].x, (int)v[2].y, color);
    DrawLineBresenham((int)v[2].x, (int)v[2].y, (int)v[3].x, (int)v[3].y, color);
    DrawLineBresenham((int)v[3].x, (int)v[3].y, (int)v[0].x, (int)v[0].y, color);
}

void Player::Draw() {
    float speedLen = sqrtf(velocity.x*velocity.x + velocity.y*velocity.y);

    Vector2 dir = {0, 0};
    if (speedLen > 0.001f) {
        dir.x = velocity.x / speedLen;
        dir.y = velocity.y / speedLen;
    }

    float stretch = isDashing ? 1.8f : 1.0f;
    float squash  = isDashing ? 0.6f : 1.0f;
    float angle   = atan2f(dir.y, dir.x) * RAD2DEG;

    float w = size * stretch;
    float h = size * squash;

    // Hieu ung dash
    for (auto &e : dashEffects) {
        float alpha = e.life / 20.0f;
        Color ringColor = Fade(SKYBLUE, alpha);
        DrawCircleMidpoint((int)e.position.x, (int)e.position.y, (int)e.radius,       ringColor);
        DrawCircleMidpoint((int)e.position.x, (int)e.position.y, (int)e.radius + 1,   Fade(SKYBLUE, alpha * 0.5f));
    }

    // Than player
    Vector2 corners[4];
    RotatedRectCorners(position.x, position.y, w, h, angle, corners);

    DrawQuadScanline(corners, SKYBLUE);
    DrawQuadOutlineBresenham(corners, Fade(DARKBLUE, 0.7f));

    DrawCircleFilledMidpoint(
        (int)(corners[0].x * 0.6f + position.x * 0.4f),
        (int)(corners[0].y * 0.6f + position.y * 0.4f),
        (int)(size * 0.12f),
        Fade(WHITE, 0.4f)
    );
}