/**
* Author: Janee Bovell
* Assignment: Pong Clone
* Date due: [10/05/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include "raylib.h"
#include <cmath>

// Global Constants
constexpr int SCREEN_WIDTH  = 1600,
              SCREEN_HEIGHT = 900,
              FPS           = 60;

constexpr Vector2 ORIGIN = {300.0f, 550.0f};
constexpr Vector2 BASE_SIZE = {450.0f, 320.0f};

constexpr char BG_IMAGE[] = "texture/Dungeon_Background.png";
constexpr char GUTS_Standing[] = "texture/guts2.png";
constexpr char GUTS_Attack[] = "texture/guts3.png";
constexpr char Zodd[] = "texture/Zodd.png";
constexpr char Health_Bar[] = "texture/Health_Bar.png";

// Global Variables
AppStatus gAppStatus = RUNNING;

Texture2D gTexture;
Texture2D gGuts_Attack;
Texture2D gZodd;
Texture2D gHealth_Bar;
Texture2D gBackground;

Vector2 gPosition = ORIGIN;
Vector2 gPosition_Health = {300.0f, 20.0f};
Vector2 gPosition_Zodd = {1310.0f, 450.0f};

Vector2 gScale = BASE_SIZE;

Vector2 gBaseScale_Zodd = {250.0f, 180.0f};
Vector2 gScale_Zodd = {250.0f, 180.0f};

float gPulseTime = 0.0f;
float gPreviousTicks = 0.0f;
float gRotation = 0.0f;

int gGutsState = 0;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Berserk Fight");

    gBackground = LoadTexture(BG_IMAGE);

    gTexture = LoadTexture(GUTS_Standing);
    gGuts_Attack = LoadTexture(GUTS_Attack);

    gZodd = LoadTexture(Zodd);
    gHealth_Bar = LoadTexture(Health_Bar);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks = GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gPulseTime += 0.8f * deltaTime;

    // Guts pulse
    gScale = {
        BASE_SIZE.x + 20.0f * (float)cos(gPulseTime),
        BASE_SIZE.y + 20.0f * (float)cos(gPulseTime)
    };

    // Zodd grows at the beginning
    if (gBaseScale_Zodd.x < 650.0f)
    {
        gBaseScale_Zodd.x += 80.0f * deltaTime;
        gBaseScale_Zodd.y += 80.0f * deltaTime;
    }

    // Zodd pulse
    gScale_Zodd = {
        gBaseScale_Zodd.x + 10.0f * (float)cos(gPulseTime),
        gBaseScale_Zodd.y + 10.0f * (float)cos(gPulseTime)
    };

    // Guts stands first, then switches to spin attack
    if (gPulseTime < 2.0f)
    {
        gGutsState = 0;
    }
    else
    {
        gGutsState = 1;

        if (gPosition.x < 1050.0f)
        {
            gPosition.x += 180.0f * deltaTime;
            gPosition.y -= 20.0f * deltaTime;
        }

        gRotation += 360.0f * deltaTime;
    }
}

void render()
{
    BeginDrawing();

    Texture2D currentTexture = gTexture;

    if (gGutsState == 1)
    {
        currentTexture = gGuts_Attack;
    }

    Rectangle textureArea = {
        0.0f,
        0.0f,
        static_cast<float>(currentTexture.width),
        static_cast<float>(currentTexture.height)
    };

    Rectangle textureBack = {
        0.0f,
        0.0f,
        static_cast<float>(gBackground.width),
        static_cast<float>(gBackground.height)
    };

    Rectangle textureArea_Zodd = {
        0.0f,
        0.0f,
        static_cast<float>(gZodd.width),
        static_cast<float>(gZodd.height)
    };

    Rectangle textureArea_Health = {
        0.0f,
        0.0f,
        static_cast<float>(gHealth_Bar.width),
        static_cast<float>(gHealth_Bar.height)
    };

    Rectangle destinationArea = {
        gPosition.x,
        gPosition.y,
        gScale.x,
        gScale.y
    };

    Rectangle destinationAreaBackground = {
        0.0f,
        0.0f,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };

    Rectangle destinationArea_Zodd = {gPosition_Zodd.x,gPosition_Zodd.y,gScale_Zodd.x,gScale_Zodd.y};

    Rectangle destinationArea_Health = {gPosition_Health.x,gPosition_Health.y,1000.0f,150.0f};

    Vector2 originOffest = {gScale.x / 2,gScale.y / 2 };

    Vector2 originOffest_Zodd = {gScale_Zodd.x / 2,gScale_Zodd.y / 2};

    Vector2 BackgroundOffset = {0.0f,0.0f};

    Vector2 originOffest_Health = {0.0f,0.0f};

    DrawTexturePro(gBackground,textureBack,destinationAreaBackground,BackgroundOffset,0.0f,WHITE);

    if (gGutsState == 0)
    {
        DrawTexturePro(currentTexture,textureArea,destinationArea,originOffest, 0.0f,WHITE);
    }
    else
    {
        DrawTexturePro(currentTexture,textureArea,destinationArea,originOffest,gRotation,WHITE);
    }

    DrawTexturePro(gZodd, textureArea_Zodd,destinationArea_Zodd,originOffest_Zodd, 0.0f,WHITE);
    
    DrawTexturePro(gHealth_Bar,textureArea_Health,destinationArea_Health,originOffest_Health,0.0f,WHITE);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gTexture);
    UnloadTexture(gGuts_Attack);
    UnloadTexture(gBackground);
    UnloadTexture(gZodd);
    UnloadTexture(gHealth_Bar);

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