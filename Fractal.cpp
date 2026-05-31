#include "Fractal.h"
#include "DrawUtils.h"
#include <cmath>

// ============================================================
//  Fractal.cpp  -  Cai dat chi tiet Hinh hoc Fractal
//
//  Nguyen tac chung:
//    Moi ham fractal deu la de quy thuan tuy.
//    Tang cu the (depth) -> hinh phuc tap hon.
//    depth == 0 -> ve doan thang truc tiep bang DrawLineBresenham.
//
//  Khong dung DrawPixel, khong dung ham ve co san cua raylib
//  (tru DrawLineBresenham da tu cai dat o DrawUtils.cpp).
// ============================================================

static const float PI_F = 3.14159265f;

// ============================================================
//  HELPER: Xoay diem P xung quanh tam O mot goc angleDeg
//  Cong thuc xoay 2D:
//    x' = (x-ox)*cos(a) - (y-oy)*sin(a) + ox
//    y' = (x-ox)*sin(a) + (y-oy)*cos(a) + oy
// ============================================================
static Vector2 RotatePoint(Vector2 p, Vector2 origin, float angleDeg) {
    float rad  = angleDeg * PI_F / 180.0f;
    float cosA = cosf(rad);
    float sinA = sinf(rad);
    float dx   = p.x - origin.x;
    float dy   = p.y - origin.y;
    return {
        origin.x + dx * cosA - dy * sinA,
        origin.y + dx * sinA + dy * cosA
    };
}


// ============================================================
//  PHAN 1: KOCH CURVE
//
//  Nguyen ly de quy:
//    Cho doan AB:
//      P1 = A + (B-A) * 1/3          <- diem 1/3
//      P2 = A + (B-A) * 2/3          <- diem 2/3
//      M  = Xoay P1 quanh trung diem P1P2 mot goc -60 do
//           (hoac xoay P2 quanh trung diem +60 do)
//           => cho ta dinh tam giac deu dung tren doan P1P2
//    De quy: Koch(A,P1) | Koch(P1,M) | Koch(M,P2) | Koch(P2,B)
//    Dung lai o depth == 0: ve thang doan AB
// ============================================================
void DrawKochCurve(Vector2 a, Vector2 b, int depth, Color color) {
    // ---- Truong hop co so: depth = 0 -> ve doan thang AB ----
    if (depth == 0) {
        DrawLineBresenham((int)a.x, (int)a.y,
                          (int)b.x, (int)b.y, color);
        return;
    }

    // ---- Tinh 3 diem trung gian ----

    // Diem 1/3 cua doan AB
    Vector2 p1 = {
        a.x + (b.x - a.x) / 3.0f,
        a.y + (b.y - a.y) / 3.0f
    };

    // Diem 2/3 cua doan AB
    Vector2 p2 = {
        a.x + (b.x - a.x) * 2.0f / 3.0f,
        a.y + (b.y - a.y) * 2.0f / 3.0f
    };

    // Dinh tam giac deu: xoay p1 quanh p2 mot goc -60 do
    // (chieu xoay -60 de dinh nho ra phia ngoai tam giac Koch)
    Vector2 mid = {
        (p1.x + p2.x) * 0.5f,
        (p1.y + p2.y) * 0.5f
    };
    Vector2 peak = RotatePoint(p1, mid, -60.0f);

    // ---- De quy 4 doan con ----
    DrawKochCurve(a,    p1,   depth - 1, color);  // Doan 1: A -> P1
    DrawKochCurve(p1,   peak, depth - 1, color);  // Doan 2: P1 -> Dinh
    DrawKochCurve(peak, p2,   depth - 1, color);  // Doan 3: Dinh -> P2
    DrawKochCurve(p2,   b,    depth - 1, color);  // Doan 4: P2 -> B
}

