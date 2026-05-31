#include "GameManager.h"
#include "DrawUtils.h"
#include "Fractal.h"
#include <cstdlib>
#include <cmath>

// ============================================================
//  HELPER: TẠO SOUND EFFECT BẰNG CODE (KHÔNG CẦN FILE)
// ============================================================

// Tạo sóng vuông (Square Wave) - thích hợp cho tiếng "beep", "hit"
static Sound GenerateSquareWaveSound(int frequency, float duration, float volume) {
    int sampleRate = 44100;
    int sampleCount = (int)(sampleRate * duration);
    int channels = 1;          // Mono
    int sampleSize = 16;       // 16-bit

    short *samples = (short*)malloc(sampleCount * sizeof(short));
    if (!samples) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for square wave sound");
        return LoadSound("");  // Sound rỗng
    }

    int period = sampleRate / frequency;
    for (int i = 0; i < sampleCount; i++) {
        // Sóng vuông: nửa chu kỳ đầu giá trị dương, nửa sau âm
        short value = ((i % period) < (period / 2)) 
                      ? (short)(32767 * volume) 
                      : (short)(-32768 * volume);
        samples[i] = value;
    }

    Wave wave = {0};
    wave.data = samples;
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = sampleSize;
    wave.channels = channels;

    Sound sound = LoadSoundFromWave(wave);
    free(samples);  // Raylib đã copy dữ liệu, có thể giải phóng
    return sound;
}

// Tạo sóng sine (mượt hơn) - thích hợp cho tiếng "ding", "collect"
static Sound GenerateSineWaveSound(int frequency, float duration, float volume) {
    int sampleRate = 44100;
    int sampleCount = (int)(sampleRate * duration);
    int channels = 1;
    int sampleSize = 16;

    short *samples = (short*)malloc(sampleCount * sizeof(short));
    if (!samples) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for sine wave sound");
        return LoadSound("");
    }

    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float value = sinf(2.0f * 3.14159265f * frequency * t);
        samples[i] = (short)(value * 32767 * volume);
    }

    Wave wave = {0};
    wave.data = samples;
    wave.frameCount = sampleCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = sampleSize;
    wave.channels = channels;

    Sound sound = LoadSoundFromWave(wave);
    free(samples);
    return sound;
}

// ============================================================
//  Constructor
// ============================================================
GameManager::GameManager()
: player({0,0}), boss({1000, 200}, 200.0f),
  bullets(BattleUI().box), energy(BattleUI().box)
{
    currentScreen  = MENU;
    invincibleTimer = 0;
    gameOver       = false;
    playerWin      = false;
    bossMaxHp      = 300;
    bossHp         = bossMaxHp;
    score          = 0;
    bossHitFlash   = 0;
    menuAnimTime   = 0.0f;
    
    // ========== KHOI TAO TOC DO TP4 ==========
    currentSpeedLevel = SPEED_NORMAL;
    speedValues[SPEED_STOP]   = 0.0f;    // Dung yen
    speedValues[SPEED_SLOW]   = 2.0f;    // Cham
    speedValues[SPEED_NORMAL] = 4.5f;    // Nhanh (mac dinh)
    speedValues[SPEED_FAST]   = 7.5f;    // Nhanh hon
    
    player.speed = speedValues[SPEED_NORMAL];
    
    // Khoi tao vi tri player trong battle box
    player.position = {
        ui.box.x + ui.box.width  / 2,
        ui.box.y + ui.box.height / 2
    };
    
    // ========== LOAD AM THANH TP4 ==========
    soundLoaded = false;
    LoadGameSounds();
}

