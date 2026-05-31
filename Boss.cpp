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

// ============================================================
//  Ham ve 1 tinh tu tai vi tri (sx, sy) voi kich thuoc va alpha
//  Tach ra de goi duoc 2 lan (truoc va sau boss)
// ============================================================
static void DrawOrbitStar(float sx, float sy, float starSize, float alpha) {
    DrawCircleFilledMidpoint((int)sx, (int)sy, (int)(starSize * 1.3f), Fade(ORANGE, alpha * 0.6f));
    DrawCircleFilledMidpoint((int)sx, (int)sy, (int)starSize,          Fade(YELLOW, alpha));
}

void Boss::Draw() {
    Vector2 c = position;
    float bob = sinf(bobTime) * 8.0f;

    // Tinh truoc toa do va thong so cua tat ca tinh tu
    int   numOrbit = 6;
    float orbitR   = size * 0.62f;
    float starSize = size * 0.055f;

    // --- Tang 3: Glow nen ---
    for (int i = 0; i < 3; i++) {
        float r     = size * (0.88f + i * 0.09f);
        float alpha = 0.18f - i * 0.05f;
        DrawCircleThick((int)c.x, (int)(c.y + bob), (int)r, 4, Fade(ORANGE, alpha));
    }

    // --- Tang 1: Vong pulse ---
    int   numPulse  = 5;
    float pulseBase = size * 0.45f;
    float pulseMax  = size * 0.82f;

    for (int i = 0; i < numPulse; i++) {
        float phase = bobTime * 2.2f - i * (3.14159f * 2.0f / numPulse);
        float t     = (sinf(phase) + 1.0f) * 0.5f;
        float r     = pulseBase + t * (pulseMax - pulseBase);
        float alpha = sinf(t * 3.14159f) * 0.55f;
        Color ringColor = (i % 2 == 0) ? ORANGE : YELLOW;
        DrawCircleMidpoint((int)c.x, (int)(c.y + bob), (int)r, Fade(ringColor, alpha));
        if (r > 1)
            DrawCircleMidpoint((int)c.x, (int)(c.y + bob), (int)r - 1, Fade(ringColor, alpha * 0.6f));
    }

    // ============================================================
    //  TINH TU PHIA SAU BOSS (sin(angle) < 0 => dang o phia sau)
    //  Ve TRUOC tam giac => bi tam giac de len => hieu ung bi che
    //  Them: giam alpha va kich thuoc de trong "xa" hon
    // ============================================================
    for (int i = 0; i < numOrbit; i++) {
        float angle   = bobTime * 1.1f + i * (3.14159f * 2.0f / numOrbit);
        float sinA    = sinf(angle);
        if (sinA >= 0) continue; // bo qua tinh tu phia truoc

        float sx = c.x + cosf(angle) * orbitR;
        float sy = c.y + bob + sinA * orbitR * 0.55f;

        float brightness = (sinf(angle - bobTime) + 1.0f) * 0.5f;
        // Tinh tu phia sau: mo hon va nho hon (cach xa)
        float alpha    = (0.25f + brightness * 0.2f) * (1.0f + sinA); // sinA am nen giam alpha
        float sizeBack = starSize * (0.6f + (-sinA) * 0.0f);           // nho hon chut
        DrawOrbitStar(sx, sy, sizeBack * 0.75f, alpha);
    }

    // ============================================================
    //  THAN TAM GIAC (ve GIUA => de len tinh tu phia sau)
    // ============================================================
    Vector2 top   = { c.x + rotation * 0.35f, c.y - size * 0.62f + bob };
    Vector2 left  = { c.x - size * 0.53f,     c.y + size * 0.44f + bob };
    Vector2 right = { c.x + size * 0.53f,     c.y + size * 0.44f + bob };

    DrawTriangleBresenham(top, left, right, GOLD, Fade(BLACK, 0.9f));
    DrawTriangleScanline(top, left, right, Fade(YELLOW, 0.35f));

    // ============================================================
    //  TINH TU PHIA TRUOC BOSS (sin(angle) >= 0 => dang o phia truoc)
    //  Ve SAU tam giac => de len boss => hieu ung noi bat phia truoc
    //  Giu nguyen alpha va kich thuoc goc
    // ============================================================
    for (int i = 0; i < numOrbit; i++) {
        float angle = bobTime * 1.1f + i * (3.14159f * 2.0f / numOrbit);
        float sinA  = sinf(angle);
        if (sinA < 0) continue; // bo qua tinh tu phia sau

        float sx = c.x + cosf(angle) * orbitR;
        float sy = c.y + bob + sinA * orbitR * 0.55f;

        float brightness = (sinf(angle - bobTime) + 1.0f) * 0.5f;
        float alpha      = 0.4f + brightness * 0.45f;
        DrawOrbitStar(sx, sy, starSize, alpha);
    }

    // ============================================================
    //  CORE NANG LUONG (ve sau cung)
    // ============================================================
    float coreX      = c.x + rotation * 0.15f;
    float coreY      = c.y + bob * 0.4f;
    float coreRadius = size * 0.19f;
    float corePulse  = 1.0f + sinf(bobTime * 3.0f) * 0.08f;
    float cr         = coreRadius * corePulse;

    DrawCircleThick((int)coreX, (int)coreY, (int)(cr * 1.55f), 3, Fade(YELLOW, 0.55f));
    DrawCircleThick((int)coreX, (int)coreY, (int)(cr * 1.25f), 2, Fade(ORANGE, 0.45f));
    DrawCircleFilledMidpoint((int)coreX, (int)coreY, (int)cr,           RED);
    DrawCircleFilledMidpoint((int)coreX, (int)coreY, (int)(cr * 0.68f), YELLOW);
    DrawCircleFilledMidpoint(
        (int)(coreX - cr * 0.38f),
        (int)(coreY - cr * 0.38f),
        (int)(cr * 0.28f),
        WHITE
    );
}