// ============================================================
//  Koch Snowflake = 3 Koch Curve khep lai thanh tam giac deu
//
//  3 dinh cua tam giac deu ngoai tiep duong tron ban kinh size:
//    V0 = center + (0,    -size)       <- dinh tren
//    V1 = center + (+size*sin60, +size*cos60) <- duoi phai
//    V2 = center + (-size*sin60, +size*cos60) <- duoi trai
//
//  Ve 3 canh: V0->V1, V1->V2, V2->V0 bang Koch Curve
// ============================================================
void DrawKochSnowflake(float cx, float cy, float size,
                       int depth, Color color) {
    float sin60 = sqrtf(3.0f) / 2.0f;  // = 0.866

    Vector2 v0 = { cx,                   cy - size         };  // Dinh tren
    Vector2 v1 = { cx + size * sin60,    cy + size * 0.5f  };  // Duoi phai
    Vector2 v2 = { cx - size * sin60,    cy + size * 0.5f  };  // Duoi trai

    // Ve 3 canh theo chieu kim dong ho (chinh la 3 Koch Curve)
    DrawKochCurve(v0, v1, depth, color);
    DrawKochCurve(v1, v2, depth, color);
    DrawKochCurve(v2, v0, depth, color);
}


// ============================================================
//  PHAN 2: DRAGON CURVE  (Duong cong Rong)
//
//  Nguyen ly de quy:
//    Cho doan AB va bien turn (1 = phai, -1 = trai):
//      M = diem "gai" = trung diem AB xoay 90 do
//          -> turn =  1: xoay +45 (sang phai)
//          -> turn = -1: xoay -45 (sang trai)
//    De quy:
//      DrawDragon(A, M, depth-1,  1)  <- luot sau xoay phai
//      DrawDragon(M, B, depth-1, -1)  <- luot sau xoay trai
//    Dung lai o depth == 0: ve thang doan AB
//
//  "turn" xen ke nhau tao ra hinh dang cuon xoan dac trung
//  cua Dragon Curve.
// ============================================================
void DrawDragonCurve(Vector2 a, Vector2 b, int depth, int turn,
                     Color color) {
    // ---- Truong hop co so ----
    if (depth == 0) {
        DrawLineBresenham((int)a.x, (int)a.y,
                          (int)b.x, (int)b.y, color);
        return;
    }

    // ---- Tinh diem gai M ----
    // Trung diem cua AB
    Vector2 mid = {
        (a.x + b.x) * 0.5f,
        (a.y + b.y) * 0.5f
    };

    // Xoay A quanh trung diem 45 do (turn = +1 hoac -1)
    // Cong thuc: xoay A quanh mid mot goc turn*45
    float angleDeg = turn * 45.0f;
    Vector2 m = RotatePoint(a, mid, angleDeg);

    // ---- De quy hai nhanh ----
    DrawDragonCurve(a, m, depth - 1,  1, color);  // Nhanh 1: luon xoay phai
    DrawDragonCurve(m, b, depth - 1, -1, color);  // Nhanh 2: luon xoay trai
}


// ============================================================
//  PHAN 3: C-CURVE  (Duong cong C)
//
//  Nguyen ly de quy:
//    Cho doan AB:
//      M = diem giua AB sau khi xoay 90 do (luon cung chieu)
//          Tinh: M = Xoay A quanh trung diem AB mot goc 90 do
//    De quy:
//      DrawCCurve(A, M, depth-1)
//      DrawCCurve(M, B, depth-1)
//    Dung lai o depth == 0: ve thang doan AB
//
//  Khac voi Dragon: C-Curve luon xoay cung 1 chieu
//  -> tao hinh dang xoay oc dac trung
// ============================================================
void DrawCCurve(Vector2 a, Vector2 b, int depth, Color color) {
    // ---- Truong hop co so ----
    if (depth == 0) {
        DrawLineBresenham((int)a.x, (int)a.y,
                          (int)b.x, (int)b.y, color);
        return;
    }

    // ---- Tinh diem gai M ----
    // Trung diem cua AB
    Vector2 mid = {
        (a.x + b.x) * 0.5f,
        (a.y + b.y) * 0.5f
    };

    // Xoay A quanh trung diem 90 do (luon cung chieu)
    Vector2 m = RotatePoint(a, mid, 90.0f);

    // ---- De quy hai nhanh ----
    DrawCCurve(a, m, depth - 1, color);
    DrawCCurve(m, b, depth - 1, color);
}


// ============================================================
//  PHAN 4: HAM UNG DUNG VAO GAME
// ============================================================