// ============================================================
//  LoadGameSounds - Load tat ca am thanh (nhạc nền từ file, SFX từ code)
// ============================================================
void GameManager::LoadGameSounds() {
    // Khoi tao audio device neu chua co
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
    }
    
    // ===== 1. Tạo các hiệu ứng âm thanh ngắn bằng sóng âm (không cần file) =====
    
    // Tiếng "hit" khi người chơi bị trúng đạn: sóng vuông tần số 300Hz, ngắn, hơi trầm
    hitSound = GenerateSquareWaveSound(300, 0.12f, 0.6f);
    
    // Tiếng nhặt Orb: sóng sine cao, trong trẻo, "ding"
    collectOrbSound = GenerateSineWaveSound(880, 0.18f, 0.7f);
    
    // Tiếng đánh trúng Boss: sóng vuông trầm, "bụp" mạnh
    bossHitSound = GenerateSquareWaveSound(160, 0.20f, 0.8f);
    
    // Tiếng Dash: sóng sine quét nhanh (có thể kết hợp 2 tần số) -> dùng sine 600Hz ngắn
    dashSound = GenerateSineWaveSound(600, 0.10f, 0.5f);
    
    // ===== 2. Load nhạc nền từ file (bạn phải có file trong thư mục sound/) =====
    // Giả sử bạn đã đặt battle.mp3 và menu.mp3 vào thư mục "sound"
    battleMusic = LoadMusicStream("F:/BaiTap/HK225/THDoHoaMayTinh/GameTP3/sound/battle.mp3");
    menuMusic   = LoadMusicStream("F:/BaiTap/HK225/THDoHoaMayTinh/GameTP3/sound/menu.mp3");
    
    // Nếu không tìm thấy file, music sẽ là invalid, ta vẫn tiếp tục (không bị crash)
    if (!IsMusicValid(battleMusic)) {
        TraceLog(LOG_WARNING, "Could not load sound/battle.mp3 - Music disabled");
    }
    if (!IsMusicValid(menuMusic)) {
        TraceLog(LOG_WARNING, "Could not load sound/menu.mp3 - Music disabled");
    }
    
    SetMusicVolume(battleMusic, 0.5f);
    SetMusicVolume(menuMusic, 0.4f);
    
    soundLoaded = true;
}

// ============================================================
//  UnloadGameSounds - Giai phong am thanh
// ============================================================
void GameManager::UnloadGameSounds() {
    if (!soundLoaded) return;
    
    UnloadSound(hitSound);
    UnloadSound(collectOrbSound);
    UnloadSound(bossHitSound);
    UnloadSound(dashSound);
    UnloadMusicStream(battleMusic);
    UnloadMusicStream(menuMusic);
    
    CloseAudioDevice();
    soundLoaded = false;
}

// ============================================================
//  ResetGame
// ============================================================
void GameManager::ResetGame() {
    bossHp         = bossMaxHp;
    score          = 0;
    gameOver       = false;
    playerWin      = false;
    invincibleTimer = 0;
    bossHitFlash   = 0;
    ui.hp          = ui.maxHp;

    player.position = {
        ui.box.x + ui.box.width  / 2,
        ui.box.y + ui.box.height / 2
    };
    player.velocity = {0, 0};
    
    // Reset toc do ve Normal
    currentSpeedLevel = SPEED_NORMAL;
    player.speed = speedValues[SPEED_NORMAL];

    bullets.Clear();
    bullets.patternTimer   = 0;
    bullets.shootTimer     = 0;
    bullets.spiralAngle    = 0;
    bullets.currentPattern = PATTERN_RADIAL;

    energy.Reset();
}

// ============================================================
//  UpdateSpeedLevel - Cap nhat muc do toc do tu phim bam
// ============================================================
void GameManager::UpdateSpeedLevel() {
    if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0)) {
        currentSpeedLevel = SPEED_STOP;
    }
    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
        currentSpeedLevel = SPEED_SLOW;
    }
    if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
        currentSpeedLevel = SPEED_NORMAL;
    }
    if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3)) {
        currentSpeedLevel = SPEED_FAST;
    }
}

// ============================================================
//  ApplySpeedLevel - Ap dung toc do vao player
// ============================================================
void GameManager::ApplySpeedLevel() {
    player.speed = speedValues[currentSpeedLevel];
}

