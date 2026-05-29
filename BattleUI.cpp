#include "BattleUI.h"
#include "DrawUtils.h"

BattleUI::BattleUI() {
    // Battle box (giong Undertale - khung trang o giua)
    box = {700, 450, 600, 600};

    playerName = "HUY";
    level = 1;
    hp    = 100;
    maxHp = 100;
}

void BattleUI::Update() {
    // Sau nay co the them animation UI
}

void BattleUI::Draw() {
    // ===== 1. KHUNG BATTLE BOX =====
    // Nen den mo ben trong khung (dung ham raylib truc tiep - khong phai bai tap ve)
    DrawRectangleRec(box, Fade(BLACK, 0.3f));

    // Ve 4 canh khung bang Bresenham day 4px, mau trang
    // => Day la noi ap dung DrawRectBresenham vao game
    DrawRectBresenham(
        (int)box.x, (int)box.y,
        (int)box.width, (int)box.height,
        4, WHITE
    );

    // ===== 2. UI PHIA DUOI KHUNG =====
    int baseY = (int)(box.y + box.height) + 40;

    // Ten nhan vat va cap do
    DrawText(playerName.c_str(),            (int)box.x,       baseY, 30, WHITE);
    DrawText(TextFormat("LV %d", level),    (int)box.x + 200, baseY, 30, WHITE);

    // ===== 3. HP BAR =====
    float hpRatio  = (float)hp / maxHp;
    int   barWidth  = 200;
    int   barHeight = 20;
    int   barX      = (int)box.x + 350;
    int   barY      = baseY + 5;

    // Nen xam (toan bo thanh HP) - dung DrawRectScanline (ham tu cai)
    DrawRectScanline(barX, barY, barWidth, barHeight, DARKGRAY);

    // Phan mau do (theo ti le HP hien tai) - dung DrawRectScanline
    int fillWidth = (int)(barWidth * hpRatio);
    if (fillWidth > 0)
        DrawRectScanline(barX, barY, fillWidth, barHeight, RED);

    // Vien ngoai HP bar bang Bresenham (ap dung thuat toan TP2)
    DrawRectBresenham(barX, barY, barWidth, barHeight, 2, WHITE);

    // Text HP
    DrawText(
        TextFormat("%d / %d", hp, maxHp),
        barX + barWidth + 20,
        barY - 5,
        20, WHITE
    );
}