#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 2,
              SCREEN_HEIGHT = 900 / 2,
              FPS           = 60,
              SIZE          = 500 / 2,
              SPEED         = 200; 

constexpr char    BG_COLOUR[]    = "#F8F1C8";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE      = { static_cast<float>(SIZE), static_cast<float>(SIZE) },
                  INIT_POS       = { ORIGIN.x - 200.0f, ORIGIN.y },
                  RUPEE_INIT_POS = { ORIGIN.x + 250.0f, ORIGIN.y };

// Images owned by Nintendo — please don't sue me.
constexpr char LINK_FP[]  = "assets/game/awakening-link.png";
constexpr char RUPEE_FP[] = "assets/game/awakening-rupee.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gLinkPosition  = INIT_POS,
        gLinkMovement  = { 0.0f, 0.0f },
        gLinkScale     = BASE_SIZE,

        gRupeePosition = RUPEE_INIT_POS,
        gRupeeMovement = { 0.0f, 0.0f },
        gRupeeScale    = { BASE_SIZE.x / 2.0f, BASE_SIZE.y / 2.0f * 1.6f },

        gMousePosition = GetMousePosition();

Texture2D gLinkTexture;
Texture2D gRupeeTexture;



Music gBGM; // Background music for the game
Sound gRupeeSound; // Sound effect for the rupee


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
                  const Vector2 *scale)
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
        gAngle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");
    InitAudioDevice();

    gBGM = LoadMusicStream("assets/game/awakening-bg.mp3"); // Load background music file
    SetMusicVolume(gBGM, 0.33f); // Set background music volume to 50%
    PlayMusicStream(gBGM); // Start playing the background music

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){ // Character follows the mouse cursor
        
        if(gLinkPosition.x < gMousePosition.x) gLinkMovement.x = 1.0f;
        if(gLinkPosition.x > gMousePosition.x) gLinkMovement.x = -1.0f;
        if(gLinkPosition.y < gMousePosition.y) gLinkMovement.y = 1.0f;
        if(gLinkPosition.y > gMousePosition.y) gLinkMovement.y = -1.0f;
    }

    if (isColliding(&gLinkPosition, &gLinkScale, &gRupeePosition, &gRupeeScale))
    {
        if (!IsSoundPlaying(gRupeeSound)) PlaySound(gRupeeSound); // Play the rupee sound effect when collision occurs
    }


    gRupeeSound = LoadSound("assets/game/rupee.mp3"); // Load rupee sound effect

    gLinkTexture  = LoadTexture(LINK_FP);
    gRupeeTexture = LoadTexture(RUPEE_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    gLinkMovement = { 0.0f, 0.0f };

    // TODO: handle WASD keyboard input

    if(IsKeyDown(KEY_D)) gLinkMovement.x = 1.0f; // handles right movement
    else if (IsKeyDown(KEY_A)) gLinkMovement.x = -1.0f;  // handles left movement // TODO: handle left mouse button input
    if(IsKeyDown(KEY_W)) gLinkMovement.y = -1.0f; // handles up movement
    else if (IsKeyDown(KEY_S)) gLinkMovement.y = 1.0f;  // handles down movement

    if (GetLength(&gLinkMovement) > 1.0f) Normalise(&gLinkMovement); // TODO: normalise diagonal movement

    //if (IsKeyPressed(KEY_A)) printf("A key is being pressed\n");

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks    = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gMousePosition = GetMousePosition();

    // TODO: apply movement to gLinkPosition using deltaTime

    gLinkPosition = {
        gLinkPosition.x + SPEED * gLinkMovement.x * deltaTime,
        gLinkPosition.y + SPEED * gLinkMovement.y * deltaTime
    };

    // TODO: check for collision between Link and the rupee

    UpdateMusicStream(gBGM); // Update the background music stream
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    renderObject(&gLinkTexture,  &gLinkPosition,  &gLinkScale);
    renderObject(&gRupeeTexture, &gRupeePosition, &gRupeeScale);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gLinkTexture);
    UnloadTexture(gRupeeTexture);
    UnloadMusicStream(gBGM); // Unload the background music stream
    UnloadSound(gRupeeSound); // Unload the rupee sound effect
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
