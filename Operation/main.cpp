
#include "DxLib.h"
#include<cstdlib>
#include<ctime>

constexpr int CellSize = 40;     // 1マスのサイズ
constexpr int GridSize = 11;     // マップのサイズ（11×11）
constexpr int WindowSize = GridSize * CellSize;

// 方向定義（上右下左の順で時計回り）
constexpr int dx[4] = { 0, 1, 0, -1 };
constexpr int dy[4] = { -1, 0, 1, 0 };

// ステージデータ（trueが柱）
bool stage[GridSize][GridSize];

// プレイヤーと敵の座標
int playerX = 0, playerY = 0;
int enemyX = GridSize - 2, enemyY = GridSize - 2;
int enemyDir = 3; // 初期方向（左向き）
int enemyMode = 0; // 0: ランダム移動, 1: 右手法, 2: 左手法

// ステージ生成（柱を奇数座標に配置）
void CreateStage()
{
    for (int y = 0; y < GridSize; ++y)
    {
        for (int x = 0; x < GridSize; ++x)
        {
            if (x % 2 == 1 && y % 2 == 1)
                stage[y][x] = true;  // 柱を配置
            else
                stage[y][x] = false; // 通路
        }
    }
}

// プレイヤーの移動処理
void MovePlayer()
{
    if (CheckHitKey(KEY_INPUT_UP) && playerY > 0 && !stage[playerY - 1][playerX]) playerY--;
    if (CheckHitKey(KEY_INPUT_DOWN) && playerY < GridSize - 1 && !stage[playerY + 1][playerX]) playerY++;
    if (CheckHitKey(KEY_INPUT_LEFT) && playerX > 0 && !stage[playerY][playerX - 1]) playerX--;
    if (CheckHitKey(KEY_INPUT_RIGHT) && playerX < GridSize - 1 && !stage[playerY][playerX + 1]) playerX++;
}

// 敵のランダム移動
void MoveEnemyRandom()
{
    int dir = rand() % 4;  // 0～3のランダムな方向
    int nx = enemyX + dx[dir];
    int ny = enemyY + dy[dir];

    if (nx >= 0 && ny >= 0 && nx < GridSize && ny < GridSize && !stage[ny][nx])
    {
        enemyX = nx;
        enemyY = ny;
    }
}

// 敵の右手法移動
void MoveEnemyRightHand()
{
    for (int i = 0; i < 4; ++i)
    {
        int rightDir = (enemyDir + 1) % 4;
        int rx = enemyX + dx[rightDir];
        int ry = enemyY + dy[rightDir];

        if (rx >= 0 && ry >= 0 && rx < GridSize && ry < GridSize && !stage[ry][rx])
        {
            enemyDir = rightDir;
            enemyX = rx;
            enemyY = ry;
            return;
        }

        int fx = enemyX + dx[enemyDir];
        int fy = enemyY + dy[enemyDir];
        if (fx >= 0 && fy >= 0 && fx < GridSize && fy < GridSize && !stage[fy][fx])
        {
            enemyX = fx;
            enemyY = fy;
            return;
        }
        else
        {
            enemyDir = (enemyDir + 3) % 4;
        }
    }
}

// 敵の左手法移動
void MoveEnemyLeftHand()
{
    for (int i = 0; i < 4; ++i)
    {
        int leftDir = (enemyDir + 3) % 4;
        int lx = enemyX + dx[leftDir];
        int ly = enemyY + dy[leftDir];

        if (lx >= 0 && ly >= 0 && lx < GridSize && ly < GridSize && !stage[ly][lx])
        {
            enemyDir = leftDir;
            enemyX = lx;
            enemyY = ly;
            return;
        }

        int fx = enemyX + dx[enemyDir];
        int fy = enemyY + dy[enemyDir];
        if (fx >= 0 && fy >= 0 && fx < GridSize && fy < GridSize && !stage[fy][fx])
        {
            enemyX = fx;
            enemyY = fy;
            return;
        }
        else
        {
            enemyDir = (enemyDir + 1) % 4;
        }
    }
}

// 敵の移動モード切り替え
void SwitchEnemyMode()
{
    if (CheckHitKey(KEY_INPUT_1)) enemyMode = 0; // ランダム移動
    if (CheckHitKey(KEY_INPUT_2)) enemyMode = 1; // 右手法
    if (CheckHitKey(KEY_INPUT_3)) enemyMode = 2; // 左手法
}

// 敵の移動処理
void MoveEnemy()
{
    SwitchEnemyMode();

    if (enemyMode == 0) MoveEnemyRandom();
    else if (enemyMode == 1) MoveEnemyRightHand();
    else if (enemyMode == 2) MoveEnemyLeftHand();
}

// 描画処理
void Draw()
{
    ClearDrawScreen();

    for (int y = 0; y < GridSize; ++y)
    {
        for (int x = 0; x < GridSize; ++x)
        {
            int px = x * CellSize;
            int py = y * CellSize;

            if (stage[y][x])
            {
                DrawBox(px, py, px + CellSize, py + CellSize, GetColor(100, 100, 100), TRUE);
            }
            else
            {
                DrawBox(px, py, px + CellSize, py + CellSize, GetColor(255, 255, 255), TRUE);
            }
        }
    }

    DrawBox(playerX * CellSize, playerY * CellSize, (playerX + 1) * CellSize, (playerY + 1) * CellSize, GetColor(0, 0, 255), TRUE);
    DrawBox(enemyX * CellSize, enemyY * CellSize, (enemyX + 1) * CellSize, (enemyY + 1) * CellSize, GetColor(255, 0, 0), TRUE);

    ScreenFlip();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    ChangeWindowMode(TRUE);
    SetGraphMode(WindowSize, WindowSize, 32);
    DxLib_Init();
    SetDrawScreen(DX_SCREEN_BACK);
    srand(time(nullptr));

    CreateStage();

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        MovePlayer();
        MoveEnemy();
        Draw();
        WaitTimer(100);
    }

    DxLib_End();
    return 0;
}