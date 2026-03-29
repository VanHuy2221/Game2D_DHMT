#pragma once
#include "raylib.h"
#include <string>

class BattleUI {
public:
    Rectangle box;

    std::string playerName;
    int level;
    int hp;
    int maxHp;

    BattleUI();

    void Update();
    void Draw();
};