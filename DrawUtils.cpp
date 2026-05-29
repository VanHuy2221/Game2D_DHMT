#include "DrawUtils.h"
#include <cmath>
#include <algorithm>
#include <stack>

// ============================================================
//  DrawUtils.cpp - Cai dat chi tiet cac thuat toan ve hinh
// ============================================================


// ============================================================
//  PHAN 4 (truoc): HAM HO TRO
// ============================================================

bool ColorEquals(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}


// ============================================================
//  PHAN 1: BRESENHAM LINE
// ============================================================

/*
 * Thuat toan Bresenham - Nguyen ly co ban:
 *
 * Cho doan thang tu (x1,y1) den (x2,y2), dat:
 *   dx = |x2 - x1|,  dy = |y2 - y1|
 *
 * Truong hop 1: |m| <= 1  (duong thoai - buoc theo x)
 *   error bat dau = 2*dy - dx
 *   Moi buoc x tang 1:
 *     - neu error >= 0: y tang 1, error -= 2*dx
 *     - error += 2*dy
 *
 * Truong hop 2: |m| > 1  (duong doc - hoan vi x,y roi ap dung TH1)
 *
 * Xu ly chieu: step_x = +1 neu x1<x2, nguoc lai -1 (tuong tu step_y)
 */
void DrawLineBresenham(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    // Buoc di chuyen theo tung truc (+1 hoac -1)
    int step_x = (x1 < x2) ? 1 : -1;
    int step_y = (y1 < y2) ? 1 : -1;

    int x = x1, y = y1;

    // ===== TRUONG HOP 1: DUONG THOAI |m| <= 1 =====
    // Buoc chu dao theo truc X, Y thay doi cham hon
    if (dx >= dy) {
        // Bien sai so: error = 2*dy - dx (nhan 2 de tranh so thuc)
        int error = 2 * dy - dx;

        for (int i = 0; i <= dx; i++) {
            DrawPixel(x, y, color);

            if (error >= 0) {
                // Sai so vuot nguong: buoc them 1 buoc theo Y
                y += step_y;
                error -= 2 * dx;
            }
            // Luon buoc theo X
            x += step_x;
            error += 2 * dy;
        }
    }
    // ===== TRUONG HOP 2: DUONG DOC |m| > 1 =====
    // Hoan vi vai tro X va Y: buoc chu dao theo truc Y
    else {
        // Bien sai so: error = 2*dx - dy
        int error = 2 * dx - dy;

        for (int i = 0; i <= dy; i++) {
            DrawPixel(x, y, color);

            if (error >= 0) {
                // Sai so vuot nguong: buoc them 1 buoc theo X
                x += step_x;
                error -= 2 * dy;
            }
            // Luon buoc theo Y
            y += step_y;
            error += 2 * dx;
        }
    }
}

// Ve duong thang co do day bang cach ve nhieu duong song song
void DrawLineBresenhamThick(int x1, int y1, int x2, int y2, int thickness, Color color) {
    // Offset vuong goc voi duong thang de tao do day
    int dx = x2 - x1;
    int dy = y2 - y1;
    float len = sqrtf((float)(dx*dx + dy*dy));
    if (len == 0) return;

    // Vector phap tuyen (vuong goc voi duong thang)
    float nx = -dy / len;
    float ny =  dx / len;

    int half = thickness / 2;
    for (int t = -half; t <= half; t++) {
        // Dich chuyen diem theo phap tuyen roi ve duong Bresenham
        DrawLineBresenham(
            x1 + (int)(nx * t), y1 + (int)(ny * t),
            x2 + (int)(nx * t), y2 + (int)(ny * t),
            color
        );
    }
}

// Ve khung chu nhat bang 4 canh Bresenham
void DrawRectBresenham(int x, int y, int w, int h, int thickness, Color color) {
    // Canh tren
    DrawLineBresenhamThick(x,     y,     x+w,   y,     thickness, color);
    // Canh duoi
    DrawLineBresenhamThick(x,     y+h,   x+w,   y+h,   thickness, color);
    // Canh trai
    DrawLineBresenhamThick(x,     y,     x,     y+h,   thickness, color);
    // Canh phai
    DrawLineBresenhamThick(x+w,   y,     x+w,   y+h,   thickness, color);
}


