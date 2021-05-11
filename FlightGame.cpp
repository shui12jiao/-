#include "raylib.h"
#include "raymath.h"

#include <iostream>

#define WIDTH 1600.0f
#define HEIGHT 900.0f
#define MAX_FPS 480.0f

#define MAX_ENEMY 15

#define MAX_BULLET 20

#define PLAYER_HEALTH 20
#define PLAYER_SIZE 20.0f
#define PLAYER_SPEED 4.0f
#define PLAYER_ROTATION_SPEED 4.0f
#define PLAYER_SHOOT_RATE 4

class Bullet {
public:
    Vector2 position;
    Color color;
    int attack;
    float radius;
    float speed;
    float rotation;
    bool active;
};

class Player {
public:
    void initialize() {
        position = Vector2{ WIDTH / 2, HEIGHT - height };
        rotation = 0.0f;
        score = 0;
        defeat = 0;
        health = PLAYER_HEALTH;
        for (int i = 0; i < MAX_BULLET; ++i) {
            bullet[i].attack = 1;
            bullet[i].color = RED;
            bullet[i].radius = PLAYER_SIZE / 7;
        }

    }

    void move() {
        if (IsKeyDown(KEY_A)) { rotation -= rSpeed; }
        if (IsKeyDown(KEY_D)) { rotation += rSpeed; }
        if (IsKeyDown(KEY_W)) {
            position.x += speed * sin(rotation * DEG2RAD);
            position.y -= speed * cos(rotation * DEG2RAD);
        }
        if (IsKeyDown(KEY_S)) {
            position.x -= speed * sin(rotation * DEG2RAD);
            position.y += speed * cos(rotation * DEG2RAD);
        }

        if (position.x > WIDTH + height) {
            position.x = -height;
        }
        else if (position.x < -height) {
            position.x = WIDTH + height;
        }
        if (position.y > (HEIGHT + height)) {
            position.y = -height;
        }
        else if (position.y < -height) {
            position.y = HEIGHT + height;
        }
    }

    void shoot(Sound& shoot) {
        ++lag;

        if (IsKeyDown(KEY_SPACE) && lag > (MAX_FPS / PLAYER_SHOOT_RATE)) {
            PlaySound(shoot);
            for (int i = 0; i < MAX_BULLET; ++i) {
                if (!bullet[i].active) {
                    bullet[i].active = true;
                    bullet[i].position = Vector2{ position.x + sin(rotation * DEG2RAD) * (height / 2.5f),position.y - cos(rotation * DEG2RAD) * (height / 2.5f) };
                    bullet[i].speed = speed * 3.0f;
                    bullet[i].rotation = rotation;
                    break;
                }
            }
            lag = 0;
        }

        for (int i = 0; i < MAX_BULLET; ++i) {
            if (bullet[i].active) {
                bullet[i].position.x += bullet[i].speed * sin(bullet[i].rotation * DEG2RAD);
                bullet[i].position.y -= bullet[i].speed * cos(bullet[i].rotation * DEG2RAD);

                if (bullet[i].position.x > WIDTH) {
                    bullet[i].active = false;
                }
                else if (bullet[i].position.x < 0) {
                    bullet[i].active = false;
                }
                if (bullet[i].position.y > HEIGHT) {
                    bullet[i].active = false;
                }
                else if (bullet[i].position.y < 0) {
                    bullet[i].active = false;
                }
            }
        }

    }

    Vector2 position;
    float rotation;
    int health;
    int score;
    int defeat;
    int lag = 0;
    Color color = MAROON;
    Bullet bullet[MAX_BULLET];

    float speed = PLAYER_SPEED * 240.0f / MAX_FPS;
    float rSpeed = PLAYER_ROTATION_SPEED * 240.0f / MAX_FPS;
    float height = (PLAYER_SIZE / 2) / tanf(20 * DEG2RAD);
};

class Enemy {
public:
    void initialize() {
        position.x = (float)GetRandomValue(0, WIDTH);
        position.y = (float)GetRandomValue(0, HEIGHT / 2);
        radius = (float)GetRandomValue(PLAYER_SIZE, PLAYER_SIZE * 4);
        speed = PLAYER_SPEED * PLAYER_SIZE / radius * 60 / MAX_FPS;
        health = radius / PLAYER_SIZE;
        survive = true;
        for (int i = 0; i < MAX_BULLET; ++i) {
            bullet[i].active = false;
            bullet[i].attack = health;
            bullet[i].color = DARKGRAY;
            bullet[i].radius = radius / 7;
        }
    }

    void move() {
        if (survive) {
            position.x += speed;
            position.y += speed;

            if (position.x > WIDTH + radius) {
                position.x = -radius;
            }
            else if (position.x < -radius) {
                position.x = WIDTH + radius;
            }
            if (position.y > (HEIGHT + radius)) {
                position.y = -radius;
            }
            else if (position.y < -radius) {
                position.y = HEIGHT + radius;
            }
        }
    }

