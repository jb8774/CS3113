/**
* Author: Janee Bovell
* Assignment: Pong Clone
* Date due: [10/14/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600,
              SCREEN_HEIGHT = 900,
              FPS           = 60,
              SIZE          = 500 ,
              SPEED         = 400,
              BULLET_BALL_SPEED = 350,
              MAX_BULLET_BALLS = 3;


constexpr char    BG_COLOUR[]    = "#2f8cea"; // Going to change for day/night background cycle
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE      = { static_cast<float>(SIZE) / 4.0f , static_cast<float>(SIZE) / 4.0f },
                  BATTLE_SHIP_SIZE = { static_cast<float>(SIZE), static_cast<float>(2000)},
                  INIT_POS       = { ORIGIN.x - 200.0f, ORIGIN.y },
                  Right_Plyr_INIT_POS = { ORIGIN.x + 250.0f, ORIGIN.y };




constexpr char Left_Player_FP[]  = "../../SkyWars_Textur_Sound/kenney_simple-space/PNG/Left_Player.png";
constexpr char Right_Player_FP[] = "../../SkyWars_Textur_Sound/kenney_simple-space/PNG/Right_Player.png";
constexpr char Bullet_Ball[] = "../../SkyWars_Textur_Sound/kenney_simple-space/PNG/ship_B.png"; // Bullet Ball would be bounced back to each player like Pong
constexpr char Left_Ship[] = "../../SkyWars_Textur_Sound/kenney_simple-space/PNG/Left_Main.png"; // Ships are a health point to see which side destroys the otherside's ship
constexpr char Right_Shipp[] = "../../SkyWars_Textur_Sound/kenney_simple-space/PNG/Right_Main.png";

constexpr char Day_BG[] = "../../SkyWars_Textur_Sound/kenney_skyboxes/Skyboxes/skybox-day.png";
constexpr char Night_BG[] = "../../SkyWars_Textur_Sound/kenney_skyboxes/Skyboxes/skybox-night.png";



// Global Variables
AppStatus gAppStatus     = RUNNING;
bool      gSinglePlayerMode = false;
bool      gIsNight = false;
int       gBallCount     = 1;
int       gLeftScore     = 0,
          gRightScore    = 0;
int       gLeftShipLives = 3,
          gRightShipLives = 3;
float     gAngleRight         = 90.0f,
          gAngleLeft          = -90.0f,
          gCPUdirection       = 1.0f,
          gPreviousTicks = 0.0f,
          gBackgroundTimer = 0.0f;

Vector2 gLeftPlayerPosition  = INIT_POS,
        gLeftPlayerMovement  = { 0.0f, 0.0f },
        gLeftPlayerScale    = BASE_SIZE,

        gRightPlayerPosition = Right_Plyr_INIT_POS,
        gRightPlayerMovement = { 0.0f, 0.0f },
        gRightPlayerScale    = BASE_SIZE,

        gMousePosition = GetMousePosition();

Vector2 gBulletBallPosition = { ORIGIN.x, ORIGIN.y },
        gBulletBallScale    = { 50.0f, 50.0f },
        gBulletBallMovement = { 1.0f, 1.0f },
        gBallPositions[MAX_BULLET_BALLS] = { 
            { 800.0f, 450.0f }, 
            { 800.0f, 350.0f },
            { 800.0f, 550.0f }
        },
        gBallMovements[MAX_BULLET_BALLS] = { 
            { 1.0f, 0.5f }, 
            { -1.0f, -0.5f },
            { 1.0f, -0.5f }
        },

        gLeftShipPosition  = { 250.0f, ORIGIN.y },
        gLeftShipScale     = BATTLE_SHIP_SIZE,

        gRightShipPosition = { 1350.0f, ORIGIN.y },
        gRightShipScale    = BATTLE_SHIP_SIZE;

Texture2D gLeftPlayerTexture;
Texture2D gRightPlayerTexture;
Texture2D gBulletBallTexture;
Texture2D gLeftShipTexture;
Texture2D gRightShipTexture;
Texture2D gDayTexture;
Texture2D gNightTexture;
Music gBGM; // Background music for the game
Sound gLaserSound; // Sound effect for the rupee


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);// TODO: implement AABB collision detection
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y)/ 2.0f);
    
    return xDistance < 0.0f && yDistance < 0.0f;
}

void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale, float angle)


{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    DrawTexturePro(
    *texture,
    textureArea, destinationArea, originOffset,
    angle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sky Wars");
    InitAudioDevice();

    gBGM = LoadMusicStream("../../SkyWars_Textur_Sound/kenney_sci-fi-sounds/Audio/bgm.mp3"); // Load background music file
    SetMusicVolume(gBGM, 0.33f); // Set background music volume to 50%
    PlayMusicStream(gBGM); // Start playing the background music

    if (isColliding(&gLeftPlayerPosition, &gLeftPlayerScale, &gRightPlayerPosition, &gRightPlayerScale))
    {
        if (!IsSoundPlaying(gLaserSound)) PlaySound(gLaserSound); // Play the rupee sound effect when collision occurs
    }


    gLaserSound = LoadSound("../../SkyWars_Textur_Sound/kenney_sci-fi-sounds/Audio/laserSmall_004.ogg"); // Load rupee sound effect

    gLeftPlayerTexture  = LoadTexture(Left_Player_FP);
    gRightPlayerTexture = LoadTexture(Right_Player_FP);
    gBulletBallTexture = LoadTexture(Bullet_Ball);
    gLeftShipTexture   = LoadTexture(Left_Ship);
    gRightShipTexture  = LoadTexture(Right_Shipp);
    gDayTexture = LoadTexture(Day_BG);
    gNightTexture = LoadTexture(Night_BG);
    SetTargetFPS(FPS);
}

void processInput()
{
    gLeftPlayerMovement = { 0.0f, 0.0f };
    gRightPlayerMovement = { 0.0f, 0.0f };

    if (IsKeyPressed(KEY_T)) gSinglePlayerMode = !gSinglePlayerMode;


    // TODO: handle WASD keyboard input

    if(IsKeyDown(KEY_D)) gLeftPlayerMovement.x = 1.0f; // handles right movement
    else if (IsKeyDown(KEY_A)) gLeftPlayerMovement.x = -1.0f;  // handles left movement // TODO: handle left mouse button input
    if(IsKeyDown(KEY_W)) gLeftPlayerMovement.y = -1.0f; // handles up movement
    else if (IsKeyDown(KEY_S)) gLeftPlayerMovement.y = 1.0f;  // handles down movement

    if (GetLength(&gLeftPlayerMovement) > 1.0f) Normalise(&gLeftPlayerMovement); // TODO: normalise diagonal movement


    if (!gSinglePlayerMode)
    {
    if(IsKeyDown(KEY_RIGHT)) gRightPlayerMovement.x = 1.0f; // handles right movement
    else if (IsKeyDown(KEY_LEFT)) gRightPlayerMovement.x = -1.0f;  // handles left movement // TODO: handle left mouse button input
    if(IsKeyDown(KEY_UP)) gRightPlayerMovement.y = -1.0f; // handles up movement
    else if (IsKeyDown(KEY_DOWN)) gRightPlayerMovement.y = 1.0f;  // handles down movement

    if (GetLength(&gRightPlayerMovement) > 1.0f) Normalise(&gRightPlayerMovement); // TODO: normalise diagonal movement
    }
    //if (IsKeyPressed(KEY_A)) printf("A key is being pressed\n");
    if (IsKeyPressed(KEY_ONE)) gBallCount = 1;
    if (IsKeyPressed(KEY_TWO)) gBallCount = 2;
    if (IsKeyPressed(KEY_THREE)) gBallCount = 3;

    for (int i = 0; i < gBallCount; ++i)
    {
        if (isColliding(&gLeftPlayerPosition, &gLeftPlayerScale, &gBallPositions[i], &gBulletBallScale))
        {
            if (!IsSoundPlaying(gLaserSound)) PlaySound(gLaserSound); // Play the rupee sound effect when collision occurs
        }
        if (isColliding(&gRightPlayerPosition, &gRightPlayerScale, &gBallPositions[i], &gBulletBallScale))
        {
            if (!IsSoundPlaying(gLaserSound)) PlaySound(gLaserSound); // Play the rupee sound effect when collision occurs
        }
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

}

void update()
{
    float ticks    = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    gBackgroundTimer += deltaTime;

    if (gBackgroundTimer >= 10.0f)
    {
        gIsNight = !gIsNight;
        gBackgroundTimer = 0.0f;
    }
    gMousePosition = GetMousePosition();
    

    if (gSinglePlayerMode){

    if (gLeftPlayerPosition.y > gRightPlayerPosition.y + 10.0f)
    {
        gRightPlayerMovement.y = 1.0f;
    }
    else if (gLeftPlayerPosition.y < gRightPlayerPosition.y - 10.0f)
    {
        gRightPlayerMovement.y = -1.0f;
    }
    else
    {
        gRightPlayerMovement.y = 0.0f;
    }
    }       

    if (gLeftShipPosition.x > 0.0f)
    {
    gLeftShipPosition.x -= 100.0f * deltaTime;

    if (gLeftShipPosition.x < 0.0f)
        gLeftShipPosition.x = 0.0f;
    }

    if (gRightShipPosition.x < SCREEN_WIDTH)
    {
    gRightShipPosition.x += 100.0f * deltaTime;

    if (gRightShipPosition.x > SCREEN_WIDTH)
        gRightShipPosition.x = static_cast<float>(SCREEN_WIDTH);
    }

    // TODO: apply movement to gLinkPosition using deltaTime

    gLeftPlayerPosition = {
        gLeftPlayerPosition.x + SPEED * gLeftPlayerMovement.x * deltaTime,
        gLeftPlayerPosition.y + SPEED * gLeftPlayerMovement.y * deltaTime
    };

    gRightPlayerPosition = {
    gRightPlayerPosition.x + SPEED * gRightPlayerMovement.x * deltaTime,
    gRightPlayerPosition.y + SPEED * gRightPlayerMovement.y * deltaTime
    };  

    if (gLeftPlayerPosition.x < gLeftShipPosition.x + gLeftShipScale.x / 2.0f + gLeftPlayerScale.x / 2.0f)
        gLeftPlayerPosition.x = gLeftShipPosition.x + gLeftShipScale.x / 2.0f + gLeftPlayerScale.x / 2.0f;
    if (gLeftPlayerPosition.x > SCREEN_WIDTH / 2.0f - gLeftPlayerScale.x / 2.0f)
        gLeftPlayerPosition.x = SCREEN_WIDTH / 2.0f - gLeftPlayerScale.x / 2.0f;
    if (gLeftPlayerPosition.y < gLeftPlayerScale.y / 2.0f)
        gLeftPlayerPosition.y = gLeftPlayerScale.y / 2.0f;
    if (gLeftPlayerPosition.y > SCREEN_HEIGHT - gLeftPlayerScale.y / 2.0f)
        gLeftPlayerPosition.y = SCREEN_HEIGHT - gLeftPlayerScale.y / 2.0f;

    if (gRightPlayerPosition.x < SCREEN_WIDTH / 2.0f + gRightPlayerScale.x / 2.0f)
    {
        gRightPlayerPosition.x = SCREEN_WIDTH / 2.0f + gRightPlayerScale.x / 2.0f;
    }

    if (gRightPlayerPosition.x > gRightShipPosition.x - gRightShipScale.x / 2.0f - gRightPlayerScale.x / 2.0f)
    {
        gRightPlayerPosition.x = gRightShipPosition.x - gRightShipScale.x / 2.0f - gRightPlayerScale.x / 2.0f;
    }   
    if (gRightPlayerPosition.y < gRightPlayerScale.y / 2.0f)
        gRightPlayerPosition.y = gRightPlayerScale.y / 2.0f;
    if (gRightPlayerPosition.y > SCREEN_HEIGHT - gRightPlayerScale.y / 2.0f)
        gRightPlayerPosition.y = SCREEN_HEIGHT - gRightPlayerScale.y / 2.0f;

    for (int i = 0; i < gBallCount; ++i)
    {
        gBallPositions[i].x += BULLET_BALL_SPEED * gBallMovements[i].x * deltaTime;
        gBallPositions[i].y += BULLET_BALL_SPEED * gBallMovements[i].y * deltaTime;

   
        if (gBallPositions[i].y <= gBulletBallScale.y / 2.0f)
        {
            gBallPositions[i].y = gBulletBallScale.y / 2.0f;
            gBallMovements[i].y *= -1.0f;
        }

        if (gBallPositions[i].y >= SCREEN_HEIGHT - gBulletBallScale.y / 2.0f)
        {
            gBallPositions[i].y = SCREEN_HEIGHT - gBulletBallScale.y / 2.0f;
            gBallMovements[i].y *= -1.0f;
        }

        if (gBallMovements[i].x < 0.0f &&
            isColliding(&gBallPositions[i], &gBulletBallScale,
                        &gLeftPlayerPosition, &gLeftPlayerScale))
        {
            gBallPositions[i].x = gLeftPlayerPosition.x
                                + gLeftPlayerScale.x / 2.0f
                                + gBulletBallScale.x / 2.0f;

            gBallMovements[i].x *= -1.0f;
        }

        if (gBallMovements[i].x > 0.0f &&
            isColliding(&gBallPositions[i], &gBulletBallScale,
                        &gRightPlayerPosition, &gRightPlayerScale))
        {
            gBallPositions[i].x = gRightPlayerPosition.x
                                - gRightPlayerScale.x / 2.0f
                                - gBulletBallScale.x / 2.0f;

            gBallMovements[i].x *= -1.0f;
        }

        if (gBallPositions[i].x <=
            gLeftShipPosition.x + gLeftShipScale.x / 2.0f)
        {
            gLeftShipLives--;

            if (gLeftShipLives == 0)
            {
                gRightScore++;
                gLeftShipLives = 3;
            }
            gBallPositions[i] = ORIGIN;
            gBallMovements[i] = { 1.0f, 0.5f };
        }

        if (gBallPositions[i].x >=
            gRightShipPosition.x - gRightShipScale.x / 2.0f)
        {
            gRightShipLives--;

            if (gRightShipLives == 0)
            {
                gLeftScore++;
                gRightShipLives = 3;
            }

            gBallPositions[i] = ORIGIN;
            gBallMovements[i] = { -1.0f, -0.5f };
        }
}
        

    // TODO: check for collision between Link and the rupee

    UpdateMusicStream(gBGM); // Update the background music stream
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    Texture2D background = gIsNight ? gNightTexture : gDayTexture;
    DrawTexturePro(
        background,
        { 0.0f, 0.0f,
        static_cast<float>(background.width),
        static_cast<float>(background.height) },

        { 0.0f, 0.0f,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT) },

        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    renderObject(&gLeftPlayerTexture,  &gLeftPlayerPosition,  &gLeftPlayerScale, gAngleRight);
    renderObject(&gRightPlayerTexture, &gRightPlayerPosition, &gRightPlayerScale, gAngleLeft);
    for (int i = 0; i < gBallCount; ++i)
    {
        renderObject(&gBulletBallTexture, &gBallPositions[i], &gBulletBallScale, 0.0f);    }

    renderObject(&gLeftShipTexture, &gLeftShipPosition, &gLeftShipScale, 0.0f);
    renderObject(&gRightShipTexture, &gRightShipPosition, &gRightShipScale, 0.0f);

    DrawText(TextFormat("Left Player Score: %d", gLeftScore), 20, 20, 20, WHITE);
    DrawText(TextFormat("Right Player Score: %d", gRightScore), SCREEN_WIDTH - 250, 20, 20, WHITE);
    DrawText(TextFormat("Left Ship Lives: %d", gLeftShipLives), 20, 50, 20, WHITE);
    DrawText(TextFormat("Right Ship Lives: %d", gRightShipLives), SCREEN_WIDTH - 250, 50, 20, WHITE);

    if (gSinglePlayerMode){
         DrawText(TextFormat("SINGLE PLAYER MODE"), 650, 20, 30,WHITE);
    } else {
         DrawText(TextFormat("TWO PLAYER MODE"), 650, 20, 30,WHITE);
    }


    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gLeftPlayerTexture);
    UnloadTexture(gRightPlayerTexture);
    UnloadTexture(gBulletBallTexture);
    UnloadTexture(gDayTexture);
    UnloadTexture(gNightTexture);
    UnloadTexture(gLeftShipTexture);
    UnloadTexture(gRightShipTexture);
    UnloadMusicStream(gBGM); // Unload the background music stream
    UnloadSound(gLaserSound); // Unload the rupee sound effect
    CloseAudioDevice(); /// Closes Audio when the program shuts down
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