// ============================================================
//  DrawSpeedIndicator - Hien thi muc do toc do hien tai
// ============================================================
void GameManager::DrawSpeedIndicator() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    int boxX = screenW - 180;
    int boxY = 20;
    int boxW = 160;
    int boxH = 70;
    
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.6f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, YELLOW);
    
    DrawText("SPEED", boxX + 10, boxY + 8, 18, YELLOW);
    
    const char* speedText = "";
    Color speedColor = WHITE;
    
    switch (currentSpeedLevel) {
        case SPEED_STOP:
            speedText = "STOP [0]";
            speedColor = RED;
            break;
        case SPEED_SLOW:
            speedText = "SLOW [1]";
            speedColor = ORANGE;
            break;
        case SPEED_NORMAL:
            speedText = "NORMAL [2]";
            speedColor = GREEN;
            break;
        case SPEED_FAST:
            speedText = "FAST [3]";
            speedColor = SKYBLUE;
            break;
    }
    
    DrawText(speedText, boxX + 10, boxY + 35, 22, speedColor);
    
    DrawText("0:Stop 1:Slow", screenW - 280, screenH - 35, 15, GRAY);
    DrawText("2:Normal 3:Fast", screenW - 280, screenH - 18, 15, GRAY);
}

// ============================================================
//  MENU
// ============================================================
void GameManager::UpdateMenu() {
    menuAnimTime += 0.016f;
    
    if (soundLoaded && IsMusicValid(menuMusic) && !IsMusicStreamPlaying(menuMusic)) {
        PlayMusicStream(menuMusic);
    }
    if (soundLoaded && IsMusicValid(menuMusic)) {
        UpdateMusicStream(menuMusic);
    }
    
    if (IsKeyPressed(KEY_ONE)) { ResetGame(); currentScreen = GAMEPLAY; }
    if (IsKeyPressed(KEY_TWO)) currentScreen = GUIDE;
    if (IsKeyPressed(KEY_THREE)) currentScreen = RANK;
    if (IsKeyPressed(KEY_ESCAPE)) CloseWindow();
}

void GameManager::DrawMenu() {
    ClearBackground(BLACK);
    DrawFractalMenuBG(menuAnimTime);

    DrawText("UNDERTALE STYLE GAME", 700, 200, 40, YELLOW);
    DrawText("1. PLAY",   850, 350, 30, WHITE);
    DrawText("2. GUIDE",  850, 420, 30, WHITE);
    DrawText("3. RANK",   850, 490, 30, WHITE);
    DrawText("ESC. EXIT", 850, 560, 30, RED);
    
    DrawText("Press 0-3 to change speed IN GAME", 680, 700, 20, Fade(WHITE, 0.5f));
}

// ============================================================
//  GUIDE
// ============================================================
void GameManager::UpdateGuide() {
    player.Update();
    if (IsKeyPressed(KEY_BACKSPACE)) currentScreen = MENU;
}

void GameManager::DrawGuide() {
    ClearBackground(BLACK);
    DrawText("GUIDE",                    900, 100, 40, YELLOW);
    DrawText("W A S D : DI CHUYEN",      750, 200, 28, WHITE);
    DrawText("SPACE   : DASH",           750, 245, 28, WHITE);
    DrawText("0 - 3   : CHANGE SPEED",   750, 290, 28, YELLOW);
    DrawText("  0: STOP   1: SLOW",      780, 330, 22, GRAY);
    DrawText("  2: NORMAL 3: FAST",      780, 358, 22, GRAY);
    DrawText("Ne dan cua Boss!",         750, 400, 26, ORANGE);
    DrawText("Nhat 3 orb vang -> Ban Boss!", 750, 440, 26, YELLOW);
    DrawText("BACKSPACE: BACK",          750, 500, 26, GRAY);
    player.Draw();
}

// ============================================================
//  RANK
// ============================================================
void GameManager::UpdateRank() {
    if (IsKeyPressed(KEY_BACKSPACE)) currentScreen = MENU;
}

void GameManager::DrawRank() {
    ClearBackground(BLACK);
    DrawText("RANKING",           850, 100, 40, YELLOW);
    DrawText("1 MIN: 5000 - HUY", 700, 250, 30, WHITE);
    DrawText("2 MIN: 3200 - TRI", 700, 300, 30, WHITE);
    DrawText("3 MIN: 1500 - PRO", 700, 350, 30, WHITE);
    DrawText("BACKSPACE: BACK",   700, 450, 25, GRAY);
}

