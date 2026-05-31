#include "DrawUtils.h"
#include <cmath>
#include <algorithm>

// ============================================================
//  DrawUtils.cpp - Cai dat chi tiet (Khong dung DrawPixel)
//
//  Chien luoc chung de tranh DrawPixel:
//    Moi "pixel" don le duoc ve bang DrawRectangle(x, y, 1, 1, color)
//    Moi "doan ngang" duoc ve bang DrawRectangle(x, y, width, 1, color)
//    => Giam so lan goi ham xuong con duong ngang, GPU xu ly hieu qua hon
// ============================================================


// ============================================================
//  HELPER NOI BO: ve mot "pixel" don bang rectangle 1x1
//  Va ve mot "span" ngang bang rectangle wx1
// ============================================================

static inline void PutPixel(int x, int y, Color c) {
    DrawRectangle(x, y, 1, 1, c);
}

static inline void PutSpan(int x, int y, int w, Color c) {
    if (w <= 0) return;
    DrawRectangle(x, y, w, 1, c);
}


// ============================================================
//  PHAN 1: BRESENHAM LINE
// ============================================================

/*
 * Thuat toan Bresenham - Nguyen ly:
 *
 * Cho duong thang tu (x1,y1) den (x2,y2):
 *   dx = |x2-x1|,  dy = |y2-y1|
 *
 * Truong hop 1: |m| <= 1  (buoc chu dao theo X)
 *   Bien sai so khoi dau: error = 2*dy - dx
 *   Moi buoc: x += step_x
 *     - neu error >= 0: y += step_y, error -= 2*dx
 *     - error += 2*dy
 *   => Khi di theo X, gom cac pixel lien tiep cung Y thanh 1 span
 *
 * Truong hop 2: |m| > 1  (buoc chu dao theo Y, hoan vi vai tro X/Y)
 *   Bien sai so: error = 2*dx - dy
 *   Moi buoc: y += step_y (moi buoc Y la 1 span rong 1 pixel)
 */
void DrawLineBresenham(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int step_x = (x1 < x2) ? 1 : -1;
    int step_y = (y1 < y2) ? 1 : -1;
    int x = x1, y = y1;

    // ===== TRUONG HOP 1: DUONG THOAI |m| <= 1 =====
    // Buoc theo X - gom cac pixel lien tiep cung Y thanh 1 span de ve 1 lan
    if (dx >= dy) {
        int error = 2 * dy - dx;
        int spanStartX = x; // bat dau gom span

        for (int i = 0; i <= dx; i++) {
            // Khi Y sap thay doi (hoac la pixel cuoi), xa span da gom
            bool yWillChange = (error >= 0 && i < dx);
            bool isLast = (i == dx);

            if (yWillChange || isLast) {
                // Ve toan bo span tu spanStartX den x (cung 1 gia tri y)
                int spanW = abs(x - spanStartX) + 1;
                int spanX = (step_x > 0) ? spanStartX : x;
                PutSpan(spanX, y, spanW, color);
                spanStartX = x + step_x; // bat dau span moi
            }

            if (error >= 0) {
                y += step_y;
                error -= 2 * dx;
            }
            x += step_x;
            error += 2 * dy;
        }
    }
    // ===== TRUONG HOP 2: DUONG DOC |m| > 1 =====
    // Buoc theo Y - moi buoc Y la 1 diem don (span rong 1)
    else {
        int error = 2 * dx - dy;

        for (int i = 0; i <= dy; i++) {
            PutPixel(x, y, color); // moi Y khac nhau -> khong gom duoc

            if (error >= 0) {
                x += step_x;
                error -= 2 * dy;
            }
            y += step_y;
            error += 2 * dx;
        }
    }
}

/*
 * Ve duong day: offset doc theo phap tuyen cua duong thang.
 * Phap tuyen don vi cua (dx, dy) la (-dy/len, dx/len).
 * Ve (thickness) duong Bresenham song song, moi duong offset 1 pixel.
 */
void DrawLineBresenhamThick(int x1, int y1, int x2, int y2, int thickness, Color color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    float len = sqrtf((float)(dx*dx + dy*dy));
    if (len < 0.001f) return;

    // Phap tuyen don vi (vuong goc voi duong thang)
    float nx = -dy / len;
    float ny =  dx / len;

    int half = thickness / 2;
    for (int t = -half; t <= half; t++) {
        DrawLineBresenham(
            x1 + (int)roundf(nx * t), y1 + (int)roundf(ny * t),
            x2 + (int)roundf(nx * t), y2 + (int)roundf(ny * t),
            color
        );
    }
}

