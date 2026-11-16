/**
 * Author: Janee Bovell
 * Assignment: Rise of the AI
 * Date due: 11/16/2026
 * Built on the CS-UY 3113 instructor starter code.
 * Review and complete the required academic-integrity declaration
 * yourself according to your course policy.
 **/

#include "CS3113/Scene.h"

// Global Constants
constexpr int SCREEN_WIDTH = 1200,
              SCREEN_HEIGHT = 675,
              FPS = 120;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f,
      gTimeAccumulator = 0.0f;
int gLives = 3;
Texture2D gMenuBackground;

// Menu, levels, and ending screens are different Scene objects.
Scene gMenu(MENU_SCENE);
Scene gLevel1(LEVEL_SCENE, 1);
Scene gLevel2(LEVEL_SCENE, 2);
Scene gLevel3(LEVEL_SCENE, 3);
Scene gLose(LOSE_SCENE);
Scene gWin(WIN_SCENE);
Scene *gCurrentScene = &gMenu;

Music gMusic;
Sound gJumpSound, gSlashSound, gHitSound, gWinSound;

void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shadows of Expedia");
    InitAudioDevice();
    gMenuBackground = LoadTexture("assets/game/main_background.jpg");
    gMusic = LoadMusicStream("assets/audio/prophecy.mp3");
    gMusic.looping = true;
    SetMusicVolume(gMusic, 0.28f);
    PlayMusicStream(gMusic);
    gJumpSound = LoadSound("assets/audio/jump.wav");
    gSlashSound = LoadSound("assets/audio/slash.wav");
    gHitSound = LoadSound("assets/audio/hit.wav");
    gWinSound = LoadSound("assets/audio/victory.wav");
    gLevel1.setSounds(&gJumpSound, &gSlashSound);
    gLevel2.setSounds(&gJumpSound, &gSlashSound);
    gLevel3.setSounds(&gJumpSound, &gSlashSound);
    SetTargetFPS(FPS);
    gPreviousTicks = static_cast<float>(GetTime());
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    if (gCurrentScene == &gMenu && IsKeyPressed(KEY_ENTER))
    {
        gLives = 3;
        gLevel1.initialiseLevel();
        gCurrentScene = &gLevel1;
    }
    else if ((gCurrentScene == &gLose || gCurrentScene == &gWin) &&
             IsKeyPressed(KEY_ENTER))
    {
        gCurrentScene = &gMenu;
    }
    else if (gCurrentScene->isLevel())
    {
        // Optional keyboard debug level switch, as required by the brief.
        if (IsKeyPressed(KEY_ONE))
        {
            gLevel1.initialiseLevel();
            gCurrentScene = &gLevel1;
        }
        else if (IsKeyPressed(KEY_TWO))
        {
            gLevel2.initialiseLevel();
            gCurrentScene = &gLevel2;
        }
        else if (IsKeyPressed(KEY_THREE))
        {
            gLevel3.initialiseLevel();
            gCurrentScene = &gLevel3;
        }
        gCurrentScene->processInput();
    }
}

void update()
{
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    UpdateMusicStream(gMusic);
    if (!gCurrentScene->isLevel())
    {
        gTimeAccumulator = 0.0f;
        return;
    }
    if (deltaTime > 0.15f) deltaTime = 0.15f;
    deltaTime += gTimeAccumulator;
    while (deltaTime >= FIXED_TIMESTEP)
    {
        SceneEvent event = gCurrentScene->update(FIXED_TIMESTEP);
        if (event == PLAYER_HIT)
        {
            PlaySound(gHitSound);
            gLives--;

            StopMusicStream(gMusic);
            PlayMusicStream(gMusic);

            if (gLives <= 0) gCurrentScene = &gLose;
            else gCurrentScene->respawnPlayer();
            deltaTime = 0.0f;
            break;
        }
        else if (event == LEVEL_FINISHED)
        {
            if (gCurrentScene == &gLevel1)
            {
                gLevel2.initialiseLevel();
                gCurrentScene = &gLevel2;
            }
            else if (gCurrentScene == &gLevel2)
            {
                gLevel3.initialiseLevel();
                gCurrentScene = &gLevel3;
            }
            else
            {
                PlaySound(gWinSound);
                gCurrentScene = &gWin;
            }
            deltaTime = 0.0f;
            break;
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    gCurrentScene->render(gLives);
    EndDrawing();
}

void shutdown()
{
    // Unload textures while the graphics context still exists.
    gLevel1.clear();
    gLevel2.clear();
    gLevel3.clear();
    UnloadTexture(gMenuBackground);
    UnloadSound(gJumpSound);
    UnloadSound(gSlashSound);
    UnloadSound(gHitSound);
    UnloadSound(gWinSound);
    UnloadMusicStream(gMusic);
    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();
    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }
    shutdown();
    return 0;
}
