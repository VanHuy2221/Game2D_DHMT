#include "Bullet.h"
#include "DrawUtils.h"
#include <cmath>
#include <cstdlib>   // rand()
#undef PI

// ============================================================
//  Hang so
// ============================================================
static const float PI = 3.14159265f;

// ============================================================
//  Constructor
// ============================================================
BulletManager::BulletManager(Rectangle box)
    : battleBox(box)
{
    patternTimer    = 0;
    patternDuration = 180;   // ~3 giay moi pattern
    shootTimer      = 0;
    shootInterval   = 12;    // ban moi 12 frame
    spiralAngle     = 0.0f;
    currentPattern  = PATTERN_RADIAL;
}

// ============================================================
//  Clear
// ============================================================
void BulletManager::Clear() {
    bullets.clear();
}

// ============================================================
//  PickNextPattern
// ============================================================
void BulletManager::PickNextPattern() {
    // Chon ngau nhien 1 trong 6 pattern
    currentPattern = (AttackPattern)(rand() % PATTERN_COUNT);
    patternTimer   = 0;
    shootTimer     = 0;
    spiralAngle    = 0.0f;

    // Tuy chinh interval theo pattern
    switch (currentPattern) {
        case PATTERN_RADIAL:  shootInterval = 40;  break;
        case PATTERN_AIMED:   shootInterval = 18;  break;
        case PATTERN_SPIRAL:  shootInterval = 5;   break;
        case PATTERN_CROSS:   shootInterval = 35;  break;
        case PATTERN_WAVE:    shootInterval = 8;   break;
        case PATTERN_RAIN:    shootInterval = 10;  break;
        default:              shootInterval = 20;  break;
    }
}

// ============================================================
//  Update
// ============================================================
void BulletManager::Update(Vector2 bossPos, Vector2 playerPos) {
    // ---- 1. Di chuyen va update tung vien dan ----
    for (auto &b : bullets) {
        if (!b.active) continue;

        // Wave: cong them dao dong vuong goc voi velocity
        if (b.type == BULLET_WAVE) {
            b.angle += 0.12f;
            // Vec phap tuyen cua velocity (xoay 90 do)
            float len = sqrtf(b.velocity.x*b.velocity.x + b.velocity.y*b.velocity.y);
            if (len > 0.001f) {
                float nx = -b.velocity.y / len;
                float ny =  b.velocity.x / len;
                float wave = sinf(b.angle + b.waveOffset) * 2.2f;
                b.position.x += b.velocity.x + nx * wave;
                b.position.y += b.velocity.y + ny * wave;
            } else {
                b.position.x += b.velocity.x;
                b.position.y += b.velocity.y;
            }
        }
        // Spiral: xoay nhe velocity moi frame
        else if (b.type == BULLET_SPIRAL) {
            float rotSpeed = 0.04f;
            float vx = b.velocity.x;
            float vy = b.velocity.y;
            b.velocity.x = vx * cosf(rotSpeed) - vy * sinf(rotSpeed);
            b.velocity.y = vx * sinf(rotSpeed) + vy * cosf(rotSpeed);
            b.position.x += b.velocity.x;
            b.position.y += b.velocity.y;
        }
        // Straight / Aimed: bay thang
        else {
            b.position.x += b.velocity.x;
            b.position.y += b.velocity.y;
        }

        b.life--;

        // Deactivate neu ra ngoai box hoac het life
        if (b.life <= 0 ||
            b.position.x < battleBox.x - 20 ||
            b.position.x > battleBox.x + battleBox.width  + 20 ||
            b.position.y < battleBox.y - 20 ||
            b.position.y > battleBox.y + battleBox.height + 20)
        {
            b.active = false;
        }
    }

    // ---- 2. Xoa dan da chet khoi vector ----
    for (int i = (int)bullets.size() - 1; i >= 0; i--) {
        if (!bullets[i].active)
            bullets.erase(bullets.begin() + i);
    }

    // ---- 3. Dieu khien luot ban ----
    patternTimer++;
    shootTimer++;

    // Doi pattern sau patternDuration frame
    if (patternTimer >= patternDuration) {
        Clear();
        PickNextPattern();
    }

    // Ban theo interval
    if (shootTimer >= shootInterval) {
        shootTimer = 0;

        switch (currentPattern) {
            case PATTERN_RADIAL:
                SpawnRadial(bossPos, 12, 3.8f, ORANGE);
                break;

            case PATTERN_AIMED:
                SpawnAimed(bossPos, playerPos, 5.0f, RED);
                // Ban them 2 vien lech chut
                {
                    Vector2 dx = {playerPos.x - bossPos.x, playerPos.y - bossPos.y};
                    float len = sqrtf(dx.x*dx.x + dx.y*dx.y);
                    if (len > 0.001f) {
                        float spread = 0.22f; // radian
                        // Vien trai
                        float a1 = atan2f(dx.y, dx.x) - spread;
                        Vector2 v1 = {cosf(a1)*4.5f, sinf(a1)*4.5f};
                        bullets.emplace_back(bossPos, v1, 7.0f, 300.0f,
                            Fade(RED, 0.6f), BULLET_AIMED);
                        // Vien phai
                        float a2 = atan2f(dx.y, dx.x) + spread;
                        Vector2 v2 = {cosf(a2)*4.5f, sinf(a2)*4.5f};
                        bullets.emplace_back(bossPos, v2, 7.0f, 300.0f,
                            Fade(RED, 0.6f), BULLET_AIMED);
                    }
                }
                break;

            case PATTERN_SPIRAL:
                SpawnSpiral(bossPos, 3, 4.2f, YELLOW);
                spiralAngle += 0.25f;
                break;

            case PATTERN_CROSS:
                SpawnCross(bossPos, 4.0f, WHITE);
                break;

            case PATTERN_WAVE:
                SpawnWave(3.5f, SKYBLUE);
                break;

            case PATTERN_RAIN:
                SpawnRain(3, 4.0f, Fade(PURPLE, 0.9f));
                break;

            default: break;
        }
    }
}