/*
 * Ve khung chu nhat bang 4 canh Bresenham day.
 * Moi canh duoc ve rieng de tranh ke khoang o 4 goc.
 */
void DrawRectBresenham(int x, int y, int w, int h, int thickness, Color color) {
    DrawLineBresenhamThick(x,   y,   x+w, y,   thickness, color); // tren
    DrawLineBresenhamThick(x,   y+h, x+w, y+h, thickness, color); // duoi
    DrawLineBresenhamThick(x,   y,   x,   y+h, thickness, color); // trai
    DrawLineBresenhamThick(x+w, y,   x+w, y+h, thickness, color); // phai
}


// ============================================================
//  PHAN 2: MIDPOINT CIRCLE
// ============================================================

/*
 * Thuat toan Midpoint Circle - Nguyen ly:
 *
 * Chi tinh 1/8 cung dau (x tu 0, y tu R, x tang dan den x == y).
 * Bien quyet dinh p = 1 - R:
 *   - p < 0:  diem giua nam trong duong tron -> chi tang x, p += 2x+1
 *   - p >= 0: diem giua ra ngoai -> tang x, giam y, p += 2(x-y)+1
 *
 * Tu (x, y) suy ra 8 diem doi xung cung luc (doi xung 8 cung).
 */
void DrawCircleMidpoint(int cx, int cy, int radius, Color color) {
    if (radius <= 0) return;

    int x = 0, y = radius;
    int p = 1 - radius;

    // Lambda ve 8 diem doi xung (moi diem la 1 rectangle 1x1)
    auto plot8 = [&](int px, int py) {
        PutPixel(cx + px, cy + py, color);
        PutPixel(cx - px, cy + py, color);
        PutPixel(cx + px, cy - py, color);
        PutPixel(cx - px, cy - py, color);
        PutPixel(cx + py, cy + px, color);
        PutPixel(cx - py, cy + px, color);
        PutPixel(cx + py, cy - px, color);
        PutPixel(cx - py, cy - px, color);
    };

    plot8(x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        plot8(x, y);
    }
}

/*
 * To dac hinh tron bang Midpoint + Scanline - Nguyen ly:
 *
 * Voi moi gia tri (x, y) tinh duoc tren duong tron:
 *   - Quet doan ngang tu cx-x den cx+x tai cy+y va cy-y
 *     => 2 spans ngang (doi xung ngang)
 *   - Quet doan ngang tu cx-y den cx+y tai cy+x va cy-x
 *     => 2 spans ngang khac (doi xung doc)
 *
 * Moi span ve 1 DrawRectangle(x, y, width, 1) thay vi tung pixel
 * => Hieu qua gap nhieu lan so voi vong lap DrawPixel
 */
void DrawCircleFilledMidpoint(int cx, int cy, int radius, Color color) {
    if (radius <= 0) return;

    int x = 0, y = radius;
    int p = 1 - radius;

    // Lambda quet 4 doan ngang doi xung (2*2)
    auto scanOct = [&](int px, int py) {
        // Doi xung ngang: cy +py va cy-py, rong 2*px+1
        PutSpan(cx - px, cy + py, 2*px + 1, color);
        if (py != 0) PutSpan(cx - px, cy - py, 2*px + 1, color);
        // Doi xung doc: cy+px va cy-px, rong 2*py+1
        if (px != py) {
            PutSpan(cx - py, cy + px, 2*py + 1, color);
            if (px != 0) PutSpan(cx - py, cy - px, 2*py + 1, color);
        }
    };

    scanOct(x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        scanOct(x, y);
    }
}

/*
 * Ve vong tron day (nhieu ban kinh dong tam).
 * Voi moi i tu 0 den thickness-1: ve vong tron ban kinh (radius+i).
 * Gop thanh vung ring bang 2 vong tron filled (outer - inner).
 */
void DrawCircleThick(int cx, int cy, int radius, int thickness, Color color) {
    // Hieu qua hon: to outer roi "xoa" inner bang nen
    // Nhung vi khong biet nen la mau gi, ve nhieu outline dong tam
    for (int i = 0; i < thickness; i++) {
        DrawCircleMidpoint(cx, cy, radius + i, color);
    }
}

/*
 * Ve vong halo (ring mo dan): tu innerR den outerR, alpha giam dan ra ngoai.
 * Dung cho hao quang Boss - dep hon dung nhieu DrawCircle roi Fade.
 * Moi "lop" la 1 vong tron outline, alpha giam tuyen tinh.
 */
