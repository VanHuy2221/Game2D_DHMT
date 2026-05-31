#include "GameManager.h"
#include "DrawUtils.h"
#include "Fractal.h"

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

    player.position = {
        ui.box.x + ui.box.width  / 2,
        ui.box.y + ui.box.height / 2
    };
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

    bullets.Clear();
    bullets.patternTimer   = 0;
    bullets.shootTimer     = 0;
    bullets.spiralAngle    = 0;
    bullets.currentPattern = PATTERN_RADIAL;

    energy.Reset();
}

// ============================================================
//  MENU
// ============================================================
void GameManager::UpdateMenu() {
    menuAnimTime += 0.016f;  // ~60fps -> tang ~1 don vi/giay
    if (IsKeyPressed(KEY_ONE)) { ResetGame(); currentScreen = GAMEPLAY; }
    if (IsKeyPressed(KEY_TWO)) currentScreen = GUIDE;
    if (IsKeyPressed(KEY_THREE)) currentScreen = RANK;
    if (IsKeyPressed(KEY_ESCAPE)) CloseWindow();
}

void GameManager::DrawMenu() {
    ClearBackground(BLACK);

    // Fractal nen man hinh Menu (Koch Snowflake + C-Curve xoay dong)
    DrawFractalMenuBG(menuAnimTime);

    DrawText("UNDERTALE STYLE GAME", 700, 200, 40, YELLOW);
    DrawText("1. PLAY",   850, 350, 30, WHITE);
    DrawText("2. GUIDE",  850, 420, 30, WHITE);
    DrawText("3. RANK",   850, 490, 30, WHITE);
    DrawText("ESC. EXIT", 850, 560, 30, RED);
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
    DrawText("Ne dan cua Boss!",         750, 300, 26, ORANGE);
    DrawText("Nhat 3 orb vang -> Ban Boss!", 750, 340, 26, YELLOW);
    DrawText("BACKSPACE: BACK",          750, 400, 26, GRAY);
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

    player.Update();
    boss.Update();
    ui.Update();

    // Giam invincibility
    if (invincibleTimer > 0) invincibleTimer--;
    if (bossHitFlash   > 0) bossHitFlash--;

    // Gioi han player trong battle box
    float hs = player.size * 0.5f;
    if (player.position.x < ui.box.x + hs)
        player.position.x = ui.box.x + hs;
    if (player.position.x > ui.box.x + ui.box.width - hs)
        player.position.x = ui.box.x + ui.box.width - hs;
    if (player.position.y < ui.box.y + hs)
        player.position.y = ui.box.y + hs;
    if (player.position.y > ui.box.y + ui.box.height - hs)
        player.position.y = ui.box.y + ui.box.height - hs;

    // Cap nhat dan Boss
    bullets.Update(boss.position, player.position);

    // Kiem tra dan trung player
    if (invincibleTimer <= 0) {
        float hitR = player.size * 0.30f;
        if (bullets.CheckPlayerHit(player.position, hitR)) {
            ui.hp -= 10;
            invincibleTimer = 45;
            score = (score - 50 < 0) ? 0 : score - 50;
            if (ui.hp <= 0) { ui.hp = 0; gameOver = true; }
        }
    }

    // ---- Cap nhat EnergySystem ----
    //  Update tra ve damage can tru vao Boss HP
    int dmg = energy.Update(player.position, player.size * 0.5f, boss.position);
    if (dmg > 0) {
        bossHp -= dmg;
        score  += 200;          // Thuong diem khi danh trung Boss
        bossHitFlash = 20;      // Flash Boss 20 frame
        if (bossHp < 0) bossHp = 0;
    }

    // Win condition
    if (bossHp <= 0) {
        playerWin = true;
        bullets.Clear();
        energy.Reset();
    }

    score++;

    if (IsKeyPressed(KEY_BACKSPACE)) currentScreen = MENU;
}

// ============================================================
//  DrawChargeIndicator  -  3 o tron hien so orb da nhat
//  Dat phia duoi BattleUI, ben phai HP bar
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
            // Orb da nhat: sang, co glow
            DrawCircleFilledMidpoint(cx, cy, orbR + 4, Fade(YELLOW, 0.20f));
            DrawCircleFilledMidpoint(cx, cy, orbR,     GOLD);
            DrawCircleFilledMidpoint(cx, cy, orbR - 4, YELLOW);
            DrawCircleFilledMidpoint(cx - orbR/4, cy - orbR/4,
                                     orbR/3, Fade(WHITE, 0.75f));
        } else {
            // O trong: khung mo
            DrawCircleMidpoint(cx, cy, orbR,     Fade(YELLOW, 0.35f));
            DrawCircleMidpoint(cx, cy, orbR - 1, Fade(DARKGRAY, 0.50f));
        }
    }

    // Neu co charge glow (vua nhat orb): flash toan bo indicator
    if (energy.chargeGlow > 0.01f) {
        DrawText("+ ORB!",
            baseX - 10, baseY - 22, 20,
            Fade(YELLOW, energy.chargeGlow));
    }

    // Neu du 3 sap ban: hien chu "READY!"
    // (Hien thi ngay sau khi FireAttack nen dung gia tri truoc khi update)
    // Thay vao do hien khi 3 o day la du
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
    // Boss HP bar
    int barW = 500, barH = 18;
    int barX = (int)(ui.box.x + (ui.box.width - barW) / 2);
    int barY = (int)(ui.box.y - 50);

    DrawRectangle(barX, barY, barW, barH, DARKGRAY);
    int fill = (int)(barW * ((float)bossHp / bossMaxHp));
    if (fill > 0) {
        // Flash trang khi Boss bi trung
        Color barColor = (bossHitFlash > 0 && (bossHitFlash / 3) % 2 == 0)
                         ? WHITE : ORANGE;
        DrawRectangle(barX, barY, fill, barH, barColor);
    }
    DrawRectangleLines(barX, barY, barW, barH, WHITE);
    DrawText("BOSS", barX - 70, barY, barH, ORANGE);
    DrawText(TextFormat("HP %d", bossHp), barX + barW + 10, barY, 16, ORANGE);

    // Score
    DrawText(TextFormat("SCORE: %d", score),
        (int)ui.box.x, (int)(ui.box.y - 82), 24, YELLOW);

    // Invincible hit flash
    if (invincibleTimer > 0 && (invincibleTimer / 5) % 2 == 0)
        DrawText("! HIT !",
            (int)(ui.box.x + ui.box.width/2 - 40),
            (int)(ui.box.y + ui.box.height/2 - 15),
            30, RED);

    // Charge indicator (3 o orb)
    DrawChargeIndicator();
}

// ============================================================
//  DrawGame
// ============================================================
void GameManager::DrawGame() {
    ClearBackground(BLACK);

    // ---- Fractal nen Battle Box (Koch Snowflake + Koch Curve vien) ----
    // Ve truoc tat ca doi tuong khac de lam "nen"
    // boss.bobTime truyen vao de animation dong bo voi Boss
    DrawFractalBackground(ui.box, boss.bobTime);

    boss.Draw();
    ui.Draw();

    // ---- Fractal hieu ung khi Boss bi trung don ----
    // Ve sau Boss nhung truoc orb/player de tao do sau
    DrawFractalHitEffect(boss.position, boss.size, bossHitFlash);

    energy.Draw();      // Ve orb va player attack
    bullets.Draw();     // Ve dan Boss
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