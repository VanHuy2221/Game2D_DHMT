#pragma once
#include "raylib.h"
#include "Player.h"
#include "Boss.h"
#include "BattleUI.h"
#include "Bullet.h"
#include "EnergyOrb.h"
#include "Fractal.h"      // <-- FRACTAL TP3

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
    Player        player;
    Boss          boss;
    BattleUI      ui;
    BulletManager bullets;
    EnergySystem  energy;

    // Trang thai game
    int   invincibleTimer;
    bool  gameOver;
    bool  playerWin;
    int   bossHp;
    int   bossMaxHp;
    int   score;
    int   bossHitFlash;

    // Animation timer cho Menu fractal background
    float menuAnimTime;

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

    void ResetGame();
    void DrawHUD();
    void DrawChargeIndicator();
};