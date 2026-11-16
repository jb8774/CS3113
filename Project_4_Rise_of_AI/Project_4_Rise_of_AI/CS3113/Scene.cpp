/**
 * Author: Janee Bovell
 * Assignment: Rise of the AI
 * Date due: 11/16/2026
 * Built on the CS-UY 3113 instructor starter code.
 * Review and complete the required academic-integrity declaration
 * yourself according to your course policy.
 **/

#include "Scene.h"

extern Texture2D gMenuBackground;


constexpr int SCENE_WIDTH = 1200;
constexpr int SCENE_HEIGHT = 675;
constexpr float GRAVITY = 880.0f;

Scene::Scene(SceneType type, int level) : mType(type), mLevel(level) {}
Scene::~Scene() { clear(); }

void Scene::setSounds(Sound *jump, Sound *slash)
{
    mJumpSound = jump;
    mSlashSound = slash;
}

void Scene::clear()
{
    delete mPlayer;
    mPlayer = nullptr;
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        delete mEnemies[i];
        mEnemies[i] = nullptr;
    }
    mEnemyCount = 0;
    for (int i = 0; i < MAX_PLAYER_ARROWS; i++) mPlayerArrows[i].active = false;
    for (int i = 0; i < MAX_ENEMIES; i++) mEnemyArrows[i].active = false;
    mMap.clear();
    if (mIsReady)
    {
        UnloadTexture(mBackground);
        UnloadTexture(mArrowTexture);
        UnloadTexture(mTile);
        UnloadTexture(mPortal);
    }
    mIsReady = false;
}

void Scene::makePlayer()
{
    delete mPlayer;
    // Rearranged frames from the user-provided GandalfHardcore archer sheet:
    // 8 frames per row: idle, run, bow shot, damage.
    std::map<Direction, std::vector<int>> atlas = {
        { RIGHT, { 8, 9, 10, 11, 12, 13, 14, 15 } },
        { LEFT,  { 8, 9, 10, 11, 12, 13, 14, 15 } },
        { UP,    { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { DOWN,  { 16, 17, 18, 19, 20, 21, 22, 23 } }
    };
    mPlayer = new Entity({ 90.0f, 485.0f }, { 88.0f, 88.0f },
                         "assets/game/archer_atlas.png", ATLAS, { 4, 8 }, atlas, PLAYER);
    mPlayer->setColliderDimensions({ 35.0f, 64.0f });
    mPlayer->setSpeed(250);
    mPlayer->setFrameSpeed(11);
    mPlayer->setJumpingPower(495.0f);
    mPlayer->setAcceleration({ 0.0f, GRAVITY });
    mFacingRight = true;
    mAttackTime = 0.0f;
    mAttackUnspent = false;
}

void Scene::initialiseLevel()
{
    if (mType != LEVEL_SCENE) return;
    clear();
    mMap.initialise(mLevel);
    makePlayer();

    const char *theme = (mLevel == 1) ? "forest" : ((mLevel == 2) ? "moon" : "castle");
    std::string tile = std::string("assets/game/tile_") + theme + ".png";
    mBackground = LoadTexture("assets/game/ruins_background.png");
    mArrowTexture = LoadTexture("assets/game/archer_arrow.png");
    mTile = LoadTexture(tile.c_str());
    mPortal = LoadTexture("assets/game/portal.png");
    mIsReady = true;

    // All 3 AI types appear in every level. Later levels add more enemies.
    const float spots[9] = { 430.0f, 1150.0f, 2080.0f, 2450.0f, 2930.0f,
                             3240.0f, 3530.0f, 3780.0f, 1650.0f };
    mEnemyCount = (mLevel == 1) ? 3 : ((mLevel == 2) ? 6 : 9);
    for (int i = 0; i < mEnemyCount; i++)
    {
        int kind = i % 3;
        // Three new goblin enemies: scout patrol, tank pursuer, archer ranger.
        const char *path = kind == 0 ? "assets/game/goblin_scout_atlas.png" :
                           (kind == 1 ? "assets/game/goblin_tank_atlas.png" :
                                        "assets/game/goblin_ranger_atlas.png");
        std::map<Direction, std::vector<int>> anim = {
            { RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7 } },
            { LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7 } },
            { UP,    { 0, 1, 2, 3, 4, 5, 6, 7 } },
            { DOWN,  { 0, 1, 2, 3, 4, 5, 6, 7 } }
        };
        mEnemies[i] = new Entity({ spots[i], 505.0f }, { 84.0f, 90.0f }, path,
                                 ATLAS, { 1, 8 }, anim, NPC);
        mEnemies[i]->setColliderDimensions({ 40.0f, 58.0f });
        mEnemies[i]->setSpeed((kind == 0 ? 90 : (kind == 1 ? 110 : 82)) + mLevel * 12);
        mEnemies[i]->setFrameSpeed(9);
        mEnemies[i]->setAIType(kind == 0 ? WANDERER : (kind == 1 ? FOLLOWER : RANGER));
        mEnemies[i]->setAcceleration({ 0.0f, GRAVITY });
        mEnemyStart[i] = spots[i];
        mEnemyShotDelay[i] = 1.3f + 0.1f * i;
    }
    mCameraX = 0.0f;
    mInvincible = 0.0f;
}