    void shoot(Vector2& pPosition) {
        ++lag;
        float rotation;

        if (survive && lag > (MAX_FPS * radius / 50)) {
            rotation = 180 - atan((pPosition.x - position.x) / (pPosition.y - position.y)) * RAD2DEG;

            for (int i = 0; i < MAX_BULLET; ++i) {
                if (!bullet[i].active) {
                    bullet[i].active = true;
                    bullet[i].position = Vector2{ position.x + sin(rotation * DEG2RAD) * radius, position.y - cos(rotation * DEG2RAD) * radius };
                    bullet[i].speed = speed * 2.0f;
                    bullet[i].rotation = rotation;
                    break;
                }
            }

            lag = 0;
        }

        for (int i = 0; i < MAX_BULLET; ++i) {
            if (bullet[i].active) {
                bullet[i].position.x += bullet[i].speed * sin(bullet[i].rotation * DEG2RAD);
                bullet[i].position.y -= bullet[i].speed * cos(bullet[i].rotation * DEG2RAD);

                if (bullet[i].position.x > WIDTH) {
                    bullet[i].active = false;
                }
                else if (bullet[i].position.x < 0) {
                    bullet[i].active = false;
                }
                if (bullet[i].position.y > HEIGHT) {
                    bullet[i].active = false;
                }
                else if (bullet[i].position.y < 0) {
                    bullet[i].active = false;
                }
            }
        }
    }

    Vector2 position;
    float radius;
    float speed;
    bool survive;
    int health;
    int lag = 0;
    Bullet bullet[MAX_BULLET];
};

static Player player;
static Enemy enemy[MAX_ENEMY];
static Bullet bullet[MAX_BULLET];
static int framesCounter;
static bool gameOver;
static bool pause;

static void Welcome();
static void InitGame(Music& background);
static void UpdateGame(Music& background, Sound& shoot, Sound& defeat);
static void DrawGame(Music& background, Sound& win, Sound& lose);
static void UnloadGame(Music& background, Sound& shoot, Sound& defeat, Sound& win, Sound& lose);

int main() {
    Welcome();

    Music background_music = LoadMusicStream("resources/background_music.ogg");
    Sound shoot_sound = LoadSound("resources/shoot_sound.ogg");
    Sound lose_sound = LoadSound("resources/lose_sound.ogg");
    Sound win_sound = LoadSound("resources/win_sound.ogg");
    Sound defeat_sound = LoadSound("resources/defeat_sound.ogg");

    InitGame(background_music);

    while (!WindowShouldClose()) {
        UpdateGame(background_music, shoot_sound, defeat_sound);
        DrawGame(background_music, win_sound, lose_sound);
        UpdateMusicStream(background_music);
    }

    UnloadGame(background_music, shoot_sound, defeat_sound, win_sound, lose_sound);

    return 0;
}


static void Welcome() {
    InitWindow(WIDTH, HEIGHT, "飞机游戏");
    InitAudioDevice();

    BeginDrawing();
    DrawText("<<< FLIGHT GAME >>>", WIDTH / 2 - MeasureText("<<< FLIGHT GAME >>>", 70) / 2, HEIGHT / 2 - 60, 70, SKYBLUE);
    DrawText("PRESS [ENTER] TO PLAY", WIDTH / 2 - MeasureText("PRESS [ENTER] TO PLAY", 20) / 2, HEIGHT / 2 + 20, 20, BLACK);
    EndDrawing();


}

static void InitGame(Music& background) {
    SetTargetFPS(MAX_FPS);

    framesCounter = 0;
    pause = false;
    gameOver = false;

    for (int i = 0; i < MAX_ENEMY; ++i) { enemy[i].initialize(); }
    player.initialize();

    PlayMusicStream(background);
}

static void UpdateGame(Music& background, Sound& shoot, Sound& defeat) {
    if (!gameOver) {
        if (IsKeyPressed('P')) {
            pause = !pause;
            if (pause) {
                PauseMusicStream(background);
            }
            else {
                 ResumeMusicStream(background);
            }
        }

        if (!pause) {

            ++framesCounter;

            //玩家--移动
            player.move();

            //玩家--射击
            player.shoot(shoot);

            //玩家&&敌人--碰撞系统
            Vector2 place = { player.position.x + sin(player.rotation * DEG2RAD) * (player.height / 2.5f),player.position.y - cos(player.rotation * DEG2RAD) * (player.height / 2.5f) };
            for (int i = 0; i < MAX_ENEMY; i++) {
                if (enemy[i].survive && CheckCollisionCircles({ place.x, place.y }, PLAYER_SIZE / 3, enemy[i].position, enemy[i].radius)) {
                    player.health -= enemy[i].health;
                    ++player.defeat;
                    enemy[i].survive = false;
                    PlaySound(defeat);
                }
            }

            //玩家&&子弹--碰撞系统
            for (int i = 0; i < MAX_ENEMY; ++i) {
                if (enemy[i].survive) {
                    for (int j = 0; j < MAX_BULLET; ++j) {
                        if (enemy[i].bullet[j].active && CheckCollisionCircles({ place.x, place.y }, PLAYER_SIZE / 3, enemy[i].bullet[j].position, enemy[i].bullet[j].radius)) {
                            player.health -= enemy[i].bullet[j].attack;
                            enemy[i].bullet[j].active = false;
                        }
                    }
                }
            }

            if (player.health <= 0) { gameOver = true; }

            //敌人&&子弹--碰撞系统
            for (int i = 0; i < MAX_BULLET; ++i) {
                if (player.bullet[i].active) {
                    place = { player.bullet[i].position.x,player.bullet[i].position.y };
                    for (int j = 0; j < MAX_ENEMY; ++j) {
                        if (enemy[j].survive && CheckCollisionCircles({ place.x, place.y }, player.bullet[i].radius, enemy[j].position, enemy[j].radius)) {
                            --enemy[j].health;
                            ++player.score;
                            if (enemy[j].health <= 0) {
                                enemy[j].survive = false;
                                ++player.defeat;
                                PlaySound(defeat);
                            }
                            player.bullet[i].active = false;
                            break;
                        }
                    }
                }
            }

            //敌人--移动--射击
            for (int i = 0; i < MAX_ENEMY; i++) {
                enemy[i].move();
                enemy[i].shoot(player.position);
            }

            if (player.defeat == MAX_ENEMY) { gameOver = true; }
        }
    }
    else {
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame(background);
            gameOver = false;
        }
    }
}

