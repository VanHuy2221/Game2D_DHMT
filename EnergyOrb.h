#pragma once
#include "raylib.h"
#include <vector>
#include <cmath>

// ============================================================
//  EnergyOrb  -  Khoi nang luong vang player co the nhat
//
//  Quy tac:
//    - Toi da 2 khoi ton tai cung luc trong battle box
//    - Moi khoi co thoi gian song (life), sau do bien mat
//    - Player di vao la tu dong nhat (khong can nhan phim)
//    - Nhat du 3 khoi => kich hoat PlayerAttack
// ============================================================

struct EnergyOrb {
    Vector2 position;
    float   radius;
    float   life;        // Frame con song
    float   maxLife;
    float   bobTime;     // Dung cho hieu ung lo-len
    bool    active;

    EnergyOrb(Vector2 pos, float r, float lifeFrames)
        : position(pos), radius(r),
          life(lifeFrames), maxLife(lifeFrames),
          bobTime(0.0f), active(true) {}
};

// ============================================================
//  PlayerAttack  -  Don tan cong bai ve phia Boss
//
//  Sau khi nhat du 3 orb, 1 vien dan sang chay tu player -> Boss
//  Dam Boss, tru HP Boss
// ============================================================
struct PlayerAttack {
    Vector2 position;
    Vector2 velocity;
    float   radius;
    float   life;
    float   maxLife;
    float   glowTime;   // Dung cho hieu ung nhip sang
    bool    active;

    PlayerAttack(Vector2 pos, Vector2 vel, float r, float lifeFrames)
        : position(pos), velocity(vel), radius(r),
          life(lifeFrames), maxLife(lifeFrames),
          glowTime(0.0f), active(true) {}
};

// ============================================================
//  EnergySystem  -  Quan ly orb + player attack cung 1 cho
// ============================================================
class EnergySystem {
public:
    std::vector<EnergyOrb>    orbs;
    std::vector<PlayerAttack> attacks;

    int  orbsCollected;     // So orb da nhat (0-2), du 3 thi ban
    int  spawnTimer;        // Dem frame de spawn orb moi
    int  spawnInterval;     // Khoang cach spawn (frame)

    Rectangle battleBox;    // Goi han spawn & hien thi

    // Hieu ung "charge" khi da nhat >= 1 orb
    float chargeGlow;

    EnergySystem(Rectangle box);

    // Goi moi frame: spawn orb, kiem tra nhat, cap nhat attack
    // Tra ve so damage can tru vao Boss HP (0 neu chua trung)
    int Update(Vector2 playerPos, float playerRadius, Vector2 bossPos);

    void Draw();
    void Reset();

private:
    void  SpawnOrb();
    bool  CheckPickup(Vector2 playerPos, float playerRadius);
    void  FireAttack(Vector2 from, Vector2 bossPos);
};