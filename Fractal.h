#pragma once
#include "raylib.h"

// ============================================================
//  Fractal.h  -  He thong Hinh hoc Fractal cho Game
//
//  Cac loai Fractal duoc cai dat:
//    1. Koch Curve / Snowflake  (duong cong Koch)
//    2. Dragon Curve            (duong cong Rong)
//    3. C-Curve                 (duong cong C)
//
//  Ung dung trong game:
//    - DrawFractalBackground()  : Nen Battle Box (Koch)
//    - DrawFractalMenuBG()      : Nen man hinh Menu (Koch quay)
//    - DrawFractalHitEffect()   : Hieu ung khi Boss bi trung (Dragon)
//
//  Tat ca ham deu dung DrawLineBresenham() tu DrawUtils
//  => nhat quan voi toan bo he thong ve cua project
// ============================================================


// ============================================================
//  NHOM 1: HAM FRACTAL CO BAN (DE QUY)
//
//  Cac ham nay la "nhan" cua TP3, duoc ghi chep day du
//  de the hien kha nang hieu va cai dat thuat toan.
// ============================================================

// ------------------------------------------------------------
//  Koch Curve  (duong cong Koch)
//
//  Y tuong: Cho doan thang AB, chia thanh 3 phan bang nhau.
//    - Diem 1/3: P1 = A + (B-A)/3
//    - Diem 2/3: P2 = A + 2*(B-A)/3
//    - Dinh tam giac deu: M = trung diem P1P2 + xoay 60 do ra ngoai
//  De quy: goi Koch(A,P1), Koch(A,M,P1 goc), Koch(M,P2), Koch(P2,B)
//  Dung lai: depth == 0 -> ve doan thang AB
//
//  depth khuyen nghi: 4~5 (dep, khong treo may)
// ------------------------------------------------------------
void DrawKochCurve(Vector2 a, Vector2 b, int depth, Color color);

// Ve Koch Snowflake (3 canh Koch hop thanh tam giac deu khep kin)
// centerX, centerY: tam tam giac    size: ban kinh ngoai tiep
void DrawKochSnowflake(float centerX, float centerY, float size,
                       int depth, Color color);

// ------------------------------------------------------------
//  Dragon Curve  (duong cong Rong)
//
//  Y tuong: Cho doan thang AB, tim diem M = xoay diem giua AB
//           mot goc 45 do ra ngoai, noi AM va MB.
//  De quy:
//    - Luot le  (odd):  xoay sang PHAI -> M o ben phai AB
//    - Luot chan (even): xoay sang TRAI -> M o ben trai AB
//  Dung lai: depth == 0 -> ve doan thang AB
//
//  depth khuyen nghi: 10~12 (dep, khong treo may)
// ------------------------------------------------------------
void DrawDragonCurve(Vector2 a, Vector2 b, int depth, int turn,
                     Color color);

// ------------------------------------------------------------
//  C-Curve  (duong cong C)
//
//  Y tuong: Don gian hon Dragon Curve.
//    Cho doan AB, tim M = xoay trung diem AB 90 do
//    (luon cung chieu). De quy goi C(A,M) va C(M,B).
//  Dung lai: depth == 0 -> ve doan thang AB
//
//  depth khuyen nghi: 10~12
// ------------------------------------------------------------
void DrawCCurve(Vector2 a, Vector2 b, int depth, Color color);


// ============================================================
//  NHOM 2: HAM UNG DUNG VAO GAME
//
//  Goi cac ham fractal co ban o tren, bo sung mau sac
//  va vi tri phu hop voi tung boi canh game.
// ============================================================

// ------------------------------------------------------------
//  DrawFractalBackground()
//
//  Ung dung: Ve vien battle box bang Koch Snowflake.
//  Thay cho vien trang don gian -> tao cam giac "nang luong"
//  xung quanh khung chien dau, phu hop chu de Undertale.
//
//  box      : Battle box hien tai
//  bobTime  : Lay tu Boss.bobTime de animation dong bo voi boss
// ------------------------------------------------------------
void DrawFractalBackground(Rectangle box, float bobTime);

// ------------------------------------------------------------
//  DrawFractalHitEffect()
//
//  Ung dung: Ve Dragon Curve toa ra tu Boss khi bi trung don.
//  Goi khi bossHitFlash > 0 trong GameManager.
//
//  bossPos   : Vi tri tam Boss
//  bossSize  : Kich thuoc Boss (lam ban kinh toa ra)
//  flashTimer: gia tri bossHitFlash hien tai (20..0)
//              -> dung de tinh alpha fade out
// ------------------------------------------------------------
void DrawFractalHitEffect(Vector2 bossPos, float bossSize, int flashTimer);

// ------------------------------------------------------------
//  DrawFractalMenuBG()
//
//  Ung dung: Ve Koch Curve lam nen man hinh Menu.
//  animTime : tang moi frame (lay tu bien tinh trong GameManager)
//             -> fractal xoay cham tao hieu ung song dong
// ------------------------------------------------------------
void DrawFractalMenuBG(float animTime);