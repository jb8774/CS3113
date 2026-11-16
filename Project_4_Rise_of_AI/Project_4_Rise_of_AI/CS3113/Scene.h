#ifndef SCENE_H
#define SCENE_H

#include "Map.h"

enum SceneType { MENU_SCENE, LEVEL_SCENE, LOSE_SCENE, WIN_SCENE };
enum SceneEvent { NOTHING_HAPPENED, PLAYER_HIT, LEVEL_FINISHED };

class Scene
{
private:
    static constexpr int MAX_ENEMIES = 12;
    SceneType mType;
    int mLevel;
    Map mMap;
    Entity *mPlayer = nullptr;
    Entity *mEnemies[MAX_ENEMIES] = {};
    float mEnemyStart[MAX_ENEMIES] = {};
    float mEnemyShotDelay[MAX_ENEMIES] = {};
    // A projectile uses a position and horizontal speed, like our Entity physics.
    struct Arrow { Vector2 position = { 0.0f, 0.0f }; float speed = 0.0f; bool active = false; };
    static constexpr int MAX_PLAYER_ARROWS = 12;
    Arrow mPlayerArrows[MAX_PLAYER_ARROWS] = {};
    Arrow mEnemyArrows[MAX_ENEMIES] = {};
    int mEnemyCount = 0;
    Texture2D mBackground = {};
    Texture2D mArrowTexture = {};
    Texture2D mTile = {};
    Texture2D mPortal = {};
    float mCameraX = 0.0f;
    float mInvincible = 0.0f;
    float mAttackTime = 0.0f;
    bool mFacingRight = true;
    bool mAttackUnspent = false;
    bool mIsReady = false;
    Sound *mJumpSound = nullptr;
    Sound *mSlashSound = nullptr;

    void makePlayer();

public:
    Scene(SceneType type, int level = 0);
    ~Scene();
    void clear();
    void initialiseLevel();
    void setSounds(Sound *jump, Sound *slash);
    void processInput();
    SceneEvent update(float deltaTime);
    void render(int lives);
    void respawnPlayer();
    bool isLevel() const { return mType == LEVEL_SCENE; }
    int getLevel() const { return mLevel; }
};

#endif
