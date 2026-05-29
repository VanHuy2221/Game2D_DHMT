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

    CloseWindow();
    return 0;
}