// ------------------------------------------------------------
//  DrawFractalBackground()
//
//  Ve 4 Koch Snowflake nho o 4 goc cua Battle Box lam trang tri.
//  + Ve Koch Curve doc theo 4 canh cua box (thay vien trang).
//
//  Animation:
//    bobTime duoc lay tu Boss.bobTime -> snowflake "nhip"
//    cung voi animation cua Boss, tao cam giac dong nhat.
//
//  Mau sac:
//    ORANGE / GOLD -> dong bo voi mau Boss va orb nang luong
// ------------------------------------------------------------
void DrawFractalBackground(Rectangle box, float bobTime) {
    // ---- 1. Koch Snowflake o 4 goc box ----
    // Size snowflake nhip theo bobTime (to - nho theo nhip boss)
    float pulse = 1.0f + sinf(bobTime * 1.5f) * 0.08f;
    float snowSize = 38.0f * pulse;
    int   snowDepth = 3;  // depth = 3: dep nhung nhe

    float margin = snowSize * 1.1f;

    // Goc tren-trai
    DrawKochSnowflake(box.x + margin,              box.y + margin,
                      snowSize, snowDepth, Fade(ORANGE, 0.55f));
    // Goc tren-phai
    DrawKochSnowflake(box.x + box.width - margin,  box.y + margin,
                      snowSize, snowDepth, Fade(GOLD, 0.55f));
    // Goc duoi-trai
    DrawKochSnowflake(box.x + margin,              box.y + box.height - margin,
                      snowSize, snowDepth, Fade(GOLD, 0.55f));
    // Goc duoi-phai
    DrawKochSnowflake(box.x + box.width - margin,  box.y + box.height - margin,
                      snowSize, snowDepth, Fade(ORANGE, 0.55f));

    // ---- 2. Koch Curve doc theo 4 canh box (thay vien trang) ----
    // depth = 2 de giup canh khong bi tran ra ngoai box
    int edgeDepth = 2;
    float edgeAlpha = 0.45f + sinf(bobTime * 0.9f) * 0.12f;

    // Canh tren: trai -> phai
    Vector2 tl = { box.x,              box.y              };
    Vector2 tr = { box.x + box.width,  box.y              };
    Vector2 bl = { box.x,              box.y + box.height };
    Vector2 br = { box.x + box.width,  box.y + box.height };

    DrawKochCurve(tl, tr, edgeDepth, Fade(YELLOW, edgeAlpha));   // Tren
    DrawKochCurve(br, bl, edgeDepth, Fade(YELLOW, edgeAlpha));   // Duoi (nguoc chieu)
    DrawKochCurve(bl, tl, edgeDepth, Fade(ORANGE, edgeAlpha));   // Trai
    DrawKochCurve(tr, br, edgeDepth, Fade(ORANGE, edgeAlpha));   // Phai
}


// ------------------------------------------------------------
//  DrawFractalHitEffect()
//
//  Khi Boss bi trung don (bossHitFlash > 0):
//    Ve 4 Dragon Curve toa ra tu Boss theo 4 huong cheo.
//    Moi Dragon Curve bat dau tu gan tam Boss, ket thuc cach
//    tam Boss mot khoang bang bossSize.
//
//  Hieu ung:
//    - flashTimer giam tu 20 -> 0: alpha cung giam theo
//    - Size cua Dragon Curve tang dan (toa ra) khi flashTimer giam
//      => visual "no tung" khi bi trung
//
//  Mau sac:
//    WHITE / YELLOW (flash manh) -> do dang tan dan
// ------------------------------------------------------------
void DrawFractalHitEffect(Vector2 bossPos, float bossSize, int flashTimer) {
    if (flashTimer <= 0) return;

    // Alpha fade out: flashTimer 20->0 tuong ung alpha 1->0
    float alpha = (float)flashTimer / 20.0f;

    // Dragon Curve size tang dan khi flashTimer giam
    // (hieu ung toa ra ngoai)
    float spread = bossSize * (1.0f - alpha) * 1.4f + bossSize * 0.5f;

    // depth = 8: du phuc tap de trong dep, khong treo may
    int depth = 8;

    // ---- 4 Dragon Curve toa ra 4 huong cheo ----
    // Diem bat dau: gan tam Boss (cach boss.size/4)
    float inner = bossSize * 0.25f;

    // Cheo tren-phai
    Vector2 a1 = { bossPos.x + inner,  bossPos.y - inner  };
    Vector2 b1 = { bossPos.x + spread, bossPos.y - spread };
    DrawDragonCurve(a1, b1, depth, 1, Fade(WHITE,  alpha));

    // Cheo tren-trai
    Vector2 a2 = { bossPos.x - inner,  bossPos.y - inner  };
    Vector2 b2 = { bossPos.x - spread, bossPos.y - spread };
    DrawDragonCurve(a2, b2, depth, -1, Fade(YELLOW, alpha * 0.85f));

    // Cheo duoi-phai
    Vector2 a3 = { bossPos.x + inner,  bossPos.y + inner  };
    Vector2 b3 = { bossPos.x + spread, bossPos.y + spread };
    DrawDragonCurve(a3, b3, depth, -1, Fade(ORANGE, alpha * 0.80f));

    // Cheo duoi-trai
    Vector2 a4 = { bossPos.x - inner,  bossPos.y + inner  };
    Vector2 b4 = { bossPos.x - spread, bossPos.y + spread };
    DrawDragonCurve(a4, b4, depth,  1, Fade(WHITE,  alpha * 0.75f));
}


