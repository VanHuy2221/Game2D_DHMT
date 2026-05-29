#pragma once
#include "raylib.h"
#include "Player.h"
#include "Boss.h"
#include "BattleUI.h"

enum GameScreen {
    MENU,
    GUIDE,
    RANK,
    GAMEPLAY
};

class GameManager {
public:
    GameScreen currentScreen;

    // Game objects
    Player player;
    Boss boss;
    BattleUI ui;

    GameManager();

    void Update();
    void Draw();

private:
    void UpdateMenu();
    void DrawMenu();

    void UpdateGuide();
    void DrawGuide();

    void UpdateRank();
    void DrawRank();

    void UpdateGame();
    void DrawGame();
};