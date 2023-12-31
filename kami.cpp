#include "raylib.h"

struct LevelData
{
    Texture2D texture;
    Vector2 position;
    Vector2 followingPosition;
    Rectangle rectangle;
    float scale;
    float xLocation;
};
struct AnimationData
{
    bool invertedState;
    Rectangle rectangle;
    Vector2 position;
    int frame;
    float updateTime;
    float runningTime;
    float positionYOffset;
};

// Refactored Methods/Functions
bool isOnGround(AnimationData data, int windowHeight, float positionOffset)
{
    return data.position.y >= windowHeight - (data.rectangle.height + positionOffset);
}

float setLevelTextureSpeed(float scrollingXLocation, float scrollingSpeed, float deltaTime, int layerLocation)
{
    scrollingXLocation -= (scrollingSpeed * layerLocation) * deltaTime;
    return scrollingXLocation;
}

LevelData updateLevelData(LevelData data, float deltaTime)
{
    // Reset Background Texture Positions
    if (data.xLocation <= -data.rectangle.width * data.scale)
    {
        data.xLocation = 0.0;
    }

    data.position.x = data.xLocation;
    data.followingPosition.x = data.xLocation + data.texture.width * data.scale;
    return data;
}

AnimationData updateAnimationData(AnimationData data, float deltaTime, int endFrame, bool spriteInverted)
{
    // Update Running Time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        // Update Animation Frame
        if (spriteInverted)
        {
            data.rectangle.x = data.frame * data.rectangle.width;
            data.frame++;
            if (data.frame > 0)
            {
                data.frame = -endFrame;
            }
            data.runningTime = 0;
        }
        else
        {
            data.rectangle.x = data.frame * data.rectangle.width;
            data.frame++;
            if (data.frame > endFrame)
            {
                data.frame = 0;
            }
            data.runningTime = 0;
        }
    }
    return data;
}