// ------------------------------------------------------------
//  DrawFractalMenuBG()
//
//  Ve Koch Snowflake va C-Curve lam nen man hinh Menu.
//  animTime tang moi frame -> fractal xoay cham tao hieu ung
//  song dong, thu hut nguoi choi.
//
//  Bo cuc:
//    - 1 Snowflake lon o trung tam (lam "logo" nen)
//    - 6 Snowflake nho xep vong tron xung quanh (xoay quanh tam)
//    - 4 C-Curve o 4 goc man hinh (trang tri)
//
//  Mau sac:
//    DARKPURPLE / PURPLE -> tone toi huyen bi, phu hop chu de
// ------------------------------------------------------------
void DrawFractalMenuBG(float animTime) {
    int screenW = 2000;
    int screenH = 1200;
    float cx    = screenW * 0.5f;
    float cy    = screenH * 0.5f;

    // ---- 1. Snowflake lon trung tam ----
    // Nhip sang theo animTime
    float bigPulse = 1.0f + sinf(animTime * 0.8f) * 0.06f;
    DrawKochSnowflake(cx, cy, 260.0f * bigPulse, 3,
                      Fade(DARKPURPLE, 0.22f));
    // Lop trong nho hon, xoay nguoc chieu
    DrawKochSnowflake(cx, cy, 160.0f * bigPulse, 3,
                      Fade(PURPLE, 0.18f));

    // ---- 2. 6 Snowflake nho xoay quanh tam ----
    // Cac snowflake xep tren duong tron ban kinh 380px
    // Goc xoay tinh luy theo animTime
    float orbitR = 380.0f;
    int   numOrb = 6;
    for (int i = 0; i < numOrb; i++) {
        float angle = animTime * 0.25f + i * (2.0f * PI_F / numOrb);
        float sx    = cx + cosf(angle) * orbitR;
        float sy    = cy + sinf(angle) * orbitR * 0.60f;  // ep lai theo Y tao hieu ung phoi canh

        float smallPulse = 1.0f + sinf(animTime * 1.2f + i * 0.9f) * 0.12f;
        float smallAlpha = 0.30f + sinf(animTime + i) * 0.10f;

        DrawKochSnowflake(sx, sy, 55.0f * smallPulse, 3,
                          Fade(PURPLE, smallAlpha));
    }

    // ---- 3. C-Curve o 4 goc man hinh ----
    // Moi goc co 1 cap C-Curve doi xung tao hinh dang xoay oc
    // depth = 10: don gian nhanh, du dep
    int cDepth  = 10;
    float cSize = 180.0f;
    float cAlpha = 0.20f + sinf(animTime * 0.6f) * 0.07f;

    // Goc tren-trai
    DrawCCurve({0.0f,           0.0f},
               {cSize,          cSize},          cDepth, Fade(PURPLE, cAlpha));

    // Goc tren-phai
    DrawCCurve({(float)screenW, 0.0f},
               {screenW - cSize, cSize},         cDepth, Fade(PURPLE, cAlpha));

    // Goc duoi-trai
    DrawCCurve({0.0f,           (float)screenH},
               {cSize,          screenH - cSize}, cDepth, Fade(PURPLE, cAlpha));

    // Goc duoi-phai
    DrawCCurve({(float)screenW, (float)screenH},
               {screenW - cSize, screenH - cSize}, cDepth, Fade(PURPLE, cAlpha));
}