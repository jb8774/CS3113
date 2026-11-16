#ifndef MAP_H
#define MAP_H

#include "Entity.h"

struct PlatformData
{
    float x;
    float y;
    float width;
    float height;
};

class Map
{
private:
    static constexpr int MAX_PLATFORMS = 20;
    PlatformData mPlatforms[MAX_PLATFORMS];
    Entity *mColliders = nullptr;
    int mCount = 0;

public:
    static constexpr float WORLD_WIDTH = 4000.0f;
    Map();
    ~Map();
    void initialise(int level);
    void clear();
    void render(float cameraX, Texture2D tileTexture) const;
    Entity *getColliders() { return mColliders; }
    int getCount() const { return mCount; }
};

#endif
