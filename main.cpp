#include "raylib.h"
#include "Player.h"
#include "Boss.h"
#include "BattleUI.h"

int main() {
    // Khởi tạo cửa sổ
    InitWindow(2000, 1200, "Simple Bullet Hell Game");
    SetTargetFPS(60);

	BattleUI ui;
	
    Vector2 center = {
	    ui.box.x + ui.box.width / 2,
	    ui.box.y + ui.box.height / 2
	};
	
	Player player(center);
    Boss boss({1000, 200}, 200.0f);

    // Game loop
    while (!WindowShouldClose()) {

        // ===== UPDATE =====
        player.Update();
        boss.Update();
        ui.Update();

		// GIỚI HẠN PLAYER TRONG BOX
		float halfSize = player.size / 2;

		if (player.position.x < ui.box.x + halfSize)
		    player.position.x = ui.box.x + halfSize;
		
		if (player.position.x > ui.box.x + ui.box.width - halfSize)
		    player.position.x = ui.box.x + ui.box.width - halfSize;
		
		if (player.position.y < ui.box.y + halfSize)
		    player.position.y = ui.box.y + halfSize;
		
		if (player.position.y > ui.box.y + ui.box.height - halfSize)
		    player.position.y = ui.box.y + ui.box.height - halfSize;

        // ===== DRAW =====
        BeginDrawing();
        ClearBackground(BLACK);

        player.Draw();
        boss.Draw();
        ui.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}