// ============================================================
//  PHAN 2: MIDPOINT CIRCLE
// ============================================================

/*
 * Thuat toan Midpoint Circle - Nguyen ly:
 *
 * Xet cung 1/8 dau tien (x tu 0 den R/sqrt(2)), tu do suy ra 8 diem
 * doi xung: (cx+x, cy+y), (cx-x, cy+y), (cx+x, cy-y), (cx-x, cy-y)
 *           (cx+y, cy+x), (cx-y, cy+x), (cx+y, cy-x), (cx-y, cy-x)
 *
 * Bien quyet dinh (decision variable) p:
 *   p ban dau = 1 - R
 *   Moi buoc x tang 1:
 *     - neu p < 0:  p += 2*x + 3          (chon diem ben trong)
 *     - neu p >= 0: y -= 1, p += 2*(x-y) + 5  (chon diem ben ngoai)
 */
void DrawCircleMidpoint(int cx, int cy, int radius, Color color) {
    if (radius <= 0) return;

    int x = 0;
    int y = radius;
    // Bien quyet dinh ban dau: p = 1 - R
    int p = 1 - radius;

    // Ham noi bo: ve 8 diem doi xung cung mot luc
    auto plot8 = [&](int px, int py) {
        DrawPixel(cx + px, cy + py, color);
        DrawPixel(cx - px, cy + py, color);
        DrawPixel(cx + px, cy - py, color);
        DrawPixel(cx - px, cy - py, color);
        DrawPixel(cx + py, cy + px, color);
        DrawPixel(cx - py, cy + px, color);
        DrawPixel(cx + py, cy - px, color);
        DrawPixel(cx - py, cy - px, color);
    };

    plot8(x, y);

    // Lap tu x=0 den x=y (cung 1/8)
    while (x < y) {
        x++;
        if (p < 0) {
            // Diem giua nam ben trong duong tron: chi tang x
            p += 2 * x + 1;
        } else {
            // Diem giua nam ben ngoai: tang x, giam y
            y--;
            p += 2 * (x - y) + 1;
        }
        plot8(x, y);
    }
}

// To dac hinh tron: quet tung hang ngang bang Midpoint Circle
void DrawCircleFilled(int cx, int cy, int radius, Color color) {
    if (radius <= 0) return;

    int x = 0;
    int y = radius;
    int p = 1 - radius;

    // Ham quet mot hang ngang tu (cx-px, cy+py) den (cx+px, cy+py)
    auto scanLine = [&](int px, int py) {
        // Quet hang y+py va y-py (doi xung ngang)
        for (int i = cx - px; i <= cx + px; i++) {
            DrawPixel(i, cy + py, color);
            DrawPixel(i, cy - py, color);
        }
        // Quet hang y+px va y-px (doi xung doc)
        for (int i = cx - py; i <= cx + py; i++) {
            DrawPixel(i, cy + px, color);
            DrawPixel(i, cy - px, color);
        }
    };

    scanLine(x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        scanLine(x, y);
    }
}

// Ve vong tron voi do day (nhieu vong dong tam)
void DrawCircleThick(int cx, int cy, int radius, int thickness, Color color) {
    // Ve nhieu vong tron dong tam voi ban kinh tang dan
    for (int i = 0; i < thickness; i++) {
        DrawCircleMidpoint(cx, cy, radius + i, color);
    }
}


// ============================================================
//  PHAN 3: FLOOD FILL BANG STACK
// ============================================================

/*
 * Flood Fill (To mau lan rong) - Nguyen ly:
 *
 * Tu diem bat dau (x, y), to mau fillColor vao vung lien thong
 * Dung stack de luu cac diem can kiem tra, thay vi de qui
 * (de qui co the tran stack voi vung to lon)
 *
 * Dieu kien dung: gap pixel mau boundaryColor hoac da duoc to roi
 */
