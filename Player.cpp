#include "Player.h"
#include <cmath>

Player::Player(Vector2 startPos) {
    position = startPos;
    velocity = {0, 0};
    speed = 1.0f;
    size = 30.0f;
    isDashing = false;
    dashTime = 0;
    dashCooldown = 0;
}

void Player::Update() {
    // ===== INPUT =====
    Vector2 input = {0, 0};
    if (IsKeyDown(KEY_W)) input.y -= 1;
    if (IsKeyDown(KEY_S)) input.y += 1;
    if (IsKeyDown(KEY_A)) input.x -= 1;
    if (IsKeyDown(KEY_D)) input.x += 1;

    // ===== DASH =====
    if (IsKeyPressed(KEY_SPACE) && dashCooldown <= 0) {
        isDashing = true;
        dashTime = 10;
        dashCooldown = 30;

        if (input.x != 0 || input.y != 0) {
            float len = sqrt(input.x*input.x + input.y*input.y);
            input.x /= len;
            input.y /= len;
            velocity.x = input.x * 15;
            velocity.y = input.y * 15;

            // 💥 VỤ NỔ NGAY TẠI VỊ TRÍ NHÂN VẬT (đã sửa)
            dashEffects.push_back({position, 10, 20});
        }
    }

    // ===== DI CHUYỂN =====
    if (!isDashing) {
        velocity.x += input.x * speed;
        velocity.y += input.y * speed;
        // ma sát
        velocity.x *= 0.85f;
        velocity.y *= 0.85f;
    }

    // ===== UPDATE DASH =====
    if (isDashing) {
        dashTime--;
        if (dashTime <= 0) {
            isDashing = false;
        }
    }
    if (dashCooldown > 0) dashCooldown--;

    // ===== CẬP NHẬT VỊ TRÍ =====
    position.x += velocity.x;
    position.y += velocity.y;

    // ===== GIỚI HẠN =====
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    if (position.x < size/2) position.x = size/2;
    if (position.x > screenWidth - size/2) position.x = screenWidth - size/2;
    if (position.y < size/2) position.y = size/2;
    if (position.y > screenHeight - size/2) position.y = screenHeight - size/2;

    // ===== UPDATE DASH EFFECT =====
    for (int i = 0; i < dashEffects.size(); i++) {
        dashEffects[i].radius += 2.5f;
        dashEffects[i].life--;
        if (dashEffects[i].life <= 0) {
            dashEffects.erase(dashEffects.begin() + i);
            i--;
        }
    }
}

void Player::Draw() {
    // ===== HIỆU ỨNG DASH =====
    for (auto &e : dashEffects) {
        float alpha = e.life / 20.0f;
        DrawCircleLines(
            e.position.x,
            e.position.y,
            e.radius,
            Fade(SKYBLUE, alpha)
        );
    }

    // ===== PLAYER =====
    float speedLen = sqrt(velocity.x*velocity.x + velocity.y*velocity.y);
    float scale = 1.0f + speedLen * 0.03f;

    // hướng di chuyển
    Vector2 dir = {0, 0};
    if (speedLen > 0) {
        dir.x = velocity.x / speedLen;
        dir.y = velocity.y / speedLen;
    }

    // scale theo hướng
    float stretch = isDashing ? 1.8f : 1.0f;
    float squash = isDashing ? 0.6f : 1.0f;
    // xoay theo hướng
    float angle = atan2(dir.y, dir.x) * RAD2DEG;

    DrawRectanglePro(
        {
            position.x,
            position.y,
            size * stretch,
            size * squash
        },
        {
            (size * stretch)/2,
            (size * squash)/2
        },
        angle,
        SKYBLUE
    );
}