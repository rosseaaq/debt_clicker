#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <string>
using std::string;


typedef enum Screen
{
    title_screen,
    cutscene,
    movement,
    game_screen,
    screen2,
    screen3,
    final_s,
    last1,
    last2,
    last3
}
Screen;

typedef enum Direction
{
    right,
    left
} Direction;

typedef enum playerState
{
    idle,
    walk
} playerState;

Direction playerDirection = right;
playerState state = idle;

Screen currentScreen = title_screen;

const int screenWidth = 1280;
const int screenHeight = 720;
const int cutWidth = 3200;
const char* title = "DEBT CLICKER";
int good = 0;
int bad = 0;

string dialogue[] = {
    "You: Uh, Hi, I was wondering if you could help me out? I need to make some money fast.",
    "???: Yes of course, i know you heard alot about us, we can help you out, but it will cost you.",
    "You: Cost me what exactly?",
    "???: Your time. Interact with the stall and you will get money, but be careful, the more you interact with it, the more it will take out of you.",
    "You: What do you mean by that? What will it take out of me?",
    "???: You'll see soon enough, but for now, begin to click.",
    "???: Oh, you got your first item! A feather may be ticklish, but money made 5 times more than that is quite nice, isn't it?",
    "???: Oh, you got your second item! A needle may be sharp, eh ill bare it, now you'll have +$1200, not bad at all.",
    "???: Third item! A hammer, not the safest item to have, but it will make you 100 times more than that, a good deal if you ask me.",
    "???: Things are getting.... intense, you got your fourth item, a knife, dont you think that could be dangerous? But hey, it makes you 500 times more than that, a steal if you ask me.",
    "???: Okay stop, stop, you've made literally half of what you need, but you look like you're in pain, you should stop for now, maybe come back later.",
    "You: No, I need to keep going, I need to save my mother, you can take it right?",
    "???: Oh, I can take it, but I dont want to, you have to give it to me willingly, and you know what happens if you dont give it to me willingly, right?",
    "???: Oh well, you got your last item...",
    "???: A GUN?! ARE YOU SERIOUS?",
    "???: Oh, you want to use it on me? You dont have to do this..",
    "You: I have to, I need to save my mother, and if this is what it takes, then so be it.",
    "???: I know, it'll clear off your debt, but i have a family too you know.",
    "You: I hope this is worth it."
};

int dialogueIndex = 0;

Rectangle player = { 100, 500, 40, 60 };
Rectangle stallZone = { 390,245,330,374.5 }; 

Rectangle neutral = { 0,   0, 253, 339 };
Rectangle smile = { 256, 0, 253, 339 };
Rectangle laugh = { 510, 0, 253, 339 };
Rectangle sad = { 766,0, 253, 339 };

Rectangle worried = { 0,   355, 253, 360 };
Rectangle scared1 = { 356, 343, 300, 350 };
Rectangle scream = { 735, 343, 253, 350 };

Rectangle hurt = { 0,   705, 253, 350 };
Rectangle cry1 = { 359, 697, 253, 350 };
Rectangle cry2 = { 734, 697, 253, 350 };

Vector2 newspaperPos = { 2535, 560};

int money = 0;

float shakeTime = 0.0f;
float shakeDuration = 0.15f; // how long it shakes
float shakeStrength = 5.0f;  // how strong the shake is

