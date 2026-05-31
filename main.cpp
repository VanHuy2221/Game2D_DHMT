#include "raylib.h"
#include "GameManager.h"

int main() {
    InitWindow(2000, 1200, "Undertale Style Game");
    SetTargetFPS(60);

    GameManager game;

    while (!WindowShouldClose()) {
        game.Update();

        BeginDrawing();
        game.Draw();
        EndDrawing();
    }

    // Giai phong am thanh truoc khi dong cua so
    // (Can goi ham nay, nhung hien tai GameManager khong co destructor)
    // Cach don gian: them dong nay neu co the
    // Hoac de ham UnloadGameSounds se tu goi khi window close
    
    CloseWindow();
    return 0;
}