// ============================================================
//  CheckPlayerHit
// ============================================================
bool BulletManager::CheckPlayerHit(Vector2 playerPos, float playerHitRadius) {
    for (auto &b : bullets) {
        if (!b.active) continue;
        float dx = b.position.x - playerPos.x;
        float dy = b.position.y - playerPos.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < b.radius + playerHitRadius) {
            b.active = false;   // Dan bien mat khi trung
            return true;
        }
    }
    return false;
}

// ============================================================
//  Draw
// ============================================================
void BulletManager::Draw() {
    for (auto &b : bullets) {
        if (!b.active) continue;

        // Tinh alpha moi dan (fade in/out)
        float lifeRatio = b.life / b.maxLife;
        float alpha = 1.0f;
        if (lifeRatio > 0.85f) alpha = (1.0f - lifeRatio) / 0.15f;  // fade in
        if (lifeRatio < 0.15f) alpha = lifeRatio / 0.15f;            // fade out

        Color c = Fade(b.color, alpha);

        int r = (int)b.radius;
        int cx = (int)b.position.x;
        int cy = (int)b.position.y;

        // Vong sang ngoai (glow)
        DrawCircleFilledMidpoint(cx, cy, r + 3, Fade(b.color, alpha * 0.25f));
        // Than chinh
        DrawCircleFilledMidpoint(cx, cy, r, c);
        // Diem sang trung tam
        if (r >= 4)
            DrawCircleFilledMidpoint(cx - r/4, cy - r/4, r/3, Fade(WHITE, alpha * 0.7f));
    }
}

// ============================================================
//  SpawnRadial
// ============================================================
void BulletManager::SpawnRadial(Vector2 origin, int numBullets, float speed, Color c) {
    float step = 2.0f * PI / numBullets;
    for (int i = 0; i < numBullets; i++) {
        float angle = i * step;
        Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
        bullets.emplace_back(origin, vel, 8.0f, 240.0f, c, BULLET_STRAIGHT);
    }
}

// ============================================================
//  SpawnAimed
// ============================================================
void BulletManager::SpawnAimed(Vector2 origin, Vector2 target, float speed, Color c) {
    float dx = target.x - origin.x;
    float dy = target.y - origin.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) return;
    Vector2 vel = { dx/len * speed, dy/len * speed };
    bullets.emplace_back(origin, vel, 9.0f, 280.0f, c, BULLET_AIMED);
}

// ============================================================
//  SpawnSpiral
// ============================================================
void BulletManager::SpawnSpiral(Vector2 origin, int arms, float speed, Color c) {
    float step = 2.0f * PI / arms;
    for (int i = 0; i < arms; i++) {
        float angle = spiralAngle + i * step;
        Vector2 vel = { cosf(angle) * speed, sinf(angle) * speed };
        bullets.emplace_back(origin, vel, 7.0f, 260.0f, c, BULLET_SPIRAL);
    }
}

// ============================================================
//  SpawnCross
// ============================================================
void BulletManager::SpawnCross(Vector2 origin, float speed, Color c) {
    // 4 huong chinhL up, down, left, right
    float dirs[8][2] = {
        {0,-1}, {0,1}, {-1,0}, {1,0},        // +
        {0.707f,-0.707f}, {0.707f,0.707f},    // x
        {-0.707f,-0.707f}, {-0.707f,0.707f}
    };
    for (int i = 0; i < 8; i++) {
        Vector2 vel = { dirs[i][0] * speed, dirs[i][1] * speed };
        bullets.emplace_back(origin, vel, 8.0f, 240.0f, c, BULLET_STRAIGHT);
    }
}

// ============================================================
//  SpawnWave  -  ban tu 2 canh trai / phai cua box
// ============================================================
void BulletManager::SpawnWave(float speed, Color c) {
    static float wavePhase = 0.0f;
    wavePhase += 0.3f;

    // Tu canh trai ban sang phai
    float yLeft = battleBox.y + battleBox.height * 0.5f
                  + sinf(wavePhase) * battleBox.height * 0.38f;
    Vector2 posL = { battleBox.x, yLeft };
    Vector2 velL = { speed, 0 };
    float offL = wavePhase;
    bullets.emplace_back(posL, velL, 7.0f, 300.0f, c, BULLET_WAVE, offL);

    // Tu canh phai ban sang trai
    float yRight = battleBox.y + battleBox.height * 0.5f
                   + sinf(wavePhase + PI) * battleBox.height * 0.38f;
    Vector2 posR = { battleBox.x + battleBox.width, yRight };
    Vector2 velR = { -speed, 0 };
    float offR = wavePhase + PI;
    bullets.emplace_back(posR, velR, 7.0f, 300.0f,
        Fade(c, 0.8f), BULLET_WAVE, offR);
}

// ============================================================
//  SpawnRain  -  mua dan tu tren xuan trong box
// ============================================================
void BulletManager::SpawnRain(int count, float speed, Color c) {
    for (int i = 0; i < count; i++) {
        float rx = battleBox.x + 10
                   + (rand() % (int)(battleBox.width - 20));
        Vector2 pos = { rx, battleBox.y };
        Vector2 vel = { 0, speed };
        bullets.emplace_back(pos, vel, 8.0f, 280.0f, c, BULLET_STRAIGHT);
    }
}