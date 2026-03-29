#include "BattleUI.h"

BattleUI::BattleUI() {
    // Battle box (giống Undertale)
    box = {700, 450, 600, 600};

    playerName = "HUY";
    level = 1;
    hp = 100;
    maxHp = 100;
}

void BattleUI::Update() {
    // sau này có thể thêm animation UI
}

void BattleUI::Draw() {
    // ===== VẼ KHUNG BATTLE =====
    DrawRectangleLinesEx(box, 4, WHITE);

    // ===== UI BÊN DƯỚI =====
    int baseY = box.y + box.height + 40;

    DrawText(playerName.c_str(), box.x, baseY, 30, WHITE);

    DrawText(TextFormat("LV %d", level), box.x + 200, baseY, 30, WHITE);

    // ===== HP BAR =====
    float hpRatio = (float)hp / maxHp;
    int barWidth = 200;
    int barHeight = 20;

    int barX = box.x + 350;
    int barY = baseY + 5;

    // nền
    DrawRectangle(barX, barY, barWidth, barHeight, DARKGRAY);

    // máu
    DrawRectangle(barX, barY, barWidth * hpRatio, barHeight, RED);

    // text HP
    DrawText(
        TextFormat("%d / %d", hp, maxHp),
        barX + barWidth + 20,
        barY - 5,
        20,
        WHITE
    );
}