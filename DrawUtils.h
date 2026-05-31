#pragma once
#include "raylib.h"
#include <vector>

// ============================================================
//  DrawUtils.h - Thu vien ve hinh tu cai dat (Khong dung DrawPixel)
//
//  Thay vi ve tung pixel mot (rat cham), cac ham o day dung:
//    - DrawRectangle(x, y, w, 1, color)  : ve mot hang ngang w pixel
//    - DrawLine (noi bo raylib)           : KHONG dung - tu cai bang spans
//  => Moi "duong" duoc bieu dien bang tap hop cac doan ngang (horizontal spans)
//     Ve ca doan 1 lan thay vi tung pixel -> nhanh hon rat nhieu
// ============================================================


// ============================================================
//  PHAN 1: THUAT TOAN VE DUONG THANG - BRESENHAM LINE
//
//  Nguyen ly Bresenham:
//    Thay vi tinh float y = m*x + b moi buoc (ton kem),
//    dung bien nguyen "error" tich luy sai so.
//    Khi error vuot nguong -> buoc them 1 buoc theo truc phu.
//    Ket qua: chi dung phep cong/tru nguyen, rat nhanh.
//
//  Xu ly day duong (thickness):
//    Ve nhieu duong Bresenham song song doc theo phap tuyen
//    cua duong chinh, moi duong offset them 1 pixel.
// ============================================================

// Ve duong thang Bresenham tong quat (xu ly moi goc do va chieu)
// Dung DrawRectangle(x,y,1,1) cho diem don, nhung gom span ngang khi co the
void DrawLineBresenham(int x1, int y1, int x2, int y2, Color color);

// Ve duong thang co do day (thickness > 1)
// Offset theo phap tuyen cua duong thang
void DrawLineBresenhamThick(int x1, int y1, int x2, int y2, int thickness, Color color);

// Ve khung chu nhat voi do day bang 4 canh Bresenham
void DrawRectBresenham(int x, int y, int w, int h, int thickness, Color color);


// ============================================================
//  PHAN 2: THUAT TOAN VE DUONG TRON - MIDPOINT CIRCLE
//
//  Nguyen ly Midpoint:
//    Chi tinh 1/8 cung (octant dau), su dung doi xung 8 huong
//    de suy ra 8 diem dong thoi: (+x,+y), (-x,+y), (+x,-y)...
//    Bien quyet dinh p = 1 - R, cap nhat nguyen toan bo.
//
//  To dac (filled):
//    Thay vi ve 8 diem roi FloodFill (cham),
//    voi moi gia tri (x,y) tren duong tron -> quet doan ngang
//    tu cx-x den cx+x (va cx-y den cx+y) bang 1 DrawRectangle.
//    => To nguyen ca hinh tron ma khong can FloodFill.
// ============================================================

// Ve vong tron rong (outline) bang Midpoint Circle - 8 diem doi xung
void DrawCircleMidpoint(int cx, int cy, int radius, Color color);

// To dac hinh tron bang Midpoint + scanline (khong dung FloodFill)
void DrawCircleFilledMidpoint(int cx, int cy, int radius, Color color);

// Ve vong tron co do day (nhieu vong dong tam)
void DrawCircleThick(int cx, int cy, int radius, int thickness, Color color);

// Ve vong tron mo dan (alpha giam tu trong ra ngoai) - dung cho hao quang Boss
void DrawCircleFadeRing(int cx, int cy, int innerR, int outerR, Color color);


// ============================================================
//  PHAN 3: VE TAM GIAC - SCANLINE FILL
//
//  Nguyen ly Scanline Triangle Fill:
//    1. Sap xep 3 dinh theo Y tang dan: top, mid, bot
//    2. Voi moi hang ngang y (tu top.y den bot.y):
//       - Tinh giao diem voi canh trai va canh phai cua tam giac
//       - Ve doan ngang noi 2 giao diem bang 1 DrawRectangle
//    => Khong can FloodFill, chinh xac va nhanh
//
//  Ve vien: sau khi fill, ve 3 canh bang DrawLineBresenham
// ============================================================

// Ve tam giac dac bang scanline fill (nhanh, khong FloodFill)
void DrawTriangleScanline(Vector2 v1, Vector2 v2, Vector2 v3, Color fillColor);

// Ve vien tam giac bang Bresenham
void DrawTriangleOutlineBresenham(Vector2 v1, Vector2 v2, Vector2 v3, Color borderColor);

// Ve tam giac day du: fill + vien
void DrawTriangleBresenham(Vector2 v1, Vector2 v2, Vector2 v3, Color fillColor, Color borderColor);


// ============================================================
//  PHAN 4: VE CHU NHAT DAC - SCANLINE (thay the DrawRectangle)
//  Dung DrawRectangle(x, y, w, 1) de quan ly ro rang
//  (hien tai chi wrap, nhung giu interface thong nhat)
// ============================================================

void DrawRectScanline(int x, int y, int w, int h, Color color);