int main()
{
    // Window Properties
    int windowDimensions[2];
    windowDimensions[0] = 1280;
    windowDimensions[1] = 720;

    // Initialize Game Window
    InitWindow(windowDimensions[0], windowDimensions[1], "Kami Dash");
    SetTargetFPS(60);


    // Set Level Background Data
    const int amountOfBGTextures = 10;
    float backgroundScollingSpeed = 10.0;
    LevelData backgroundData[amountOfBGTextures]{};

    backgroundData[0].texture = LoadTexture("textures/dark_pixel_forest/Layer_0011_0.png");
    backgroundData[1].texture = LoadTexture("textures/dark_pixel_forest/Layer_0010_1.png");
    backgroundData[2].texture = LoadTexture("textures/dark_pixel_forest/Layer_0009_2.png");
    backgroundData[3].texture = LoadTexture("textures/dark_pixel_forest/Layer_0008_3.png");
    backgroundData[4].texture = LoadTexture("textures/dark_pixel_forest/Layer_0007_Lights.png");
    backgroundData[5].texture = LoadTexture("textures/dark_pixel_forest/Layer_0006_4.png");
    backgroundData[6].texture = LoadTexture("textures/dark_pixel_forest/Layer_0005_5.png");
    backgroundData[7].texture = LoadTexture("textures/dark_pixel_forest/Layer_0004_Lights.png");
    backgroundData[8].texture = LoadTexture("textures/dark_pixel_forest/Layer_0003_6.png");
    backgroundData[9].texture = LoadTexture("textures/dark_pixel_forest/Layer_0001_8.png");

    for (int i = 0; i < amountOfBGTextures; i++)
    {
        backgroundData[i].position.x = 0.0;
        backgroundData[i].position.y = -backgroundData[i].texture.height / 2 - 60.0;
        backgroundData[i].followingPosition.x = backgroundData[i].position.x + backgroundData[i].texture.width;
        backgroundData[i].followingPosition.y = -backgroundData[i].texture.height / 2 - 60.0;
        backgroundData[i].rectangle.x = 0.0;
        backgroundData[i].rectangle.y = 0.0;
        backgroundData[i].rectangle.height = backgroundData[i].texture.height;
        backgroundData[i].rectangle.width = backgroundData[i].texture.width;
        backgroundData[i].scale = 1.5;
        backgroundData[i].xLocation = 0.0;
    }

    // Set Level Foreground Data
    const int amountOfFGTextures = 2;
    float foregroundScrollingSpeed = 100.0;
    LevelData foregroundData[amountOfFGTextures]{};

    foregroundData[0].texture = LoadTexture("textures/dark_pixel_forest/Layer_0002_7.png");
    foregroundData[1].texture = LoadTexture("textures/dark_pixel_forest/Layer_0000_9.png");
    
    for (int i = 0; i < amountOfFGTextures; i++)
    {
        foregroundData[i].position.x = 0.0;
        foregroundData[i].position.y = -foregroundData[i].texture.height / 2 - 60.0;
        foregroundData[i].followingPosition.x = foregroundData[i].position.x + foregroundData[i].texture.width;
        foregroundData[i].followingPosition.y = -foregroundData[i].texture.height / 2 - 60.0;
        foregroundData[i].rectangle.x = 0.0;
        foregroundData[i].rectangle.y = 0.0;
        foregroundData[i].rectangle.height = foregroundData[i].texture.height;
        foregroundData[i].rectangle.width = foregroundData[i].texture.width;
        foregroundData[i].scale = 1.5;
        foregroundData[i].xLocation = 0.0;
    }

    // Set Character "Kami" Data
    Texture2D kami = LoadTexture("textures/running_knight_girl.png");
    AnimationData kamiData;
    int powerUpsCollected = 0;
    bool enemyCollision = false;
    bool powerUpCollision = false;

    kamiData.rectangle.width = kami.width/7;
    kamiData.rectangle.height = kami.height;
    kamiData.rectangle.x = 0.0;
    kamiData.rectangle.y = 0.0;
    kamiData.positionYOffset = 70.0;
    kamiData.position.x = windowDimensions[0]/2 - kami.width/1.5;
    kamiData.position.y = windowDimensions[1] + kami.height;
    kamiData.updateTime = 1.0/12.0;
    kamiData.runningTime = 0;
    kamiData.frame = 0;
 
    // Set PowerUp Crystals Data
    Texture2D powerCrystal = LoadTexture("textures/power_ups/crystals/blue/blue_crystal_sprites_sheet.png");

    const int amountOfCrystals = 6;
    AnimationData powerCrystals[amountOfCrystals]{};

    for (int i = 0; i < amountOfCrystals; i++)
    {
        powerCrystals[i].rectangle.width = powerCrystal.width/4;
        powerCrystals[i].rectangle.height = powerCrystal.height;
        powerCrystals[i].rectangle.x = 0.0;
        powerCrystals[i].rectangle.y = 0.0;
        powerCrystals[i].positionYOffset = 75.0;
        powerCrystals[i].position.x = windowDimensions[0] + (i * 300);
        powerCrystals[i].position.y = windowDimensions[1] - (powerCrystal.height + powerCrystals[i].positionYOffset);
        powerCrystals[i].updateTime = 1.0/4.0;
        powerCrystals[i].runningTime = 0.0;
        powerCrystals[i].frame = 0;
    }

    int powerCrystalVelocity = -200;

    // Set Blight Enemy Data
    Texture2D blightEnemy = LoadTexture("textures/necromancer_character_sheet.png");

    const int amountOfEnemies = 6;
    AnimationData blightEnemies[amountOfEnemies]{};

    for (int i = 0; i < amountOfEnemies; i++)
    {
        blightEnemies[i].invertedState = true;
        blightEnemies[i].rectangle.width = -blightEnemy.width/17; 
        blightEnemies[i].rectangle.height = blightEnemy.height/7;
        blightEnemies[i].rectangle.x = 0.0;
        blightEnemies[i].rectangle.y = blightEnemy.height/7;
        blightEnemies[i].positionYOffset = 75.0;
        blightEnemies[i].position.x = windowDimensions[0] + (i * 1000);
        blightEnemies[i].position.y = windowDimensions[1] - (blightEnemy.height/7 + blightEnemies[i].positionYOffset);
        blightEnemies[i].updateTime = 1.0/7.0;
        blightEnemies[i].runningTime = 0.0;
        blightEnemies[i].frame = -7;
    }

    int blightEnemyVelocity = -200;

    // Set Gravity Properties (pixels/s)
    const int gravity = 1600;
    const int jumpVelocity = -800;
    int velocity = 0;
    bool inTheAir = false;

    // The Main Game
    while (!WindowShouldClose())
    {
        // Delta Time
        const float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(WHITE);

        // Detection Parameters for Power Crystals in relation to Kami
        for (AnimationData crystal : powerCrystals)
        {
            float padding = 20.0;
            Rectangle crystalRectangle
            {
                crystal.position.x + padding,
                crystal.position.y + padding,
                crystal.rectangle.width - 2 * padding,
                crystal.rectangle.height - 2 * padding
            };
            Rectangle kamiRectangle
            {
                kamiData.position.x,
                kamiData.position.y,
                kamiData.rectangle.width,
                kamiData.rectangle.height
            };
            if (CheckCollisionRecs(crystalRectangle, kamiRectangle))
            {
                powerUpCollision = true;
            }
            
        }

        // Detection Parameters for Blight Enemies in relation to Kami
        for (AnimationData blight : blightEnemies)
        {
            float padding = 60.0;
            Rectangle blightRectangle
            {
                blight.position.x + padding,
                blight.position.y + padding,
                -blight.rectangle.width + padding,
                blight.rectangle.height - padding
            };
            Rectangle kamiRectangle
            {
                kamiData.position.x,
                kamiData.position.y,
                kamiData.rectangle.width,
                kamiData.rectangle.height
            };

            if (CheckCollisionRecs(blightRectangle, kamiRectangle))
            {
                enemyCollision = true;
            }
            
        }

        // Update Background Data
        for (int i = 0; i < amountOfBGTextures; i++)
        {
            // Render the Background
            DrawTextureEx(backgroundData[i].texture, backgroundData[i].position, 0.0, backgroundData[i].scale, WHITE);
            DrawTextureEx(backgroundData[i].texture, backgroundData[i].followingPosition, 0.0, backgroundData[i].scale, WHITE);

            // Update the Background Texture Speed
            backgroundData[i].xLocation = setLevelTextureSpeed(backgroundData[i].xLocation, backgroundScollingSpeed, deltaTime, i);

            // Update Background Positions
            backgroundData[i] = updateLevelData(backgroundData[i], deltaTime);
        }

        // Game State depending on Player's Collision Status
        if (enemyCollision)
        {
            // Render Game Over Sequence
            DrawText("Game Over!", windowDimensions[0]/4 + 125, windowDimensions[1]/2 - 60, 70, WHITE);
            DrawText("Play Again?", windowDimensions[0]/4 + 220, windowDimensions[1]/2 + 60, 36, WHITE);
            DrawText("Press: Yes[Y] / No[ESC]", windowDimensions[0]/4 + 170, windowDimensions[1]/2 + 120, 24, WHITE);

            // Restart the core gameplay for replay
            if (IsKeyPressed(KEY_Y))
            {
                enemyCollision = false;
                kamiData.position.x = windowDimensions[0]/2 - kami.width/1.5;
                for (int i = 0; i < amountOfCrystals; i++)
                {
                    powerCrystals[i].position.x = windowDimensions[0] + (i * 300);
                }
                for (int i = 0; i < amountOfEnemies; i++)
                {
                    blightEnemies[i].position.x = windowDimensions[0] + (i * 1000);
                }
            }
        }
        else
        {   

            // Render Kami
            DrawTextureRec(kami, kamiData.rectangle, kamiData.position, WHITE);
            
            // Update Kami's Position
            kamiData.position.y += velocity * deltaTime;

            // Render & Update Power Crystal Values
            for (int i = 0; i < amountOfCrystals; i++)
            {
                // Render the Power Crystals
                DrawTextureRec(powerCrystal, powerCrystals[i].rectangle, powerCrystals[i].position, WHITE);

                // Update the Power Crystals Position
                powerCrystals[i].position.x += powerCrystalVelocity * deltaTime;

                // Update the Power Crystals Animation Data
                powerCrystals[i] = updateAnimationData(powerCrystals[i], deltaTime, 3, powerCrystals[i].invertedState);
            }

            // Render & Update Blight Enemy Values
            for (int i = 0; i < amountOfEnemies; i++)
            {
                // Render the Blight Enemies
                DrawTextureRec(blightEnemy, blightEnemies[i].rectangle, blightEnemies[i].position, WHITE);

                // Update the Blight Enemies Position
                blightEnemies[i].position.x += blightEnemyVelocity * deltaTime;

                // Update the Blight Enemies Animation Data
                blightEnemies[i] = updateAnimationData(blightEnemies[i], deltaTime, 7, blightEnemies[i].invertedState);
            }

            // Ground Check
            if (isOnGround(kamiData, windowDimensions[1], kamiData.positionYOffset))
            {
                // Player is on the Ground
                velocity = 0;
                kamiData.position.y = windowDimensions[1] - (kamiData.rectangle.height + kamiData.positionYOffset);
                inTheAir = false;

                // Update Kami's Animation Frame
                kamiData = updateAnimationData(kamiData, deltaTime, 6, kamiData.invertedState);
            }
            else
            {
                // Player is in the Air, apply Gravity
                velocity += gravity * deltaTime;
                inTheAir = true;
            }

            // Player Jumps at Velocity
            if(IsKeyPressed(KEY_SPACE) && !inTheAir)
            {
                velocity += jumpVelocity;
            }            
        }
        // Render & Update Foreground Data
        for (int i = 0; i < amountOfFGTextures; i++)
        {
            // Render the Foreground
            DrawTextureEx(foregroundData[i].texture, foregroundData[i].position, 0.0, foregroundData[i].scale, WHITE);
            DrawTextureEx(foregroundData[i].texture, foregroundData[i].followingPosition, 0.0, foregroundData[i].scale, WHITE);

            // Update the Foreground Texture Speed
            foregroundData[i].xLocation = setLevelTextureSpeed(foregroundData[i].xLocation, foregroundScrollingSpeed, deltaTime, i);

            // Update Foreground Positions
            foregroundData[i] = updateLevelData(foregroundData[i], deltaTime);
        }
        EndDrawing();
    }

    UnloadTexture(kami);
    UnloadTexture(powerCrystal);
    UnloadTexture(blightEnemy);
    for (int i = 0; i < amountOfBGTextures; i++)
    {
        UnloadTexture(backgroundData[i].texture);
    }  
    for (int i = 0; i < amountOfFGTextures; i++)
    {
        UnloadTexture(foregroundData[i].texture);
    }  
    CloseWindow();
}