void Scene::respawnPlayer()
{
    makePlayer();
    mCameraX = 0.0f;
    mInvincible = 1.5f;
}

void Scene::processInput()
{
    if (mType != LEVEL_SCENE || mPlayer == nullptr) return;
    mPlayer->resetMovement();
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        mPlayer->moveLeft();
        mFacingRight = false;
    }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        mPlayer->moveRight();
        mFacingRight = true;
    }
    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && mPlayer->isCollidingBottom())
    {
        mPlayer->jump();
        if (mJumpSound != nullptr) PlaySound(*mJumpSound);
    }
    if (IsKeyPressed(KEY_SPACE) && mAttackTime <= 0.0f)
    {
        mAttackTime = 0.28f;
        mAttackUnspent = true;
        // Fire one arrow on each press of Space.
        for (int i = 0; i < MAX_PLAYER_ARROWS; i++)
        {
            if (!mPlayerArrows[i].active)
            {
                Vector2 p = mPlayer->getPosition();
                mPlayerArrows[i].position = { p.x + (mFacingRight ? 31.0f : -31.0f), p.y - 10.0f };
                mPlayerArrows[i].speed = mFacingRight ? 630.0f : -630.0f;
                mPlayerArrows[i].active = true;
                break;
            }
        }
        if (mSlashSound != nullptr) PlaySound(*mSlashSound);
    }
}