int main(void)
{   
    Camera2D camera = { 0 };
    InitWindow(screenWidth, screenHeight, "Debt Clicker");   
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    bool usingStall = false;
	bool readNews = false;
    bool Choice[8] = { false,false,false,false,false,false,false,false };

    Texture2D frames[32]; //an array of frames
    Texture2D cutsceneFrames[14]; //an array of frames for the cutscene
	Texture2D walkLeft[6]; //an array of frames for the walk left animation
	Texture2D walkRight[6]; //an array of frames for the walk right animation
	Texture2D leftIdle[4]; //an array of frames for the left idle animation
	Texture2D rightIdle[4]; //an array of frames for the right idle animation
	Texture2D stall = LoadTexture("assets/stall/unnamed.png");
	Texture2D background = LoadTexture("assets/bg/background.png");
	Texture2D bg1 = LoadTexture("assets/gamescreen/1.png");
	Texture2D bg2 = LoadTexture("assets/gamescreen/2.png");
	Texture2D emotion = LoadTexture("assets/emotions.png");

	Texture2D banner = LoadTexture("assets/banner.png");

    for (int i = 0; i < 32; i++)
    {
		frames[i] = LoadTexture(TextFormat("assets/gif/frame_%04d.png", i + 1)); //loop is created in order to load all the frames of the animation into an array of textures
    }

    for (int k = 0; k < 6; k++)
    {
        walkLeft[k] = LoadTexture(TextFormat("assets/walk left/frame_%04d.png", k + 1)); //loop is created in order to load all the frames of the animation into an array of textures
		walkRight[k] = LoadTexture(TextFormat("assets/walk right/frame_%04d.png", k + 1)); //loop is created in order to load all the frames of the animation into an array of textures
    }

    for( int k = 0; k < 4; k++)
    {
        leftIdle[k] = LoadTexture(TextFormat("assets/left idle/frame_%04d.png", k + 1)); //loop is created in order to load all the frames of the animation into an array of textures
        rightIdle[k] = LoadTexture(TextFormat("assets/right idle/frame_%04d.png", k + 1)); //loop is created in order to load all the frames of the animation into an array of textures
    }

    for (int j = 0; j < 14; j++)
    {
        cutsceneFrames[j] = LoadTexture(TextFormat("assets/cutscene/frame_%04d.png", j + 1)); 
    }

         
    int currentFrame[2] = { 0,0 }; //variable to keep track of the current frame of the animation
	float timer[3] = { 0,0,0 }; //variable to keep track of the time elapsed since the last frame change

	int animFrame = 0;
	float animTimer = 0;
    float scale = 4.0f;
    int textIndex = 0;

    float glowTimer = 0;
    float alpha = 1.0f;
    float fadeAlpha = 1.0f;
    bool startPressed = false;
    float delayTimer = 0;
 
	float emoTimer = 0;

    Vector2 position = { 500, 264 };

	Rectangle currentEmotion = neutral;

    Font titleFont = LoadFont("assets/fonts/KiwiSoda.ttf");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {   
        BeginDrawing();
        ClearBackground(BLACK);
        float stallScale = 0.70f;

        // stop player leaving world
        if (player.x < 0) player.x = 0;
        if (player.x > cutWidth - player.width)
            player.x = cutWidth - player.width;

        switch (currentScreen)
        {
        case title_screen:
            timer[0] += GetFrameTime(); //increment the timer by the time elapsed since the last frame

            if (timer[0] >= 0.05) //if the timer has reached 0.05 seconds (20 frames per second), change the frame of the animation
            {
                currentFrame[0]++; //increment the current frame
                timer[0] = 0; //reset the timer

                if (currentFrame[0] >= 32) //if the current frame has reached the end of the animation, reset it to the beginning
                    currentFrame[0] = 0;
            }

            if (fadeAlpha > 0)
            {
                fadeAlpha -= 0.5f * GetFrameTime();
            }

            if (!startPressed)
            {
                glowTimer += GetFrameTime();
                alpha = 0.5f + 0.5f * sin(glowTimer * 3);
            }

            DrawTexturePro(
                frames[currentFrame[0]],  //texture
                { 0,0,(float)frames[currentFrame[0]].width,(float)frames[currentFrame[0]].height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                WHITE //tint color (white means no tint)
            );

            DrawTextEx(titleFont, title, Vector2{ 333, 204 }, 100, 2, WHITE);
            DrawTextEx(titleFont, title, Vector2{ 331, 202 }, 100, 2, MAROON);
            DrawText("PRESS ENTER TO START", 430, 600, 30, Fade(WHITE, alpha));
            DrawRectangle(0, 0, 1280, 720, Fade(BLACK, fadeAlpha));


            if (IsKeyPressed(KEY_ENTER) && !startPressed)
            {
                startPressed = true;
            }

            if (startPressed)
            {
                delayTimer += GetFrameTime();

                if (delayTimer > 0.7)

                    fadeAlpha += 0.8f * GetFrameTime();

                if (fadeAlpha >= 1)
                {
                    currentScreen = cutscene;
                }
            }

            DrawRectangle(0, 0, 1280, 720, Fade(BLACK, fadeAlpha));
            // Title screen logic
            break;
        case cutscene:
        {
            timer[1] += GetFrameTime(); //increment the timer by the time elapsed since the last frame

            if (timer[1] >= 0.05) //if the timer has reached 0.05 seconds (20 frames per second), change the frame of the animation
            {
                currentFrame[1]++; //increment the current frame
                timer[1] = 0; //reset the timer

                if (currentFrame[1] >= 14) //if the current frame has reached the end of the animation, reset it to the beginning
                    currentFrame[1] = 0;
            }

            timer[2] += GetFrameTime();

            DrawTexturePro(
                cutsceneFrames[currentFrame[1]],  //texture
                { 0,0,(float)cutsceneFrames[currentFrame[1]].width,(float)cutsceneFrames[currentFrame[1]].height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                WHITE //tint color (white means no tint)
            );

            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

            float fade = timer[2] / 2.0f;

            if (fade > 1.0f)
                fade = 1.0f;

            Color textColor = Fade(WHITE, fade);

            // Title
            DrawText("LOCAL NEWS", 520, 200, 30, textColor);

            // Headline
            DrawText("STRANGE STALL APPEARS IN ALLEY", 430, 260, 20, textColor);

            // Body
            DrawText("Witnesses say money appears when", 430, 310, 20, textColor);
            DrawText("interacting with an animatronic figure.", 430, 340, 20, textColor);

            // Important line (center-ish + bigger)
            DrawText("$25,000.", 540, 420, 40, Fade(RED, fade));

            // Final line
            DrawText("Thats what it would take to save her.", 430, 480, 20, textColor);

            DrawText("Press SPACE to continue", 490, 650, 20, Fade(WHITE, alpha));

            if (IsKeyPressed(KEY_SPACE))
            {
                currentScreen = movement;
            }
        } // cutscene logic
        break;
        case movement:
        {
            state = idle;

            float speed = 300 * GetFrameTime();
            Vector2 playerCenter = {
                player.x + player.width * scale / 2,
                player.y + player.height * scale / 2
            };

            float distanceToNewspaper = Vector2Distance(playerCenter, newspaperPos);
            bool nearNewspaper = distanceToNewspaper < 100;

            bool nearStall = CheckCollisionRecs(player, stallZone);

            timer[2] += GetFrameTime(); //increment the timer by the time elapsed since the last frame

            if (timer[2] >= 0.05) //if the timer has reached 0.05 seconds (20 frames per second), change the frame of the animation
            {
                timer[2] = 0; //reset the timer
            }

            if (fadeAlpha > 0)
            {
                fadeAlpha -= 0.5f * GetFrameTime();
            }

            if (nearStall && IsKeyPressed(KEY_E) && !usingStall)
            {
                usingStall = true;
            }
            if (usingStall)
            {
                delayTimer += GetFrameTime();

                if (delayTimer > 0.7)

                    fadeAlpha += 0.8f * GetFrameTime();

                if (fadeAlpha >= 1.0f)
                {
					fadeAlpha = 1.0f;
                    currentScreen = game_screen;
                }
            }

            if (nearNewspaper && IsKeyPressed(KEY_E))
            {
                readNews = true;
            }

            if (IsKeyDown(KEY_RIGHT))
            {
                player.x += speed;
                playerDirection = right;
                state = walk;
            }

            if (IsKeyDown(KEY_LEFT))
            {
                player.x -= speed;
                playerDirection = left;
                state = walk;
            }

            if (state == idle)
                animFrame %= 4;
            else
                animFrame %= 6;

            animTimer += GetFrameTime();

            if (animTimer >= 0.1f)
            {
                animFrame++;
                animTimer = 0;
            }

            if (player.x < 0) player.x = 0;
            if (player.x > cutWidth - player.width)
                player.x = cutWidth - player.width;

            camera.target = { player.x + player.width / 2, screenHeight / 2 };

            if (camera.target.x < camera.offset.x)
                camera.target.x = camera.offset.x;

            if (camera.target.x > cutWidth - (screenWidth - camera.offset.x))
                camera.target.x = cutWidth - (screenWidth - camera.offset.x);

            Texture2D playerTexture;

            if (state == walk)
            {
                if (playerDirection == right)
                    playerTexture = walkRight[animFrame % 6];
                else
                    playerTexture = walkLeft[animFrame % 6];
            }
            else
            {
                if (playerDirection == right)
                    playerTexture = rightIdle[animFrame % 4];
                else
                    playerTexture = leftIdle[animFrame % 4];
            }

            BeginMode2D(camera);
            DrawTexture(background, 0, 0, WHITE);


            Rectangle stallSource = { 0, 0, (float)stall.width, (float)stall.height };

            Rectangle stallDest = {
                550,
                430,
                stall.width * stallScale,
                stall.height * stallScale
            };

            DrawTexturePro(stall, stallSource, stallDest, { stallDest.width / 2, stallDest.height / 2 }, 0, LIGHTGRAY);

            Rectangle source = { 0, 0, (float)playerTexture.width, (float)playerTexture.height };

            Rectangle dest = {
                player.x,
                player.y,
                playerTexture.width * scale,
                playerTexture.height * scale
            };

            DrawTexturePro(playerTexture, source, dest, { dest.width / 2, dest.height / 2 }, 0, WHITE);

            EndMode2D();
            if (nearStall)
            {
                DrawText("Press E to interact with stall", 450, 650, 20, WHITE);
            }
            if (nearNewspaper)
            {
                DrawText("Press E to read newspaper", 450, 650, 20, WHITE);
            }
            if (readNews)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

                DrawText("LOCAL NEWS", 500, 200, 40, WHITE);
                DrawText("STRANGE STALL APPEARS IN ALLEY", 350, 280, 20, WHITE);
                DrawText("Witnesses say money appears when", 350, 320, 20, WHITE);
                DrawText("interacting with an animatronic figure.", 350, 350, 20, WHITE);

                DrawText("Press X to close", 500, 450, 20, GRAY);

                if (IsKeyPressed(KEY_X))
                {
                    readNews = false;
                }
            }
            DrawText("Press Left, Right Arrow Keys to move", 980, 690, 16, WHITE);
            DrawRectangle(0, 0, 1280, 720, Fade(BLACK, fadeAlpha));
            break;
        }
        case game_screen:
        {
            ClearBackground(BLACK);

            DrawTexturePro(
                bg1,  //texture
                { 0,0,(float)bg1.width,(float)bg1.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTexturePro(
                bg2,  //texture
                { 0,0,(float)bg2.width,(float)bg2.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTextureRec(emotion, currentEmotion, position, WHITE);

            DrawTexturePro(banner,
                { 0, 0, (float)banner.width, (float)banner.height },
                { 0,0, 400,100 },
                { 0, 0 }, 0, WHITE);

            DrawText(TextFormat("Money = $%d", money), 20, 30, 43, BLACK);

            DrawText("Press SPACE to interact with stall", 355, 640, 30, WHITE);

            if (IsKeyPressed(KEY_SPACE))
            {
                currentScreen = screen2;
            }
            break;
        }
        case screen2:
        {
            ClearBackground(BLACK);

            DrawTexturePro(
                bg1,  //texture
                { 0,0,(float)bg1.width,(float)bg1.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTexturePro(
                bg2,  //texture
                { 0,0,(float)bg2.width,(float)bg2.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );
           
            DrawTextureRec(emotion, currentEmotion, position, WHITE);
            DrawTexturePro(banner,
                { 0, 0, (float)banner.width, (float)banner.height },
                { 0,0, 400,100 },
                { 0, 0 }, 0, WHITE);

            DrawText(TextFormat("Money = $%d", money), 20, 30, 43, BLACK);

            if (IsKeyPressed(KEY_ENTER))
            {
                if (dialogueIndex < 5)
                {
                    dialogueIndex++;

                    if (dialogueIndex == 1)
                        currentEmotion = smile;
                    else if (dialogueIndex == 5)
                        currentEmotion = laugh;
                }
                else
                {
                    currentScreen = screen3;
                    currentEmotion = smile;
                }
            }
            DrawText(dialogue[dialogueIndex].c_str(), 30, 620, 18, WHITE);
            DrawText("Press ENTER to continue", 1070, 690, 16, WHITE);
            break;
        }
        case screen3:
        {
            ClearBackground(BLACK);

            DrawTexturePro(
                bg1,  //texture
                { 0,0,(float)bg1.width,(float)bg1.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTexturePro(
                bg2,  //texture
                { 0,0,(float)bg2.width,(float)bg2.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );
            DrawTexturePro(banner,
                { 0, 0, (float)banner.width, (float)banner.height },
                { 0,0, 400,100 },
                { 0, 0 }, 0, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                money++;
                shakeTime = shakeDuration;
                if (money == 100)
                {
                    currentEmotion = laugh;
                    dialogueIndex = 6;        //feather
					money = money + 400; //money = 500
                }
                else if (money > 100 && money < 700)
                {
                    currentEmotion = smile;
                }
                else if (money == 700 && !Choice[0])
                {
                    currentEmotion = worried;
                    dialogueIndex = 7;
					Choice[0] = true;
                }
                else if (money > 1900 && money < 3000)
                {
                    currentEmotion = hurt;
                }
                else if (money == 3000 && !Choice[1])
                {
                    currentEmotion = laugh;
                    dialogueIndex = 8; //hammer, money = 4900
					Choice[1] = true;
                }
                else if (money > 3000 && money < 5000)
                {
                    currentEmotion = sad;
                }
                else if (money == 5000 && !Choice[2])
                {
                    currentEmotion = hurt; //knife, money = 10000
					dialogueIndex = 9;
					Choice[2] = true;
                }
                else if (money > 5000 && money < 10000)
                {
                    currentEmotion = scared1;
                }
            }
            if (Choice[0])
            {
                DrawText("Would you take it? (Y/N)", 476, 650, 20, RED);

                if (IsKeyPressed(KEY_Y))
                {
                    money += 1200;
                    Choice[0] = false;
                    bad++;
                }

                if (IsKeyPressed(KEY_N))
                {
                    Choice[0] = false;
                    good++;
                }
            }
            if (Choice[1])
            {
                DrawText("Would you take it? (Y/N)", 476, 650, 20, RED);

                if (IsKeyPressed(KEY_Y))
                {

                    money = money + 2000;
                    Choice[1] = false;
                    bad++;
                }

                if (IsKeyPressed(KEY_N))
                {
                    Choice[1] = false;
                    good++;
                }
            }
            if (Choice[2])
            {
                DrawText("Would you take it? (Y/N)", 476, 650, 20, RED);

                if (IsKeyPressed(KEY_Y))
                {

                    money += 5000;
                    currentEmotion = scared1;
                    Choice[2] = false;
                    bad++;
                }

                if (IsKeyPressed(KEY_N))
                {
                    Choice[2] = false;
                    good++;

                }
            }
            if (money >= 10000 && currentScreen != final_s)
            {
                DrawText("Press ENTER to continue", 1070, 690, 16, WHITE);

                currentEmotion = scared1;

                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = final_s;
                }
            }
            DrawText(dialogue[dialogueIndex].c_str(), 30, 620, 18, WHITE);

            Vector2 shakeOffset = { 0, 0 };

            if (shakeTime > 0)
            {
                shakeTime -= GetFrameTime();

                shakeOffset.x = (float)GetRandomValue(-shakeStrength, shakeStrength);
                shakeOffset.y = (float)GetRandomValue(-shakeStrength, shakeStrength);
            }

            Vector2 shakenPos = { position.x + shakeOffset.x, position.y + shakeOffset.y };
			DrawRectangle(513, 565, 240, 40, BLACK);
            DrawTextureRec(emotion, currentEmotion, shakenPos, WHITE);

            DrawText(TextFormat("Money = $%d", money), 20, 30, 43, BLACK);
            break;
        }
        case final_s:
        {
            ClearBackground(BLACK);

            DrawTexturePro(
                bg1,  //texture
                { 0,0,(float)bg1.width,(float)bg1.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTexturePro(
                bg2,  //texture
                { 0,0,(float)bg2.width,(float)bg2.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );
            DrawRectangle(513, 565, 240, 40, BLACK);
            DrawTextureRec(emotion, currentEmotion, position, WHITE);
            DrawTexturePro(banner,
                { 0, 0, (float)banner.width, (float)banner.height },
                { 0,0, 400,100 },
                { 0, 0 }, 0, WHITE);

            DrawText(TextFormat("Money = $%d", money), 20, 30, 43, BLACK);
            DrawText("Press ENTER to continue", 1070, 690, 16, WHITE);
            if (IsKeyPressed(KEY_ENTER))
            {
                if (dialogueIndex < 12)
                {
                    dialogueIndex++;

                    if (dialogueIndex == 10)
                    {
                        currentEmotion = hurt;
 
                    }
                    else if (dialogueIndex == 12)
                        currentEmotion = sad;
                }
                else
                {
                    currentScreen = last1;
                    currentEmotion = cry1;
                }
            }
            DrawText(dialogue[dialogueIndex].c_str(), 30, 620, 18, WHITE);
            break;
        }
        case last1:
        {
            ClearBackground(BLACK);

            DrawTexturePro(
                bg1,  //texture
                { 0,0,(float)bg1.width,(float)bg1.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTexturePro(
                bg2,  //texture
                { 0,0,(float)bg2.width,(float)bg2.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );
            DrawTexturePro(banner,
                { 0, 0, (float)banner.width, (float)banner.height },
                { 0,0, 400,100 },
                { 0, 0 }, 0, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                money++;
                shakeTime = shakeDuration;
                if (money >= 10000 && money < 15000)
                {
                    currentEmotion = cry2;
                }
                else if (money == 15000)
                {
                    currentEmotion = hurt;
                    dialogueIndex = 13;
                    currentScreen = last2;
                }

            }

            DrawText(dialogue[dialogueIndex].c_str(), 30, 620, 18, WHITE);

            Vector2 shakeOffset = { 0, 0 };

            if (shakeTime > 0)
            {
                shakeTime -= GetFrameTime();

                shakeOffset.x = (float)GetRandomValue(-shakeStrength, shakeStrength);
                shakeOffset.y = (float)GetRandomValue(-shakeStrength, shakeStrength);
            }

            Vector2 shakenPos = { position.x + shakeOffset.x, position.y + shakeOffset.y };
          
            DrawRectangle(513, 565, 240, 40, BLACK);
            DrawTextureRec(emotion, currentEmotion, shakenPos, WHITE);
            DrawText(TextFormat("Money = $%d", money), 20, 30, 43, BLACK);
            break;
        }
        case last2:
        {
            ClearBackground(BLACK);

            DrawTexturePro(
                bg1,  //texture
                { 0,0,(float)bg1.width,(float)bg1.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );

            DrawTexturePro(
                bg2,  //texture
                { 0,0,(float)bg2.width,(float)bg2.height },  //start at the top left corner of the texture(0,0) and use the entire texture as the source rectangle
                { 0,0,screenWidth,screenHeight },
                { 0,0 }, //origin (pivot point) is set to the top left corner of the destination rectangle
                0, //no rotation
                GRAY //tint color (white means no tint)
            );
            DrawRectangle(513, 565, 240, 40, BLACK);
            DrawTextureRec(emotion, currentEmotion, position, WHITE);
            DrawTexturePro(banner,
                { 0, 0, (float)banner.width, (float)banner.height },
                { 0,0, 400,100 },
                { 0, 0 }, 0, WHITE);

            DrawText(TextFormat("Money = $%d", money), 20, 30, 43, BLACK);

            if (IsKeyPressed(KEY_ENTER))
            {
                if (dialogueIndex < 19)
                {
                    dialogueIndex++;

                    if (dialogueIndex == 14)
                        currentEmotion = scream;
                    else if (dialogueIndex == 15)
                        currentEmotion = scared1;
                    else if (dialogueIndex == 16)
                        currentEmotion = sad;
                }
                else
                {
                    currentScreen = last3;
                    currentEmotion = cry1;
                }
            }
            DrawText(dialogue[dialogueIndex].c_str(), 30, 620, 18, WHITE);
            DrawText("Press ENTER to continue", 1070, 690, 16, WHITE);
            break;
        }
        case last3:
            ClearBackground(BLACK);

            const char* gameOver = "GAME OVER";

            const char* choiceText;

            if (bad >= good)
                choiceText = "You made some bad choices...";
            else
                choiceText = "You made some good choices...";

            const char* exitText = "Press ESC to exit";

            int gameOverSize = 80;
            int normalSize = 30;
            int smallSize = 20;

            // Center positions
            int gameOverX = (screenWidth - MeasureText(gameOver, gameOverSize)) / 2;

            string moneyText = TextFormat("Money = $%d", money);
            int moneyX = (screenWidth - MeasureText(moneyText.c_str(), normalSize)) / 2;

            int choiceX = (screenWidth - MeasureText(choiceText, smallSize)) / 2;
            int exitX = (screenWidth - MeasureText(exitText, smallSize)) / 2;

            DrawText(gameOver, gameOverX, 250, gameOverSize, WHITE);

            DrawText(moneyText.c_str(), moneyX, 360, normalSize, WHITE);

            if (bad >= good)
                DrawText(choiceText, choiceX, 500, smallSize, RED);
            else
                DrawText(choiceText, choiceX, 500, smallSize, GREEN);

            DrawText(exitText, exitX, 600, smallSize, GRAY);

            break;

        }
        EndDrawing();
    }
	UnloadTexture(emotion);
    CloseWindow();
    UnloadFont(titleFont);

    return 0;
}