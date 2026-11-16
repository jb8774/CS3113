/**
 * Author: Janee Bovell
 * Assignment: Rise of the AI
 * Date due: 11/16/2026
 * Built on the CS-UY 3113 instructor starter code.
 * Review and complete the required academic-integrity declaration
 * yourself according to your course policy.
 **/

#include "Map.h"

Map::Map() {}
Map::~Map() { clear(); }

void Map::clear()
{
    delete[] mColliders;
    mColliders = nullptr;
    mCount = 0;
}

void Map::initialise(int level)
{
    clear();

    // Four ground sections with gaps, plus elevated jump platforms.
    // The gaps grow wider in each level.
    float gap = 95.0f + (level - 1) * 37.0f;
    PlatformData layout[] = {
        {    0.0f, 580.0f,  820.0f, 96.0f },
        {  820.0f + gap, 580.0f,  720.0f, 96.0f },
        { 1540.0f + gap * 2.0f, 580.0f, 780.0f, 96.0f },
        { 2320.0f + gap * 3.0f, 580.0f, WORLD_WIDTH - (2320.0f + gap * 3.0f), 96.0f },
        {  285.0f, 462.0f,  238.0f, 32.0f },
        {  586.0f, 409.0f,  135.0f, 32.0f },
        { 1125.0f, 464.0f,  225.0f, 32.0f },
        { 1378.0f, 394.0f,  145.0f, 32.0f },
        { 1955.0f, 470.0f,  205.0f, 32.0f },
        { 2225.0f, 411.0f,  145.0f, 32.0f },
        { 2910.0f, 470.0f,  230.0f, 32.0f },
        { 3210.0f, 408.0f,  175.0f, 32.0f },
        { 3470.0f, 465.0f,  180.0f, 32.0f }
    };
    mCount = static_cast<int>(sizeof(layout) / sizeof(layout[0]));
    mColliders = new Entity[mCount];
    for (int i = 0; i < mCount; i++)
    {
        mPlatforms[i] = layout[i];
        mColliders[i].setEntityType(PLATFORM);
        mColliders[i].setPosition({ layout[i].x + layout[i].width / 2.0f,
                                    layout[i].y + layout[i].height / 2.0f });
        mColliders[i].setColliderDimensions({ layout[i].width, layout[i].height });
    }
}

void Map::render(float cameraX, Texture2D tileTexture) const
{
    Rectangle source = { 0.0f, 0.0f, static_cast<float>(tileTexture.width),
                         static_cast<float>(tileTexture.height) };
    Vector2 origin = { 0.0f, 0.0f };
    for (int i = 0; i < mCount; i++)
    {
        PlatformData p = mPlatforms[i];
        if (p.x + p.width < cameraX || p.x > cameraX + 1200.0f) continue;
        for (float x = p.x; x < p.x + p.width; x += 64.0f)
        {
            float tileWidth = p.x + p.width - x;
            if (tileWidth > 64.0f) tileWidth = 64.0f;
            for (float y = p.y; y < p.y + p.height; y += 64.0f)
            {
                float tileHeight = p.y + p.height - y;
                if (tileHeight > 64.0f) tileHeight = 64.0f;
                Rectangle piece = { x - cameraX, y, tileWidth, tileHeight };
                Rectangle src = { 0.0f, 0.0f, tileWidth, tileHeight };
                DrawTexturePro(tileTexture, src, piece, origin, 0.0f, WHITE);
            }
        }
    }
}
