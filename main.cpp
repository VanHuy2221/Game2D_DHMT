#include "raylib.h"
#include "Player.h"
#include "Boss.h"

int main() {
    // Khởi tạo cửa sổ
    InitWindow(2000, 1200, "Simple Bullet Hell Game");
    SetTargetFPS(60);

    Player player({400, 300});
    Boss boss({1200, 500}, 200.0f);  

    // Game loop
    while (!WindowShouldClose()) {

        // ===== UPDATE =====
        player.Update();
        boss.Update();

        // ===== DRAW =====
        BeginDrawing();
        ClearBackground(BLACK);

        player.Draw();
        boss.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}