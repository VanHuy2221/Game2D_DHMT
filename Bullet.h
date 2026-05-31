#pragma once
#include "raylib.h"
#include <vector>
#include <cmath>

// ============================================================
//  Cac loai dan cua Boss (phong cach Undertale)
// ============================================================
enum BulletType {
    BULLET_STRAIGHT,   // Dan bay thang theo 1 huong
    BULLET_AIMED,      // Dan bay thang ve phia player
    BULLET_SPIRAL,     // Dan xoay tron (spiral)
    BULLET_WAVE        // Dan bay theo song sin
};

// ============================================================
//  Cau truc mot vien dan
// ============================================================
struct Bullet {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float life;        // So frame con song (0 = destroy)
    float maxLife;
    float angle;       // Goc hien tai (dung cho spiral/wave)
    float waveOffset;  // Offset pha song sin (wave bullet)
    BulletType type;
    Color color;
    bool active;

    Bullet(Vector2 pos, Vector2 vel, float r, float lifeFrames,
           Color c, BulletType t = BULLET_STRAIGHT, float waveOff = 0.0f)
        : position(pos), velocity(vel), radius(r),
          life(lifeFrames), maxLife(lifeFrames),
          angle(0.0f), waveOffset(waveOff),
          type(t), color(c), active(true) {}
};

// ============================================================
//  Cac kieu pattern tan cong cua Boss
// ============================================================
enum AttackPattern {
    PATTERN_RADIAL,    // Bắn đều mọi hướng từ Boss
    PATTERN_AIMED,     // Bắn liên tục về phía player
    PATTERN_SPIRAL,    // Xoay tron bắn ra
    PATTERN_CROSS,     // Bắn theo hình thập
    PATTERN_WAVE,      // Sóng đạn từ cạnh box
    PATTERN_RAIN,      // Mưa đạn từ trên xuống
    PATTERN_COUNT
};

// ============================================================
//  BulletManager - Quan ly toan bo he thong dan
// ============================================================
class BulletManager {
public:
    std::vector<Bullet> bullets;

    // Timer dieu khien attack
    int   patternTimer;     // Frame dem den lua chon pattern moi
    int   patternDuration;  // So frame moi pattern keo dai
    int   shootTimer;       // Timer ban trong 1 pattern
    int   shootInterval;    // Frame giua 2 lan ban
    AttackPattern currentPattern;

    float spiralAngle;      // Goc xoay tich luy cho pattern spiral

    // Tham so cua battle box (can de tao mua dan)
    Rectangle battleBox;

    BulletManager(Rectangle box);

    // Cap nhat toan bo he thong
    void Update(Vector2 bossPos, Vector2 playerPos);

    // Kiem tra va xu ly va cham voi player
    // Tra ve true neu player bi trung dan
    bool CheckPlayerHit(Vector2 playerPos, float playerHitRadius);

    // Ve tat ca vien dan
    void Draw();

    // Xoa het dan (khi doi pattern hoac restart)
    void Clear();

private:
    // ========================================================
    //  Cac ham sinh dan (spawn) theo tung pattern
    // ========================================================

    // PATTERN_RADIAL: bắn numBullets vien toa ra deu theo moi huong
    void SpawnRadial(Vector2 origin, int numBullets, float speed, Color c);

    // PATTERN_AIMED: ban 1 vien thang ve phia player
    void SpawnAimed(Vector2 origin, Vector2 target, float speed, Color c);

    // PATTERN_SPIRAL: ban 1 cung xoay tich luy theo spiralAngle
    void SpawnSpiral(Vector2 origin, int arms, float speed, Color c);

    // PATTERN_CROSS: ban 4 vien theo huong +, 4 vien theo huong x
    void SpawnCross(Vector2 origin, float speed, Color c);

    // PATTERN_WAVE: ban song dan tu 2 ben canh box
    void SpawnWave(float speed, Color c);

    // PATTERN_RAIN: mua dan tu tren xuong trong battle box
    void SpawnRain(int count, float speed, Color c);

    // Chon pattern ngau nhien tiep theo
    void PickNextPattern();
};