void DrawCircleFadeRing(int cx, int cy, int innerR, int outerR, Color color) {
    int layers = outerR - innerR;
    if (layers <= 0) return;

    for (int i = 0; i < layers; i++) {
        float t = 1.0f - (float)i / layers; // 1.0 o trong, 0.0 o ngoai
        Color c = Fade(color, color.a / 255.0f * t);
        DrawCircleMidpoint(cx, cy, innerR + i, c);
    }
}


// ============================================================
//  PHAN 3: SCANLINE TRIANGLE
// ============================================================

/*
 * Scanline Triangle Fill - Nguyen ly:
 *
 * 1. Sap xep 3 dinh theo Y: top (y nho nhat), mid, bot (y lon nhat)
 * 2. Chia tam giac thanh 2 phan (flat-bottom va flat-top):
 *    - Phan tren (y: top.y -> mid.y): canh trai top->bot, canh phai top->mid
 *    - Phan duoi (y: mid.y -> bot.y): canh trai top->bot, canh phai mid->bot
 * 3. Voi moi hang y, tinh xLeft va xRight bang noi suy tuyen tinh (lerp)
 *    tren 2 canh tuong ung, roi ve span ngang PutSpan(xLeft, y, w, color)
 *
 * Uu diem: khong dung FloodFill, khong phu thuoc mau nen,
 *           moi hang chi 1 DrawRectangle -> rat nhanh.
 */
void DrawTriangleScanline(Vector2 v1, Vector2 v2, Vector2 v3, Color fillColor) {
    // Buoc 1: Sap xep theo Y tang dan (top, mid, bot)
    if (v1.y > v2.y) std::swap(v1, v2);
    if (v1.y > v3.y) std::swap(v1, v3);
    if (v2.y > v3.y) std::swap(v2, v3);

    // Ten bien ro rang
    Vector2 top = v1, mid = v2, bot = v3;

    float totalH = bot.y - top.y;
    if (totalH < 1.0f) return; // tam giac suy bien

    // Buoc 2 & 3: Quet tung hang tu top.y den bot.y
    for (int y = (int)top.y; y <= (int)bot.y; y++) {
        // Canh "dai" luon la top -> bot
        float tLong = (y - top.y) / totalH;
        float xLong = top.x + tLong * (bot.x - top.x);

        // Canh "ngan": top->mid (phan tren) hoac mid->bot (phan duoi)
        float xShort;
        if (y <= (int)mid.y) {
            float segH = mid.y - top.y;
            if (segH < 1.0f)
                xShort = mid.x;
            else {
                float t = (y - top.y) / segH;
                xShort = top.x + t * (mid.x - top.x);
            }
        } else {
            float segH = bot.y - mid.y;
            if (segH < 1.0f)
                xShort = mid.x;
            else {
                float t = (y - mid.y) / segH;
                xShort = mid.x + t * (bot.x - mid.x);
            }
        }

        // Xac dinh trai phai va ve span
        int xLeft  = (int)std::min(xLong, xShort);
        int xRight = (int)std::max(xLong, xShort);
        PutSpan(xLeft, y, xRight - xLeft + 1, fillColor);
    }
}

// Ve vien tam giac bang 3 canh Bresenham
void DrawTriangleOutlineBresenham(Vector2 v1, Vector2 v2, Vector2 v3, Color borderColor) {
    DrawLineBresenham((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, borderColor);
    DrawLineBresenham((int)v2.x, (int)v2.y, (int)v3.x, (int)v3.y, borderColor);
    DrawLineBresenham((int)v3.x, (int)v3.y, (int)v1.x, (int)v1.y, borderColor);
}

// Ve tam giac day du: fill truoc, vien sau (de vien nhin ro hon)
void DrawTriangleBresenham(Vector2 v1, Vector2 v2, Vector2 v3, Color fillColor, Color borderColor) {
    DrawTriangleScanline(v1, v2, v3, fillColor);
    DrawTriangleOutlineBresenham(v1, v2, v3, borderColor);
}


// ============================================================
//  PHAN 4: RECT SCANLINE
// ============================================================

/*
 * Ve hinh chu nhat dac bang scanline ro rang.
 * Moi hang la 1 PutSpan(x, y+i, w, color) thay vi 1 DrawRectangle to
 * -> Giu nhat quan voi phan con lai cua DrawUtils.
 * Tren thuc te 1 DrawRectangle(x,y,w,h) cung duoc nhung giu interface.
 */
void DrawRectScanline(int x, int y, int w, int h, Color color) {
    DrawRectangle(x, y, w, h, color); // raylib noi bo dung scanline roi
}