SceneEvent Scene::update(float deltaTime)
{
    if (mType != LEVEL_SCENE || mPlayer == nullptr) return NOTHING_HAPPENED;
    if (mInvincible > 0.0f) mInvincible -= deltaTime;
    if (mAttackTime > 0.0f) mAttackTime -= deltaTime;

    // The same movement/atlas/direction pattern as the Entity class provided.
    if (mAttackTime > 0.0f) mPlayer->setDirection(DOWN);
    else if (GetLength(mPlayer->getMovement()) == 0.0f) mPlayer->setDirection(UP);
    else mPlayer->setDirection(mFacingRight ? RIGHT : LEFT);
    mPlayer->setFlipX(!mFacingRight);
    mPlayer->update(deltaTime, nullptr, mMap.getColliders(), mMap.getCount(), nullptr, 0);

    // Player arrows fly across the scrolling world and disappear when they hit an enemy.
    for (int j = 0; j < MAX_PLAYER_ARROWS; j++)
    {
        if (!mPlayerArrows[j].active) continue;
        mPlayerArrows[j].position.x += mPlayerArrows[j].speed * deltaTime;
        if (mPlayerArrows[j].position.x < 0.0f ||
            mPlayerArrows[j].position.x > Map::WORLD_WIDTH)
            mPlayerArrows[j].active = false;
    }

    for (int i = 0; i < mEnemyCount; i++)
    {
        Entity *enemy = mEnemies[i];
        if (!enemy->isActive()) continue;
        if (enemy->getAIType() == WANDERER)
        {
            if (enemy->getPosition().x > mEnemyStart[i] + 88.0f) enemy->setDirection(LEFT);
            if (enemy->getPosition().x < mEnemyStart[i] - 88.0f) enemy->setDirection(RIGHT);
        }
        enemy->update(deltaTime, mPlayer, mMap.getColliders(), mMap.getCount(), nullptr, 0);
        enemy->setFlipX(enemy->getMovement().x < 0.0f);

        Vector2 a = mPlayer->getPosition();
        Vector2 b = enemy->getPosition();
        float dx = b.x - a.x;
        float dy = fabs(b.y - a.y);
        for (int j = 0; j < MAX_PLAYER_ARROWS; j++)
        {
            if (mPlayerArrows[j].active &&
                fabs(mPlayerArrows[j].position.x - b.x) < 27.0f &&
                fabs(mPlayerArrows[j].position.y - b.y) < 36.0f)
            {
                mPlayerArrows[j].active = false;
                enemy->deactivate();
                break;
            }
        }
        if (!enemy->isActive()) continue;

        // The third AI keeps its distance and shoots at the archer.
        if (enemy->getAIType() == RANGER)
        {
            mEnemyShotDelay[i] -= deltaTime;
            if (mEnemyShotDelay[i] <= 0.0f && fabs(dx) < 550.0f && dy < 110.0f)
            {
                mEnemyArrows[i].position = { b.x + (dx < 0.0f ? -25.0f : 25.0f), b.y - 10.0f };
                mEnemyArrows[i].speed = dx < 0.0f ? -340.0f : 340.0f;
                mEnemyArrows[i].active = true;
                mEnemyShotDelay[i] = 1.8f - 0.2f * mLevel;
            }
        }
        if (mInvincible <= 0.0f && fabs(dx) < 38.0f && dy < 65.0f)
        {
            mInvincible = 1.5f;
            return PLAYER_HIT;
        }
        if (b.y > 820.0f) enemy->deactivate();
    }
    for (int i = 0; i < mEnemyCount; i++)
    {
        if (!mEnemyArrows[i].active) continue;
        mEnemyArrows[i].position.x += mEnemyArrows[i].speed * deltaTime;
        Vector2 p = mPlayer->getPosition();
        if (fabs(mEnemyArrows[i].position.x - p.x) < 22.0f &&
            fabs(mEnemyArrows[i].position.y - p.y) < 39.0f)
        {
            mEnemyArrows[i].active = false;
            if (mInvincible <= 0.0f) return PLAYER_HIT;
        }
        if (mEnemyArrows[i].position.x < 0.0f ||
            mEnemyArrows[i].position.x > Map::WORLD_WIDTH)
            mEnemyArrows[i].active = false;
    }
    mAttackUnspent = false;
    if (mPlayer->getPosition().y > 740.0f) return PLAYER_HIT;
    if (mPlayer->getPosition().x >= 3850.0f && mPlayer->getPosition().y > 345.0f)
        return LEVEL_FINISHED;

    mCameraX = mPlayer->getPosition().x - 480.0f;
    if (mCameraX < 0.0f) mCameraX = 0.0f;
    if (mCameraX > Map::WORLD_WIDTH - SCENE_WIDTH)
        mCameraX = Map::WORLD_WIDTH - SCENE_WIDTH;
    return NOTHING_HAPPENED;
}