static void DrawGame(Music& background, Sound& win, Sound& lose) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    
        //玩家
        Vector2 v1 = { player.position.x + sinf(player.rotation * DEG2RAD) * (player.height),
                      player.position.y - cosf(player.rotation * DEG2RAD) * (player.height) };
        Vector2 v2 = { player.position.x - cosf(player.rotation * DEG2RAD) * (PLAYER_SIZE / 2),
                      player.position.y - sinf(player.rotation * DEG2RAD) * (PLAYER_SIZE / 2) };
        Vector2 v3 = { player.position.x + cosf(player.rotation * DEG2RAD) * (PLAYER_SIZE / 2),
                      player.position.y + sinf(player.rotation * DEG2RAD) * (PLAYER_SIZE / 2) };
        DrawTriangle(v1, v2, v3, player.color);

        //玩家--子弹
        for (int i = 0; i < MAX_BULLET; ++i) {
            if (player.bullet[i].active == true) { DrawCircleV(player.bullet[i].position, player.bullet[i].radius, player.bullet[i].color); }
        }

        //敌人
        for (int i = 0; i < MAX_ENEMY; i++) {
            if (enemy[i].survive) {
                DrawCircleV(enemy[i].position, enemy[i].radius, GRAY);

                //敌人--子弹
                for (int j = 0; j < MAX_BULLET; ++j) {
                    if (enemy[i].bullet[j].active == true) { DrawCircleV(enemy[i].bullet[j].position, enemy[i].bullet[j].radius, enemy[i].bullet[j].color); }
                }
            }
            else {
                DrawCircleV(enemy[i].position, enemy[i].radius, Fade(LIGHTGRAY, 0.3f));
            }
        }

        //信息
        DrawText(TextFormat("TIME: %.02f", (float)framesCounter / MAX_FPS), 10, 10, 10, BLACK);
        DrawText(TextFormat("FPS: %.02f", (float)GetFPS()), MeasureText("TIME: %000000000.02f", 10), 10, 10, BLACK);

        DrawText(TextFormat("HEALTH: %d", player.health), WIDTH - 120, 10, 15, BLACK);
        DrawText(TextFormat("SPEED: %.02f", player.speed), WIDTH - 120, 30, 15, BLACK);
        DrawText(TextFormat("SCORE: %d", player.score), WIDTH - 120, 50, 15, BLACK);

        //胜利||失败
        if (gameOver && player.defeat == MAX_ENEMY) {
            DrawText("--- YOU WIN ---", WIDTH / 2 - MeasureText("--- YOU WIN ---", 50) / 2, HEIGHT / 2 - 60, 50, RED);
            DrawText("PRESS [ENTER] TO PLAY AGAIN", WIDTH / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, HEIGHT / 2, 20, BLACK);

            if (IsMusicPlaying(background)) StopMusicStream(background);
            if (!pause) {
                PlaySound(win);
                pause = !pause;
            }
        }
        else if (gameOver) {
            DrawText("--- YOU LOSE ---", WIDTH / 2 - MeasureText("--- YOU LOSE ---", 50) / 2, HEIGHT / 2 - 60, 50, RED);
            DrawText("PRESS [ENTER] TO PLAY AGAIN", WIDTH / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, HEIGHT / 2 , 20, BLACK);

            if (IsMusicPlaying(background)) StopMusicStream(background);
            if (!pause) {
                PlaySound(lose);
                pause = !pause;
            }
        }

        //暂停
        if (pause && !gameOver) {
            DrawText("PAUSED", WIDTH / 2 - MeasureText("PAUSED", 50) / 2, HEIGHT / 2 - 60, 50, BLACK);
        }

    EndDrawing();
}

static void UnloadGame(Music& background, Sound& shoot, Sound& defeat, Sound& win, Sound& lose) {
    UnloadMusicStream(background);
    UnloadSound(shoot);
    UnloadSound(lose);
    UnloadSound(win);
    UnloadSound(defeat);

    CloseAudioDevice();
    CloseWindow();
}