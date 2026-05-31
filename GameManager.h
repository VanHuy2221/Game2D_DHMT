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

// Thêm enum cho các mức tốc độ (TP4)
enum SpeedLevel {
    SPEED_STOP = 0,      // Dung
    SPEED_SLOW = 1,      // Cham
    SPEED_NORMAL = 2,    // Nhanh
    SPEED_FAST = 3       // Nhanh hon
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

    // ========== TP4: BO SUNG ==========
    // Cac muc do toc do
    SpeedLevel currentSpeedLevel;
    float       speedValues[4];  // 0:stop, 1:slow, 2:normal, 3:fast
    
    // Am thanh
    Sound   hitSound;           // Tieng player bi trung dan
    Sound   collectOrbSound;    // Tieng nhat orb
    Sound   bossHitSound;       // Tieng ban trung boss
    Sound   dashSound;          // Tieng dash
    Music   battleMusic;        // Nhac nen trong battle
    Music   menuMusic;          // Nhac nen menu
    bool    soundLoaded;        // Kiem tra da load am thanh chua

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
    
    // ========== TP4: HAM MOI ==========
    void LoadGameSounds();      // Load tat ca am thanh
    void UnloadGameSounds();    // Giai phong am thanh
    void UpdateSpeedLevel();    // Cap nhat toc do tu phim bam
    void ApplySpeedLevel();     // Ap dung toc do vao player
    void DrawSpeedIndicator();  // Hien thi muc do toc do hien tai
};