#include "EnergyOrb.h"
#include "DrawUtils.h"
#include <cmath>
#include <cstdlib>

static const float TAU = 6.28318530f;

// ============================================================
//  Constructor
// ============================================================
EnergySystem::EnergySystem(Rectangle box)
    : battleBox(box)
{
    orbsCollected  = 0;
    spawnTimer     = 0;
    spawnInterval  = 240;   // Spawn moi ~4 giay
    chargeGlow     = 0.0f;
}

// ============================================================
//  Reset
// ============================================================
void EnergySystem::Reset() {
    orbs.clear();
    attacks.clear();
    orbsCollected = 0;
    spawnTimer    = 0;
    chargeGlow    = 0.0f;
}

// ============================================================
//  SpawnOrb  -  Tao 1 khoi o vi tri ngau nhien trong box
//  (Tranh vung ria box 40px de khoi khong dinh vao tuong)
// ============================================================
void EnergySystem::SpawnOrb() {
    if ((int)orbs.size() >= 2) return;   // Toi da 2 khoi cung luc

    float margin = 60.0f;
    float rx = battleBox.x + margin
               + (rand() % (int)(battleBox.width  - margin * 2));
    float ry = battleBox.y + margin
               + (rand() % (int)(battleBox.height - margin * 2));

    // Tranh spawn de len nhau (khoang cach toi thieu 80px)
    for (auto &o : orbs) {
        float dx = o.position.x - rx;
        float dy = o.position.y - ry;
        if (sqrtf(dx*dx + dy*dy) < 80.0f) {
            rx = battleBox.x + margin
                 + (rand() % (int)(battleBox.width  - margin * 2));
            ry = battleBox.y + margin
                 + (rand() % (int)(battleBox.height - margin * 2));
        }
    }

    orbs.emplace_back(Vector2{rx, ry}, 18.0f, 360.0f);  // Song 6 giay
}

// ============================================================
//  CheckPickup  -  Kiem tra player co cham orb nao khong
//  Tra ve true neu nhat duoc it nhat 1 orb
// ============================================================
bool EnergySystem::CheckPickup(Vector2 playerPos, float playerRadius) {
    bool picked = false;
    for (auto &o : orbs) {
        if (!o.active) continue;
        float dx   = o.position.x - playerPos.x;
        float dy   = o.position.y - playerPos.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < o.radius + playerRadius + 6.0f) {
            o.active = false;
            orbsCollected++;
            picked = true;
        }
    }
    return picked;
}

// ============================================================
//  FireAttack  -  B?n don tan cong tu player -> Boss
// ============================================================
void EnergySystem::FireAttack(Vector2 from, Vector2 bossPos) {
    float dx  = bossPos.x - from.x;
    float dy  = bossPos.y - from.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) return;

    float speed = 8.0f;
    Vector2 vel = { dx / len * speed, dy / len * speed };

    // Life du de bay qua man hinh 2000px
    attacks.emplace_back(from, vel, 22.0f, 400.0f);
}

// ============================================================
//  Update
//  Tra ve tong damage can tru Boss HP trong frame nay
// ============================================================
int EnergySystem::Update(Vector2 playerPos, float playerRadius, Vector2 bossPos) {
    int damageDealt = 0;

    // ---- 1. Spawn orb theo timer ----
    spawnTimer++;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0;
        SpawnOrb();
    }

    // ---- 2. Update tung orb ----
    for (auto &o : orbs) {
        if (!o.active) continue;
        o.bobTime += 0.05f;
        o.life--;
        if (o.life <= 0) o.active = false;
    }
    // Xoa orb chet
    for (int i = (int)orbs.size() - 1; i >= 0; i--)
        if (!orbs[i].active) orbs.erase(orbs.begin() + i);

    // ---- 3. Kiem tra nhat ----
    if (CheckPickup(playerPos, playerRadius)) {
        chargeGlow = 1.0f;  // Flash hieu ung nhat duoc

        // Du 3 orb: ban don tan cong
        if (orbsCollected >= 3) {
            orbsCollected = 0;
            FireAttack(playerPos, bossPos);
        }
    }

    // Giam glow
    if (chargeGlow > 0.0f) chargeGlow -= 0.03f;
    if (chargeGlow < 0.0f) chargeGlow = 0.0f;

    // ---- 4. Update PlayerAttack ----
    for (auto &a : attacks) {
        if (!a.active) continue;
        a.glowTime += 0.18f;
        a.position.x += a.velocity.x;
        a.position.y += a.velocity.y;
        a.life--;

        // Kiem tra trung Boss (hitbox Boss ~size/2)
        float dx   = a.position.x - bossPos.x;
        float dy   = a.position.y - bossPos.y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < a.radius + 90.0f) {   // 90 = xap xi ban kinh va cham Boss
            a.active   = false;
            damageDealt += 30;
        }

        if (a.life <= 0) a.active = false;
    }
    // Xoa attack chet
    for (int i = (int)attacks.size() - 1; i >= 0; i--)
        if (!attacks[i].active) attacks.erase(attacks.begin() + i);

    return damageDealt;
}

