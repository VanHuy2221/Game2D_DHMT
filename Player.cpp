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
            float len = sqrtf(input.x*input.x + input.y*input.y);
            input.x /= len;
            input.y /= len;
            velocity.x = input.x * 15;
            velocity.y = input.y * 15;

            // 💥 VU NO TAI VI TRI NHAN VAT
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

// ============================================================
//  Ham noi bo: tinh 4 goc cua hinh chu nhat sau khi xoay
//  De DrawRectanglePro (co xoay) bang Bresenham, minh tinh
//  toa do 4 dinh roi ve 4 canh + scanline fill thu cong.
//
//  Cho hinh chu nhat kich thuoc (w x h), tam tai origin:
//    Goc TL = (-w/2, -h/2), TR = (w/2, -h/2)
//    BL = (-w/2,  h/2), BR = (w/2,  h/2)
//  Sau do xoay moi goc theo angle va tinh theo pivot.
// ============================================================
static void RotatedRectCorners(
    float cx, float cy,       // tam hinh chu nhat
    float w,  float h,        // kich thuoc
    float angleDeg,           // goc xoay (degree)
    Vector2 out[4]            // tra ve 4 dinh: TL, TR, BR, BL
) {
    float rad = angleDeg * DEG2RAD;
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    // 4 goc local (pivot tai tam)
    float lx[4] = { -w/2,  w/2,  w/2, -w/2 };
    float ly[4] = { -h/2, -h/2,  h/2,  h/2 };

    for (int i = 0; i < 4; i++) {
        out[i].x = cx + lx[i] * cosA - ly[i] * sinA;
        out[i].y = cy + lx[i] * sinA + ly[i] * cosA;
    }
}

// ============================================================
//  Ham noi bo: ve tu giac dac (convex) bang scanline.
//  Nhan 4 dinh theo thu tu (TL, TR, BR, BL).
//  Chia thanh 2 tam giac: (0,1,2) va (0,2,3) roi scanline fill.
// ============================================================
static void DrawQuadScanline(Vector2 v[4], Color color) {
    DrawTriangleScanline(v[0], v[1], v[2], color);
    DrawTriangleScanline(v[0], v[2], v[3], color);
}

// ============================================================
//  Ham noi bo: ve vien tu giac bang Bresenham (4 canh).
// ============================================================
static void DrawQuadOutlineBresenham(Vector2 v[4], Color color) {
    DrawLineBresenham((int)v[0].x, (int)v[0].y, (int)v[1].x, (int)v[1].y, color);
    DrawLineBresenham((int)v[1].x, (int)v[1].y, (int)v[2].x, (int)v[2].y, color);
    DrawLineBresenham((int)v[2].x, (int)v[2].y, (int)v[3].x, (int)v[3].y, color);
    DrawLineBresenham((int)v[3].x, (int)v[3].y, (int)v[0].x, (int)v[0].y, color);
}

void Player::Draw() {
    float speedLen = sqrtf(velocity.x*velocity.x + velocity.y*velocity.y);

    // Huong di chuyen
    Vector2 dir = {0, 0};
    if (speedLen > 0.001f) {
        dir.x = velocity.x / speedLen;
        dir.y = velocity.y / speedLen;
    }

    // Stretch/squash khi dash (giu nguyen y thiet ke cu)
    float stretch = isDashing ? 1.8f : 1.0f;
    float squash  = isDashing ? 0.6f : 1.0f;
    float angle   = atan2f(dir.y, dir.x) * RAD2DEG;

    float w = size * stretch;
    float h = size * squash;

    // ===== HIEU UNG DASH (vong no lan ra) =====
    // Thay DrawCircleLines bang DrawCircleMidpoint (outline Bresenham)
    // De them do day nhin ro hon khi alpha con cao
    for (auto &e : dashEffects) {
        float alpha = e.life / 20.0f;
        Color ringColor = Fade(SKYBLUE, alpha);
        // Ve 2 vong sat nhau tao cam giac day -> nhin dep hon DrawCircleLines
        DrawCircleMidpoint((int)e.position.x, (int)e.position.y, (int)e.radius,       ringColor);
        DrawCircleMidpoint((int)e.position.x, (int)e.position.y, (int)e.radius + 1,   Fade(SKYBLUE, alpha * 0.5f));
    }

    // ===== THAN PLAYER (hinh chu nhat xoay) =====
    // DrawRectanglePro cu dung ham co san cua raylib.
    // O day tu tinh 4 goc sau khi xoay, roi:
    //   1. Fill bang 2 tam giac (DrawTriangleScanline) - ap dung TP2
    //   2. Ve vien bang Bresenham (DrawLineBresenham)   - ap dung TP2

    Vector2 corners[4];
    RotatedRectCorners(position.x, position.y, w, h, angle, corners);

    // Fill than (mau SKYBLUE)
    DrawQuadScanline(corners, SKYBLUE);

    // Vien ngoai (mau toi hon SKYBLUE de co chieu sau)
    DrawQuadOutlineBresenham(corners, Fade(DARKBLUE, 0.7f));

    // Diem sang (highlight) o goc tren-trai than de them chieu sau
    // => Dung DrawCircleFilledMidpoint: 1 diem nho tron mau trang
    DrawCircleFilledMidpoint(
        (int)(corners[0].x * 0.6f + position.x * 0.4f),
        (int)(corners[0].y * 0.6f + position.y * 0.4f),
        (int)(size * 0.12f),
        Fade(WHITE, 0.4f)
    );
}