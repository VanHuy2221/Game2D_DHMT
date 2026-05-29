#include "GameManager.h"

GameManager::GameManager()
: player({0,0}), boss({1000, 200}, 200.0f)
{
    currentScreen = MENU;

    // spawn player gi?a box
    player.position = {
        ui.box.x + ui.box.width / 2,
        ui.box.y + ui.box.height / 2
    };
}

void GameManager::UpdateMenu() {
    if (IsKeyPressed(KEY_ONE)) currentScreen = GAMEPLAY;
    if (IsKeyPressed(KEY_TWO)) currentScreen = GUIDE;
    if (IsKeyPressed(KEY_THREE)) currentScreen = RANK;
    if (IsKeyPressed(KEY_ESCAPE)) CloseWindow();
}

void GameManager::DrawMenu() {
    ClearBackground(BLACK);

    DrawText("UNDERTALE STYLE GAME", 700, 200, 40, YELLOW);

    DrawText("1. PLAY", 850, 350, 30, WHITE);
    DrawText("2. GUIDE", 850, 420, 30, WHITE);
    DrawText("3. RANK", 850, 490, 30, WHITE);
    DrawText("ESC. EXIT", 850, 560, 30, RED);
}

void GameManager::UpdateGuide() {
    player.Update();

    if (IsKeyPressed(KEY_BACKSPACE))
        currentScreen = MENU;
}

void GameManager::DrawGuide() {
    ClearBackground(BLACK);

    DrawText("GUIDE", 900, 100, 40, YELLOW);

    DrawText("W A S D: MOVE", 800, 200, 30, WHITE);
    DrawText("SPACE: DASH", 800, 250, 30, WHITE);
    DrawText("BACKSPACE: BACK", 800, 300, 30, GRAY);

    player.Draw();
}

void GameManager::UpdateRank() {
    if (IsKeyPressed(KEY_BACKSPACE))
        currentScreen = MENU;
}

void GameManager::DrawRank() {
    ClearBackground(BLACK);

    DrawText("RANKING", 850, 100, 40, YELLOW);

    DrawText("1 MIN: 5000 - HUY", 700, 250, 30, WHITE);
    DrawText("2 MIN: 3200 - TRI", 700, 300, 30, WHITE);
    DrawText("3 MIN: 1500 - PRO", 700, 350, 30, WHITE);

    DrawText("BACKSPACE: BACK", 700, 450, 25, GRAY);
}

void GameManager::UpdateGame() {
    player.Update();
    boss.Update();
    ui.Update();

    float halfSize = player.size * 0.8f;

    if (player.position.x < ui.box.x + halfSize)
        player.position.x = ui.box.x + halfSize;

    if (player.position.x > ui.box.x + ui.box.width - halfSize)
        player.position.x = ui.box.x + ui.box.width - halfSize;

    if (player.position.y < ui.box.y + halfSize)
        player.position.y = ui.box.y + halfSize;

    if (player.position.y > ui.box.y + ui.box.height - halfSize)
        player.position.y = ui.box.y + ui.box.height - halfSize;

    if (IsKeyPressed(KEY_BACKSPACE))
        currentScreen = MENU;
}

void GameManager::DrawGame() {
    ClearBackground(BLACK);

    boss.Draw();
    player.Draw();
    ui.Draw();
}

void GameManager::Update() {
    switch (currentScreen) {
        case MENU: UpdateMenu(); break;
        case GUIDE: UpdateGuide(); break;
        case RANK: UpdateRank(); break;
        case GAMEPLAY: UpdateGame(); break;
    }
}

void GameManager::Draw() {
    switch (currentScreen) {
        case MENU: DrawMenu(); break;
        case GUIDE: DrawGuide(); break;
        case RANK: DrawRank(); break;
        case GAMEPLAY: DrawGame(); break;
    }
}