/**
 * Author: Janee Bovell
 * Assignment: Lunar Lander!
 * Date due: 10/30/2026
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600,
              SCREEN_HEIGHT = 900,
              FPS           = 120;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_PADS = 5,
                NUMBER_OF_ROOFS = 4,
                NUMBER_OF_BACKGROUNDS = 6,
                LANDINGS_TO_WIN = 5;

constexpr float FIXED_TIMESTEP          = 1.0f / 60.0f,
                ACCELERATION_OF_GRAVITY = 70.0f,
                HORIZONTAL_ACCELERATION = 600.0f,
                LIFT_ACCELERATION       = 155.0f,
                FUEL_BURN               = 7.5f,
                SAFE_VERTICAL_SPEED     = 205.0f,
                SAFE_HORIZONTAL_SPEED   = 115.0f,
                MOVING_PAD_SPEED        = 45.0f;

constexpr char BG_COLOUR[] = "#302D44";

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f,
      gTimeAccumulator = 0.0f,
      gFuel = 100.0f,
      gThrustX = 0.0f,
      gLift = 0.0f,
      gMovingPadDirection = 1.0f,
      gLandingMessageTime = 0.0f;

int gScore = 0,
    gLandings = 0,
    gLastLandingPoints = 0;

bool gMouseWasDown = false;

bool gMissionComplete = false,
     gMissionFailed   = false;

Entity *gHelicopter = nullptr;
Entity *gPads       = nullptr;

// Each rooftop is worth a different number of points.
const int gPadPoints[NUMBER_OF_PADS] = { 100, 200, 300, 400, 500 };

const Vector2 gStartPadPositions[NUMBER_OF_PADS] = {
    { 170.0f, 495.0f }, // pink roof
    { 470.0f, 291.0f }, // purple roof
    { 755.0f, 457.0f }, // red roof
    { 1295.0f, 348.0f }, // blue roof
    { 1470.0f, 348.0f }  // moving cyan roof
};

// The RESTART button at the top-right of the game window.
const Rectangle gRestartButton = { 1360.0f, 12.0f, 222.0f, 58.0f };

const Vector2 gRoofPositions[NUMBER_OF_ROOFS] = {
    { 188.0f, 500.0f }, // lower-left building
    { 475.0f, 296.0f }, // tall building
    { 752.0f, 462.0f }, // middle building
    { 1406.0f, 353.0f } // wide right building
};
const Vector2 gRoofDimensions[NUMBER_OF_ROOFS] = {
    { 280.0f, 0.0f },
    { 289.0f, 0.0f },
    { 270.0f, 0.0f },
    { 387.0f, 0.0f }
};

Texture2D gCityBuildings;
Texture2D gBackgrounds[NUMBER_OF_BACKGROUNDS];
Texture2D gStars[NUMBER_OF_PADS];
float gBackgroundX[NUMBER_OF_BACKGROUNDS] = { 0.0f };

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void resetHelicopter();
void restartGame();


void resetHelicopter()
{
    if (gHelicopter != nullptr) delete gHelicopter;

    std::map<Direction, std::vector<int>> helicopterAnimation = {
        { RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { UP,    { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { DOWN,  { 0, 1, 2, 3, 4, 5, 6, 7 } }
    };

    gHelicopter = new Entity(
        { 1040.0f, 95.0f }, // start between rooftops; no automatic first landing
        { 126.0f, 42.0f },
        "assets/game/helicopter.png",
        ATLAS,
        { 1, 8 },
        helicopterAnimation
    );
    gHelicopter->setColliderDimensions({ 78.0f, 26.0f });
    gHelicopter->setFrameSpeed(12);
    gHelicopter->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
}

// R and the on-screen RESTART button begin a fresh run.
// Successful landings use resetHelicopter instead to keep the score.
void restartGame()
{
    gScore = 0;
    gLandings = 0;
    gLastLandingPoints = 0;
    gLandingMessageTime = 0.0f;
    gFuel = 100.0f;
    gThrustX = 0.0f;
    gLift = 0.0f;
    gMissionComplete = false;
    gMissionFailed = false;
    gMovingPadDirection = 1.0f;
    gPads[4].setPosition(gStartPadPositions[4]);

    resetHelicopter();
    gTimeAccumulator = 0.0f;
    gPreviousTicks = static_cast<float>(GetTime());
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 3 - Helicopter Lander");

    resetHelicopter();


    const char *padFiles[NUMBER_OF_PADS] = {
        "assets/game/pad_pink.png",
        "assets/game/pad_purple.png",
        "assets/game/pad_red.png",
        "assets/game/pad_blue.png",
        "assets/game/pad_cyan.png"
    };
    const char *starFiles[NUMBER_OF_PADS] = {
        "assets/game/star_pink.png",
        "assets/game/star_purple.png",
        "assets/game/star_red.png",
        "assets/game/star_blue.png",
        "assets/game/star_cyan.png"
    };
    gPads = new Entity[NUMBER_OF_PADS];
    for (int i = 0; i < NUMBER_OF_PADS; i++)
    {
        gPads[i].setTexture(padFiles[i]);
        gPads[i].setScale({ 168.0f, 54.0f });
        gPads[i].setColliderDimensions({ 166.0f, 12.0f });
        gPads[i].setPosition(gStartPadPositions[i]);
        gStars[i] = LoadTexture(starFiles[i]);
    }


    gCityBuildings = LoadTexture("assets/game/city_rooftops.png");
    const char *backgroundFiles[NUMBER_OF_BACKGROUNDS] = {
        "assets/game/city_1.png",
        "assets/game/city_2.png",
        "assets/game/city_3.png",
        "assets/game/city_4.png",
        "assets/game/city_5.png",
        "assets/game/city_6.png"
    };
    for (int i = 0; i < NUMBER_OF_BACKGROUNDS; i++)
        gBackgrounds[i] = LoadTexture(backgroundFiles[i]);

    SetTargetFPS(FPS);
    gPreviousTicks = static_cast<float>(GetTime());
}

void processInput()
{
    if (IsKeyPressed(KEY_R)) restartGame();

    Vector2 mousePosition = GetMousePosition();
    bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (mouseDown && !gMouseWasDown &&
        mousePosition.x >= gRestartButton.x &&
        mousePosition.x <= gRestartButton.x + gRestartButton.width &&
        mousePosition.y >= gRestartButton.y &&
        mousePosition.y <= gRestartButton.y + gRestartButton.height)
    {
        restartGame();
    }
    gMouseWasDown = mouseDown;

    // Only control acceleration.
    gThrustX = 0.0f;
    gLift = 0.0f;

    if (!gMissionComplete && !gMissionFailed)
    {
        if      (IsKeyDown(KEY_A)) gThrustX = -1.0f;
        else if (IsKeyDown(KEY_D)) gThrustX =  1.0f;

        if (IsKeyDown(KEY_W)) gLift = 1.0f;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    if (gMissionComplete || gMissionFailed) return;

    deltaTime += gTimeAccumulator;
    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        bool landedThisStep = false;

        float horizontal = gThrustX;
        float lift       = gLift;

        gHelicopter->setAcceleration({
            horizontal * HORIZONTAL_ACCELERATION,
            ACCELERATION_OF_GRAVITY - lift * LIFT_ACCELERATION
        });

        Vector2 movingPadPosition = gPads[4].getPosition();
        movingPadPosition.x += MOVING_PAD_SPEED * gMovingPadDirection * FIXED_TIMESTEP;

        if (movingPadPosition.x >= 1510.0f) gMovingPadDirection = -1.0f;
        if (movingPadPosition.x <= 1430.0f) gMovingPadDirection =  1.0f;
        gPads[4].setPosition(movingPadPosition);


        Vector2 landingVelocity = gHelicopter->getVelocity();
        landingVelocity.x += gHelicopter->getAcceleration().x * FIXED_TIMESTEP;
        landingVelocity.y += gHelicopter->getAcceleration().y * FIXED_TIMESTEP;

        gHelicopter->update(FIXED_TIMESTEP, gPads, NUMBER_OF_PADS);

        Vector2 helicopterPosition = gHelicopter->getPosition();
        Vector2 helicopterCollider = gHelicopter->getColliderDimensions();

        if (helicopterPosition.y + helicopterCollider.y / 2.0f >= SCREEN_HEIGHT - 26.0f ||
            helicopterPosition.y - helicopterCollider.y / 2.0f <= 0.0f ||
            helicopterPosition.x - helicopterCollider.x / 2.0f <= 0.0f ||
            helicopterPosition.x + helicopterCollider.x / 2.0f >= SCREEN_WIDTH)
        {
            gMissionFailed = true;
        }

        if (gHelicopter->isCollidingBottom())
        {
            int landedPad = -1;
            float closestDistance = 10000.0f;
            for (int i = 0; i < NUMBER_OF_PADS; i++)
            {
                Vector2 padPosition = gPads[i].getPosition();
                float distanceX = fabs(helicopterPosition.x - padPosition.x);
                float distanceY = fabs(helicopterPosition.y - padPosition.y);
                float landingHalfWidth =
                    (helicopterCollider.x + gPads[i].getColliderDimensions().x) / 2.0f;

                if (distanceX < landingHalfWidth &&
                    distanceY < 35.0f && distanceX < closestDistance)
                {
                    landedPad = i;
                    closestDistance = distanceX;
                }
            }

            if (landedPad >= 0 &&
                landingVelocity.y >= 0.0f &&
                landingVelocity.y <= SAFE_VERTICAL_SPEED &&
                fabs(landingVelocity.x) <= SAFE_HORIZONTAL_SPEED)
            {
                gLastLandingPoints = gPadPoints[landedPad];
                gScore += gLastLandingPoints;
                gLandings++;
                gLandingMessageTime = 2.0f;
                landedThisStep = true;

                if (gLandings >= LANDINGS_TO_WIN)
                    gMissionComplete = true; // finish the mission after 5 landings
                else
                {
                    gFuel = 100.0f;          // refuel for the next landing
                    resetHelicopter();        // start in the air with zero speed
                }
            }
            else
            {
                gMissionFailed = true; // unsafe landing or missed halo
            }
        }


        if (!gMissionComplete && !gMissionFailed)
        {
            for (int i = 0; i < NUMBER_OF_ROOFS; i++)
            {
                float distanceX = fabs(helicopterPosition.x - gRoofPositions[i].x);
                float totalHalfWidth = (helicopterCollider.x + gRoofDimensions[i].x) / 2.0f;

                if (distanceX < totalHalfWidth &&
                    helicopterPosition.y + helicopterCollider.y / 2.0f >= gRoofPositions[i].y)
                {
                    gMissionFailed = true;
                    break;
                }
            }
        }


        float backgroundSpeed[NUMBER_OF_BACKGROUNDS] = {
            0.0f, 5.0f, 11.0f, 18.0f, 26.0f, 36.0f
        };
        for (int i = 1; i < NUMBER_OF_BACKGROUNDS; i++)
        {
            gBackgroundX[i] -= backgroundSpeed[i] * FIXED_TIMESTEP;
            if (gBackgroundX[i] <= -SCREEN_WIDTH)
                gBackgroundX[i] += SCREEN_WIDTH;
        }

        if (gLandingMessageTime > 0.0f)
            gLandingMessageTime -= FIXED_TIMESTEP;

        deltaTime -= FIXED_TIMESTEP;
        if (landedThisStep) deltaTime = 0.0f;
        if (gMissionComplete || gMissionFailed || landedThisStep) break;
    }

    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));


    for (int i = 0; i < NUMBER_OF_BACKGROUNDS; i++)
    {
        Rectangle sourceArea = {
            0.0f, 0.0f,
            static_cast<float>(gBackgrounds[i].width),
            static_cast<float>(gBackgrounds[i].height)
        };
        Rectangle destinationArea = {
            gBackgroundX[i], 0.0f,
            static_cast<float>(SCREEN_WIDTH),
            static_cast<float>(SCREEN_HEIGHT)
        };
        Vector2 originOffset = { 0.0f, 0.0f };

        DrawTexturePro(gBackgrounds[i], sourceArea, destinationArea,
                       originOffset, 0.0f, WHITE);

        if (i > 0)
        {
            destinationArea.x += SCREEN_WIDTH;
            DrawTexturePro(gBackgrounds[i], sourceArea, destinationArea,
                           originOffset, 0.0f, WHITE);
        }
    }


    Rectangle buildingSource = {
        0.0f, 0.0f,
        static_cast<float>(gCityBuildings.width),
        static_cast<float>(gCityBuildings.height)
    };
    Rectangle buildingDestination = {
        0.0f, 260.0f,
        static_cast<float>(SCREEN_WIDTH),
        1165.0f
    };
    Vector2 buildingOrigin = { 0.0f, 0.0f };
    DrawTexturePro(gCityBuildings, buildingSource, buildingDestination,
                   buildingOrigin, 0.0f, WHITE);

    for (int i = 0; i < NUMBER_OF_PADS; i++)
    {
        Vector2 padPosition = gPads[i].getPosition();
        Rectangle starSource = {
            0.0f, 0.0f,
            static_cast<float>(gStars[i].width),
            static_cast<float>(gStars[i].height)
        };
        Rectangle starDestination = {
            padPosition.x, padPosition.y - 64.0f, 34.0f, 43.0f
        };
        Vector2 starOrigin = { 17.0f, 21.5f };
        DrawTexturePro(gStars[i], starSource, starDestination,
                       starOrigin, 0.0f, WHITE);

        gPads[i].render();

        std::string padPointsText = "+" + std::to_string(gPadPoints[i]);
        DrawText(padPointsText.c_str(),
                 static_cast<int>(padPosition.x) - 25,
                 static_cast<int>(padPosition.y) - 116, 21, WHITE);
    }
    gHelicopter->render();

    DrawRectangle(314, 10, 350, 66, ColorFromHex("#293242"));
    std::string scoreText = "SCORE: " + std::to_string(gScore);
    std::string landingsText = "LANDINGS: " + std::to_string(gLandings) +
                               "/" + std::to_string(LANDINGS_TO_WIN);
    DrawText(scoreText.c_str(), 326, 16, 28, ColorFromHex("#FFF1A6"));
    DrawText(landingsText.c_str(), 326, 51, 18, WHITE);

    DrawRectangle(static_cast<int>(gRestartButton.x),
                  static_cast<int>(gRestartButton.y),
                  static_cast<int>(gRestartButton.width),
                  static_cast<int>(gRestartButton.height), ColorFromHex("#293242"));
    DrawRectangleLines(static_cast<int>(gRestartButton.x),
                       static_cast<int>(gRestartButton.y),
                       static_cast<int>(gRestartButton.width),
                       static_cast<int>(gRestartButton.height), WHITE);
    DrawText("RESTART  [R]", 1381, 29, 25, WHITE);

    DrawRectangle(0, SCREEN_HEIGHT - 26, SCREEN_WIDTH, 26,
                  ColorFromHex("#182232"));
    DrawText("A/D MOVE    W THRUST    R RESTART    Q QUIT    LAND SLOWLY IN A HALO",
             72, SCREEN_HEIGHT - 20, 17, WHITE);

    if (gLandingMessageTime > 0.0f && !gMissionComplete && !gMissionFailed)
    {
        std::string landingText = "+" + std::to_string(gLastLandingPoints) +
                                  " POINTS!  NEXT LANDING!";
        DrawRectangle(608, 91, 390, 44, ColorFromHex("#293242"));
        DrawText(landingText.c_str(), 626, 103, 22, GREEN);
    }

    if (gMissionComplete)
    {
        DrawRectangle(490, 356, 620, 152, ColorFromHex("#1D3040"));
        DrawText("MISSION ACCOMPLISHED!", 543, 377, 34, GREEN);
        std::string finalScore = "Final Score: " + std::to_string(gScore);
        DrawText(finalScore.c_str(), 695, 423, 24, WHITE);
        DrawText("Press R or click RESTART to play again.", 592, 467, 22, WHITE);
    }
    else if (gMissionFailed)
    {
        DrawRectangle(490, 356, 620, 152, ColorFromHex("#1D3040"));
        DrawText("MISSION FAILED!", 635, 377, 34, RED);
        std::string finalScore = "Score: " + std::to_string(gScore);
        DrawText(finalScore.c_str(), 734, 423, 24, WHITE);
        DrawText("Press R or click RESTART to try again.", 604, 467, 22, WHITE);
    }

    EndDrawing();
}

void shutdown()
{
    delete gHelicopter;
    delete[] gPads;

    UnloadTexture(gCityBuildings);
    for (int i = 0; i < NUMBER_OF_PADS; i++) UnloadTexture(gStars[i]);
    for (int i = 0; i < NUMBER_OF_BACKGROUNDS; i++)
        UnloadTexture(gBackgrounds[i]);

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