// ============================================================
//  GAMEPLAY  -  Update
// ============================================================
void GameManager::UpdateGame() {
    if (gameOver || playerWin) {
        if (IsKeyPressed(KEY_R))         ResetGame();
        if (IsKeyPressed(KEY_BACKSPACE)) currentScreen = MENU;
        return;
    }

    UpdateSpeedLevel();
    ApplySpeedLevel();

    player.Update();
    boss.Update();
    ui.Update();

    // Phat nhac nen battle
    if (soundLoaded && IsMusicValid(battleMusic)) {
        if (!IsMusicStreamPlaying(battleMusic)) {
            PlayMusicStream(battleMusic);
        }
        UpdateMusicStream(battleMusic);
    }

    if (invincibleTimer > 0) invincibleTimer--;
    if (bossHitFlash   > 0) bossHitFlash--;

    float hs = player.size * 0.5f;
    if (player.position.x < ui.box.x + hs)
        player.position.x = ui.box.x + hs;
    if (player.position.x > ui.box.x + ui.box.width - hs)
        player.position.x = ui.box.x + ui.box.width - hs;
    if (player.position.y < ui.box.y + hs)
        player.position.y = ui.box.y + hs;
    if (player.position.y > ui.box.y + ui.box.height - hs)
        player.position.y = ui.box.y + ui.box.height - hs;

    bullets.Update(boss.position, player.position);

    if (invincibleTimer <= 0) {
        float hitR = player.size * 0.30f;
        if (bullets.CheckPlayerHit(player.position, hitR)) {
            ui.hp -= 10;
            invincibleTimer = 45;
            score = (score - 50 < 0) ? 0 : score - 50;
            
            if (soundLoaded) PlaySound(hitSound);
            
            if (ui.hp <= 0) { ui.hp = 0; gameOver = true; }
        }
    }

    int prevOrbsCollected = energy.orbsCollected;
    int dmg = energy.Update(player.position, player.size * 0.5f, boss.position);
    
    if (energy.orbsCollected > prevOrbsCollected && soundLoaded) {
        PlaySound(collectOrbSound);
    }
    
    if (dmg > 0) {
        bossHp -= dmg;
        score  += 200;
        bossHitFlash = 20;
        
        if (soundLoaded) PlaySound(bossHitSound);
        
        if (bossHp < 0) bossHp = 0;
    }

    if (bossHp <= 0) {
        playerWin = true;
        bullets.Clear();
        energy.Reset();
        if (soundLoaded && IsMusicValid(battleMusic)) StopMusicStream(battleMusic);
    }

    score++;

    if (IsKeyPressed(KEY_BACKSPACE)) {
        currentScreen = MENU;
        if (soundLoaded && IsMusicValid(battleMusic)) StopMusicStream(battleMusic);
    }
    
    // Phát tiếng dash nếu player vừa dash
    if (player.justDashed && soundLoaded) {
        PlaySound(dashSound);
    }
}

// ============================================================
//  DrawChargeIndicator
// ============================================================
void GameManager::DrawChargeIndicator() {
    int baseX = (int)(ui.box.x + ui.box.width) - 5;
    int baseY = (int)(ui.box.y + ui.box.height) + 45;
    int orbR  = 12;
    int gap   = 32;

    DrawText("CHARGE", baseX - 88, baseY - 2, 18, YELLOW);

    for (int i = 0; i < 3; i++) {
        int cx = baseX + 20 + i * gap;
        int cy = baseY + orbR / 2;

        if (i < energy.orbsCollected) {
            DrawCircleFilledMidpoint(cx, cy, orbR + 4, Fade(YELLOW, 0.20f));
            DrawCircleFilledMidpoint(cx, cy, orbR,     GOLD);
            DrawCircleFilledMidpoint(cx, cy, orbR - 4, YELLOW);
            DrawCircleFilledMidpoint(cx - orbR/4, cy - orbR/4,
                                     orbR/3, Fade(WHITE, 0.75f));
        } else {
            DrawCircleMidpoint(cx, cy, orbR,     Fade(YELLOW, 0.35f));
            DrawCircleMidpoint(cx, cy, orbR - 1, Fade(DARKGRAY, 0.50f));
        }
    }

    if (energy.chargeGlow > 0.01f) {
        DrawText("+ ORB!",
            baseX - 10, baseY - 22, 20,
            Fade(YELLOW, energy.chargeGlow));
    }

    if (energy.orbsCollected == 2) {
        DrawText("1 more!",
            baseX - 15, baseY + orbR * 2 + 8, 18,
            Fade(ORANGE, 0.85f));
    }
}