// ============================================================
//  Draw
// ============================================================
void EnergySystem::Draw() {
    // ---- Ve cac EnergyOrb ----
    for (auto &o : orbs) {
        if (!o.active) continue;

        float lifeRatio = o.life / o.maxLife;
        // Fade in (0.9-1.0) va fade out (0-0.1)
        float alpha = 1.0f;
        if (lifeRatio > 0.88f) alpha = (1.0f - lifeRatio) / 0.12f;
        if (lifeRatio < 0.12f) alpha = lifeRatio / 0.12f;

        // Bob nhe len xuong
        float bob = sinf(o.bobTime) * 5.0f;
        int cx = (int)o.position.x;
        int cy = (int)(o.position.y + bob);
        int r  = (int)o.radius;

        // --- Lop glow ngoai cung (lon, mo) ---
        DrawCircleFilledMidpoint(cx, cy, r + 12, Fade(YELLOW,   alpha * 0.12f));
        DrawCircleFilledMidpoint(cx, cy, r +  7, Fade(YELLOW,   alpha * 0.22f));

        // --- Vong ngoai nhip sang ---
        float pulse = 1.0f + sinf(o.bobTime * 2.5f) * 0.12f;
        DrawCircleMidpoint(cx, cy, (int)(r * pulse) + 2, Fade(GOLD,   alpha * 0.55f));
        DrawCircleMidpoint(cx, cy, (int)(r * pulse),     Fade(YELLOW, alpha * 0.85f));

        // --- Than chinh ---
        DrawCircleFilledMidpoint(cx, cy, r,            Fade(GOLD,   alpha));
        DrawCircleFilledMidpoint(cx, cy, r - 4,        Fade(YELLOW, alpha));

        // --- Hat sang trung tam ---
        DrawCircleFilledMidpoint(cx - r/4, cy - r/4, r/3,
            Fade(WHITE, alpha * 0.75f));

        // --- Tia sang quay xung quanh (4 tia) ---
        for (int i = 0; i < 4; i++) {
            float ang = o.bobTime * 1.2f + i * (TAU / 4.0f);
            float rx1 = cx + cosf(ang) * (r + 1);
            float ry1 = cy + sinf(ang) * (r + 1);
            float rx2 = cx + cosf(ang) * (r + 7);
            float ry2 = cy + sinf(ang) * (r + 7);
            DrawLineBresenham((int)rx1, (int)ry1, (int)rx2, (int)ry2,
                Fade(YELLOW, alpha * 0.6f));
        }

        // --- Timer bar nho phia duoi orb (cho player biet con bao lau) ---
        int bw = r * 2;
        int bh = 4;
        int bx = cx - r;
        int by = cy + r + 8;
        DrawRectScanline(bx,    by, bw,                      bh, Fade(DARKGRAY, alpha * 0.6f));
        DrawRectScanline(bx,    by, (int)(bw * lifeRatio),   bh, Fade(YELLOW,   alpha * 0.85f));
        DrawRectBresenham(bx,   by, bw,                      bh, 1, Fade(WHITE, alpha * 0.4f));
    }

    // ---- Ve PlayerAttack ----
    for (auto &a : attacks) {
        if (!a.active) continue;

        float lifeRatio = a.life / a.maxLife;
        float alpha = 1.0f;
        if (lifeRatio < 0.1f) alpha = lifeRatio / 0.1f;

        float glow  = (sinf(a.glowTime) + 1.0f) * 0.5f;  // 0-1 nhip sang
        int   cx    = (int)a.position.x;
        int   cy    = (int)a.position.y;
        int   r     = (int)a.radius;

        // Duoi sang (trail): ve 3 vong nho phia sau
        float tx = a.position.x - a.velocity.x * 3.0f;
        float ty = a.position.y - a.velocity.y * 3.0f;
        DrawCircleFilledMidpoint((int)tx, (int)ty, r - 4, Fade(GOLD,   alpha * 0.40f));
        float tx2 = a.position.x - a.velocity.x * 6.0f;
        float ty2 = a.position.y - a.velocity.y * 6.0f;
        DrawCircleFilledMidpoint((int)tx2, (int)ty2, r - 8, Fade(YELLOW, alpha * 0.20f));

        // Glow ngoai
        DrawCircleFilledMidpoint(cx, cy, r + 8, Fade(YELLOW, alpha * (0.15f + glow * 0.15f)));
        DrawCircleFilledMidpoint(cx, cy, r + 4, Fade(GOLD,   alpha * (0.30f + glow * 0.20f)));

        // Vong ngoai
        DrawCircleMidpoint(cx, cy, r + 2, Fade(WHITE,  alpha * 0.70f));

        // Than chinh
        DrawCircleFilledMidpoint(cx, cy, r,            Fade(GOLD,   alpha));
        DrawCircleFilledMidpoint(cx, cy, r - 5,        Fade(YELLOW, alpha));
        DrawCircleFilledMidpoint(cx, cy, r - 9,        Fade(WHITE,  alpha * 0.80f));

        // Hat sang nho
        DrawCircleFilledMidpoint(cx - r/5, cy - r/5, r/4,
            Fade(WHITE, alpha * 0.90f));
    }
}