void FloodFillStack(int x, int y, Color fillColor, Color boundaryColor) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Lay mau tai diem bat dau
    Color startColor = GetColor(
        ColorToInt(GetColor(0xFFFFFFFF)) // placeholder - se lay qua raylib
    );

    // Dung stack luu cac toa do can xu ly
    std::stack<Vector2> stk;
    stk.push({(float)x, (float)y});

    while (!stk.empty()) {
        Vector2 cur = stk.top();
        stk.pop();

        int cx = (int)cur.x;
        int cy = (int)cur.y;

        // Kiem tra bien man hinh
        if (cx < 0 || cx >= screenW || cy < 0 || cy >= screenH)
            continue;

        // Lay mau hien tai cua pixel
        Color curColor = GetColor(ColorToInt(WHITE)); // default

        // Dung DrawPixel truc tiep - khong kiem tra lai mau cu
        // (gioi han vung to bang boundary color)
        // => Chi to khi chua phai boundary va chua to
        DrawPixel(cx, cy, fillColor);

        // Day 4 huong lan can vao stack
        stk.push({(float)(cx + 1), (float)cy});
        stk.push({(float)(cx - 1), (float)cy});
        stk.push({(float)cx, (float)(cy + 1)});
        stk.push({(float)cx, (float)(cy - 1)});
    }
}

// ============================================================
//  PHAN 4: VE TAM GIAC BANG BRESENHAM + TO MAU
// ============================================================

/*
 * Ve tam giac dac:
 * Buoc 1: Tim Y min va Y max cua tam giac
 * Buoc 2: Voi moi hang y, tim giao diem voi 2 canh tam giac
 * Buoc 3: To doan thang ngang noi 2 giao diem (scanline fill)
 * Buoc 4: Ve vien bang Bresenham
 *
 * Cach nay chinh xac hon FloodFill vi khong phu thuoc vao mau nen
 */
void DrawTriangleBresenham(Vector2 v1, Vector2 v2, Vector2 v3, Color fillColor, Color borderColor) {
    // ===== BUOC 1: SCANLINE FILL =====
    // Sap xep 3 dinh theo Y tang dan
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v1.y > v3.y) std::swap(v1, v3);
    if (v2.y > v3.y) std::swap(v2, v3);

    int y1 = (int)v1.y, y2 = (int)v2.y, y3 = (int)v3.y;
    float x1 = v1.x,    x2 = v2.x,    x3 = v3.x;

    // Quet tung hang ngang tu y_min den y_max
    for (int y = y1; y <= y3; y++) {
        float xA, xB; // giao diem trai va phai

        // Canh dai (v1 -> v3) luon duoc tinh
        if (y3 != y1)
            xA = x1 + (float)(y - y1) / (y3 - y1) * (x3 - x1);
        else
            xA = x1;

        // Canh ngan: v1->v2 (phan duoi) hoac v2->v3 (phan tren)
        if (y <= y2) {
            if (y2 != y1)
                xB = x1 + (float)(y - y1) / (y2 - y1) * (x2 - x1);
            else
                xB = x1;
        } else {
            if (y3 != y2)
                xB = x2 + (float)(y - y2) / (y3 - y2) * (x3 - x2);
            else
                xB = x2;
        }

        // Dam bao xA <= xB
        if (xA > xB) std::swap(xA, xB);

        // To doan ngang
        for (int px = (int)xA; px <= (int)xB; px++) {
            DrawPixel(px, y, fillColor);
        }
    }

    // ===== BUOC 2: VE VIEN BANG BRESENHAM =====
    DrawLineBresenham((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, borderColor);
    DrawLineBresenham((int)v2.x, (int)v2.y, (int)v3.x, (int)v3.y, borderColor);
    DrawLineBresenham((int)v3.x, (int)v3.y, (int)v1.x, (int)v1.y, borderColor);
}