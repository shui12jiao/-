#include "raylib.h"
#include "raymath.h"

#include <iostream>

#define WIDHT 1200.0
#define HEIGHT 900.0
#define ENEMY_NUM 5

class Player {
public:
    Vector2 position = Vector2{ WIDHT / 2, HEIGHT / 2 };
    bool survive = true;
};

class Enemy {
public:
    Vector2 position;
    bool survive = true;
};

class Bullet {
public:
    Vector2 position;
};


static void InitGame(void) {
    InitWindow(WIDHT, HEIGHT, "飞机游戏");
}
static void UpdateGame(void) {

}
static void DrawGame(void) {

}
static void UnloadGame(void) {

}



int main() {
    InitGame();
    SetTargetFPS(60);
   
    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    UnloadGame();         

    CloseWindow();        

    return 0;
}