// ============================================================
//  DrawHUD
// ============================================================
void GameManager::DrawHUD() {
    int barW = 500, barH = 18;
    int barX = (int)(ui.box.x + (ui.box.width - barW) / 2);
    int barY = (int)(ui.box.y - 50);

    DrawRectangle(barX, barY, barW, barH, DARKGRAY);
    int fill = (int)(barW * ((float)bossHp / bossMaxHp));
    if (fill > 0) {
        Color barColor = (bossHitFlash > 0 && (bossHitFlash / 3) % 2 == 0)
                         ? WHITE : ORANGE;
        DrawRectangle(barX, barY, fill, barH, barColor);
    }
    DrawRectangleLines(barX, barY, barW, barH, WHITE);
    DrawText("BOSS", barX - 70, barY, barH, ORANGE);
    DrawText(TextFormat("HP %d", bossHp), barX + barW + 10, barY, 16, ORANGE);

    DrawText(TextFormat("SCORE: %d", score),
        (int)ui.box.x, (int)(ui.box.y - 82), 24, YELLOW);

    if (invincibleTimer > 0 && (invincibleTimer / 5) % 2 == 0)
        DrawText("! HIT !",
            (int)(ui.box.x + ui.box.width/2 - 40),
            (int)(ui.box.y + ui.box.height/2 - 15),
            30, RED);

    DrawChargeIndicator();
    DrawSpeedIndicator();
}

// ============================================================
//  DrawGame
// ============================================================
void GameManager::DrawGame() {
    ClearBackground(BLACK);

    DrawFractalBackground(ui.box, boss.bobTime);

    boss.Draw();
    ui.Draw();
    DrawFractalHitEffect(boss.position, boss.size, bossHitFlash);

    energy.Draw();
    bullets.Draw();
    player.Draw();
    DrawHUD();

    if (gameOver) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.65f));
        DrawText("GAME OVER",
            GetScreenWidth()/2 - 120, GetScreenHeight()/2 - 60, 60, RED);
        DrawText("R: Retry   BACKSPACE: Menu",
            GetScreenWidth()/2 - 180, GetScreenHeight()/2 + 30, 28, WHITE);
    }

    if (playerWin) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));
        DrawText("YOU WIN!",
            GetScreenWidth()/2 - 120, GetScreenHeight()/2 - 60, 60, YELLOW);
        DrawText(TextFormat("SCORE: %d", score),
            GetScreenWidth()/2 - 80, GetScreenHeight()/2 + 10, 36, WHITE);
        DrawText("R: Play Again   BACKSPACE: Menu",
            GetScreenWidth()/2 - 200, GetScreenHeight()/2 + 60, 26, GRAY);
    }
}

// ============================================================
//  Dispatch
// ============================================================
void GameManager::Update() {
    switch (currentScreen) {
        case MENU:     UpdateMenu();  break;
        case GUIDE:    UpdateGuide(); break;
        case RANK:     UpdateRank();  break;
        case GAMEPLAY: UpdateGame();  break;
    }
}

void GameManager::Draw() {
    switch (currentScreen) {
        case MENU:     DrawMenu();  break;
        case GUIDE:    DrawGuide(); break;
        case RANK:     DrawRank();  break;
        case GAMEPLAY: DrawGame();  break;
    }
}