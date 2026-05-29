#pragma once
#include "raylib.h"
#include <vector>
#include <stack>

// ============================================================
//  DrawUtils.h - Thu vien ve hinh tu cai dat
//  Cac thuat toan duoc tu cai dat bang DrawPixel,
//  khong dung cac ham ve san co cua Raylib (DrawLine, DrawCircle...)
// ============================================================

// ============================================================
//  PHAN 1: THUAT TOAN VE DUONG THANG - BRESENHAM LINE
//  Nguyen ly: thay vi tinh float, dung bien sai so nguyen (error)
//  de quyet dinh pixel nao can to, tranh phep nhan/chia so thuc
// ============================================================

// Ve duong thang tong quat (xu ly tat ca truong hop goc do)
// - |m| < 1  : buoc theo x, quyet dinh y bang error
// - |m| > 1  : buoc theo y, quyet dinh x bang error (hoan vi truc)
// - Xu ly ca 2 chieu am (dx < 0, dy < 0)
void DrawLineBresenham(int x1, int y1, int x2, int y2, Color color);

// Ve duong thang voi do day (thickness), dung cho khung UI
void DrawLineBresenhamThick(int x1, int y1, int x2, int y2, int thickness, Color color);

// Ve hinh chu nhat bang 4 canh Bresenham (dung cho BattleUI box)
void DrawRectBresenham(int x, int y, int w, int h, int thickness, Color color);

// ============================================================
//  PHAN 2: THUAT TOAN VE DUONG TRON - MIDPOINT CIRCLE
//  Nguyen ly: xet diem giua (midpoint) giua 2 pixel ung vien
//  De quyet dinh pixel nao nam tren duong tron hon
//  Dung tinh doi xung 8 cung (octant symmetry) de ve ca vong tron
//  chi tu 1/8 tinh toan
// ============================================================

// Ve vong tron rong (outline) bang Midpoint Circle
// Tan dung doi xung 8 cung: neu (x,y) tren duong tron
// thi (y,x), (-x,y), (x,-y)... cung tren duong tron
void DrawCircleMidpoint(int cx, int cy, int radius, Color color);

// To dac hinh tron: quet tung hang ngang giua 2 diem bien
void DrawCircleFilled(int cx, int cy, int radius, Color color);

// Ve vong tron voi do day (dung cho hao quang Boss)
void DrawCircleThick(int cx, int cy, int radius, int thickness, Color color);

// ============================================================
//  PHAN 3: THUAT TOAN TO MAU DA GIAC - FLOOD FILL
//  Nguyen ly: to mau tu diem bat dau, lan rong ra xung quanh
//  den khi gap mau bien (boundary color)
//  Dung stack thay vi de qui thuan tuy de tranh stack overflow
//  khi vung to lon
// ============================================================

// To mau theo bien (boundary fill) - dung stack khong de qui
// fillColor: mau can to vao
// boundaryColor: mau bien khong vuot qua
void FloodFillStack(int x, int y, Color fillColor, Color boundaryColor);

// ============================================================
//  PHAN 4: HAM HO TRO NOI BO
// ============================================================

// Kiem tra 2 mau co giong nhau khong (de dung trong flood fill)
bool ColorEquals(Color a, Color b);

// Ve tam giac dac bang Bresenham + FloodFill
// (Dung cho than Boss)
void DrawTriangleBresenham(Vector2 v1, Vector2 v2, Vector2 v3, Color fillColor, Color borderColor);