void Scene::render(int lives)
{
    if (mType == MENU_SCENE)
    {
ClearBackground(BLACK);

DrawTexturePro(
        gMenuBackground,
    {
        0.0f,
        0.0f,
        (float)gMenuBackground.width,
        (float)gMenuBackground.height
    },
    {
        0.0f,
        0.0f,
        (float)SCENE_WIDTH,
        (float)SCENE_HEIGHT
    },
    { 0.0f, 0.0f },
    0.0f,
    WHITE
);

    // menu background
    float imageWidth = (float)gMenuBackground.width;
    float imageHeight = imageWidth * SCENE_HEIGHT / SCENE_WIDTH;

    if (imageHeight > gMenuBackground.height)
    {
        imageHeight = (float)gMenuBackground.height;
        imageWidth = imageHeight * SCENE_WIDTH / SCENE_HEIGHT;
    }

    Rectangle source = {
        ((float)gMenuBackground.width - imageWidth) / 2.0f,
        ((float)gMenuBackground.height - imageHeight) / 2.0f,
        imageWidth,
        imageHeight
    };

    Rectangle destination = {
        0.0f, 0.0f,
        (float)SCENE_WIDTH,
        (float)SCENE_HEIGHT
    };


    DrawRectangle(
        0, 0, SCENE_WIDTH, SCENE_HEIGHT,
        Color{ 0, 0, 0, 115 }
    );

    // game title
    const char *title = "SHADOWS OF EXPEDIA";
    int titleSize = 65;

    DrawText(
        title,
        (SCENE_WIDTH - MeasureText(title, titleSize)) / 2,
        150,
        titleSize,
        ColorFromHex("#e5d3aa")
    );

    // start instruction
    const char *startText = "PRESS ENTER TO START";

    DrawText(
        startText,
        (SCENE_WIDTH - MeasureText(startText, 32)) / 2,
        440,
        32,
        GOLD
    );

    // controls
    DrawText(
        "A / D MOVE       W JUMP       SPACE SHOOT       Q QUIT",
        275, 560, 22, WHITE
    );

    return;
}
    if (mType == WIN_SCENE || mType == LOSE_SCENE)
    {
        ClearBackground(ColorFromHex("#1d2340"));
        if (mType == WIN_SCENE)
        {
            DrawText("THE CURSE IS BROKEN", 254, 203, 53, ColorFromHex("#e7d399"));
            DrawText("YOU WIN! ALL 3 LEVELS CLEARED", 301, 289, 28, WHITE);
        }
        else
        {
            DrawText("THE SHADOWS PREVAIL", 242, 203, 53, ColorFromHex("#e8a3aa"));
            DrawText("YOU LOSE - NO LIVES REMAIN", 321, 289, 28, WHITE);
        }
        DrawText("PRESS ENTER TO RETURN TO MENU", 307, 406, 27, ColorFromHex("#b7dce0"));
        return;
    }
    // Scroll the user-provided ruins background behind the platforms.
    // The square image repeats; keep its pixels square at 675 x 675.
    Color backgroundTint = mLevel == 1 ? WHITE :
                           (mLevel == 2 ? ColorFromHex("#a6bce2") :
                                          ColorFromHex("#a592b6"));
    float offset = fmod(mCameraX * 0.18f, 675.0f);
    Rectangle backgroundSource = { 0.0f, 0.0f,
        static_cast<float>(mBackground.width), static_cast<float>(mBackground.height) };
    for (int i = 0; i < 3; i++)
    {
        Rectangle backgroundDest = { i * 675.0f - offset, 0.0f, 675.0f, 675.0f };
        DrawTexturePro(mBackground, backgroundSource, backgroundDest,
                       { 0.0f, 0.0f }, 0.0f, backgroundTint);
    }
    mMap.render(mCameraX, mTile);
    float portalX = 3880.0f - mCameraX;
    if (portalX > -100.0f && portalX < SCENE_WIDTH + 100.0f)
    {
        Rectangle gateSource = { 0.0f, 0.0f, (float)mPortal.width, (float)mPortal.height };
        Rectangle gateDest = { portalX, 502.0f, 100.0f, 150.0f };
        DrawTexturePro(mPortal, gateSource, gateDest, { 50.0f, 75.0f }, 0.0f, WHITE);
    }
    for (int i = 0; i < mEnemyCount; i++)
        if (mEnemies[i]->isActive()) mEnemies[i]->render(mCameraX);

    if (mInvincible <= 0.0f || static_cast<int>(mInvincible * 12.0f) % 2 == 0)
        mPlayer->render(mCameraX);

    // Render arrows from both the player and goblin ranger.
    Rectangle arrowSource = { 0.0f, 0.0f,
        static_cast<float>(mArrowTexture.width), static_cast<float>(mArrowTexture.height) };
    for (int i = 0; i < MAX_PLAYER_ARROWS; i++)
    {
        if (!mPlayerArrows[i].active) continue;
        Rectangle source = arrowSource;
        if (mPlayerArrows[i].speed < 0.0f) source.width = -source.width;
        Rectangle dest = { mPlayerArrows[i].position.x - mCameraX,
                           mPlayerArrows[i].position.y, 46.0f, 10.0f };
        DrawTexturePro(mArrowTexture, source, dest, { 23.0f, 5.0f }, 0.0f, WHITE);
    }
    for (int i = 0; i < mEnemyCount; i++)
    {
        if (!mEnemyArrows[i].active) continue;
        Rectangle source = arrowSource;
        if (mEnemyArrows[i].speed < 0.0f) source.width = -source.width;
        Rectangle dest = { mEnemyArrows[i].position.x - mCameraX,
                           mEnemyArrows[i].position.y, 46.0f, 10.0f };
        DrawTexturePro(mArrowTexture, source, dest, { 23.0f, 5.0f }, 0.0f, ORANGE);
    }
    DrawRectangle(0, 0, SCENE_WIDTH, 51, ColorFromHex("#1f263b"));
    std::string ui = "LEVEL " + std::to_string(mLevel) + "/3     LIVES: " +
                     std::to_string(lives) + "     A/D: MOVE   W: JUMP   SPACE: SHOOT";
    DrawText(ui.c_str(), 19, 14, 23, WHITE);
    DrawText("1 / 2 / 3: LEVEL DEBUG    Q: QUIT", 775, 640, 16, WHITE);
}
