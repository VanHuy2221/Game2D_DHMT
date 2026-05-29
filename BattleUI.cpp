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
    // To nen den mo (giu nguyen, khong phai ve duong thang)
    DrawRectangleRec(box, Fade(BLACK, 0.3f));

    // Ve 4 canh khung bang Bresenham (thay DrawRectangleLinesEx cu)
    // Do day 4px, mau trang
    DrawRectBresenham(
        (int)box.x, (int)box.y,
        (int)box.width, (int)box.height,
        4, WHITE
    );

    // ===== 2. UI PHIA DUOI KHUNG =====
    int baseY = (int)(box.y + box.height) + 40;

    // Ten nhan vat va cap do (dung DrawText thuan - khong phai bai tap ve)
    DrawText(playerName.c_str(),            (int)box.x,       baseY, 30, WHITE);
    DrawText(TextFormat("LV %d", level),    (int)box.x + 200, baseY, 30, WHITE);

    // ===== 3. HP BAR =====
    float hpRatio  = (float)hp / maxHp;
    int   barWidth  = 200;
    int   barHeight = 20;
    int   barX      = (int)box.x + 350;
    int   barY      = baseY + 5;

    // --- Nen xam (tong chieu dai thanh mau) ---
    // To bang Bresenham: ve nhieu duong ngang chong len nhau
    for (int row = 0; row < barHeight; row++) {
        DrawLineBresenham(barX, barY + row, barX + barWidth, barY + row, DARKGRAY);
    }

    // --- Phan mau do (theo ti le HP hien tai) ---
    int fillWidth = (int)(barWidth * hpRatio);
    for (int row = 0; row < barHeight; row++) {
        DrawLineBresenham(barX, barY + row, barX + fillWidth, barY + row, RED);
    }

    // --- Vien ngoai HP bar bang Bresenham ---
    DrawRectBresenham(barX, barY, barWidth, barHeight, 2, WHITE);

    // --- Text HP ---
    DrawText(
        TextFormat("%d / %d", hp, maxHp),
        barX + barWidth + 20,
        barY - 5,
        20, WHITE
    );
}