#include "iGraphics.h"
#include "GameCommon.h"
#include "Player.h"
#include "Enemy.h"
#include "PowerUp.h"
#include "LevelManager.h"
#include "SettingsManager.h"
#include "SaveManager.h"
#include "HighScoreManager.h"
#include "MenuSystem.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENEMIES 16

AppState appState = STATE_MAIN_MENU;

Player player;
Enemy enemies[MAX_ENEMIES];
int enemyCount = 0;
Enemy enemyTemplates[3];

PowerUp powerUp;
Settings settings;
SaveData saveData;
HighScoreList highScores;

unsigned int bgTex = 0;        
unsigned int titleTex = 0;     
unsigned int arenaBgTex = 0;   
unsigned int groundTex = 0;    
unsigned int heartFullTex = 0;
unsigned int heartEmptyTex = 0;

double cameraX = 0;            

int currentDay = 1;
int livesRemaining = TOTAL_LIVES;
int dayTransitionTimer = 0;
int deathPauseTimer = 0;
int dayClearPauseTimer = 0;
bool pendingGameOver = false;   

char nameEntryBuffer[MAX_NAME_LEN];
int nameEntryLen = 0;

bool deathHandled = false;
bool dayClearHandled = false;

void copyEnemyTextures(Enemy &dst, Enemy &src);
void spawnEnemiesForDay(int day);
void beginDay();
void beginDayTransition();
void startNewGame();
void continueGame();
void onDayCleared();
void triggerPlayerDeathSequence();
void triggerDayClearSequence();
void resolvePlayerAttack();
void resolveEnemyAttack(Enemy &e);
void updateCamera();

int buildMainMenuButtons(Button out[6]);
int buildConfirmButtons(Button out[2]);
int buildOptionsButtons(Button out[2]);
int buildAudioButtons(Button out[5]);
int buildBackOnlyButton(Button out[1], double y);
int buildPauseButtons(Button out[2]);
int buildReturnButton(Button out[1]);

void drawMainMenuScreen();
void drawNewGameConfirmScreen();
void drawOptionsMenuScreen();
void drawAudioSettingsScreen();
void drawAboutMenuScreen();
void drawHighScoresScreen();
void drawNameEntryScreen();
void drawDayTransitionScreen();
void drawGameplayScene();
void drawPlayerHealthBar();
void drawPauseMenuScreen();
void drawGameOverScreen();
void drawGameCompleteScreen();
void drawScrollingWorldBackground();
void drawHeartsUI();

void handleMainMenuClicks();
void handleNewGameConfirmClicks();
void handleOptionsMenuClicks();
void handleAudioSettingsClicks();
void handleAboutMenuClicks();
void handleHighScoresMenuClicks();
void handleNameEntryInput();
void handleDayTransition();
void handleGameplay();
void handleDeathPause();
void handleDayClearPause();
void handlePauseMenuClicks();
void handleGameOverInput();
void handleGameCompleteInput();

void loadAllAssets()
{
    loadPlayerAssets(player);

    resetEnemy(enemyTemplates[0], ENEMY_SMALL, 0, 0, 0);  loadEnemyAssets(enemyTemplates[0]);
    resetEnemy(enemyTemplates[1], ENEMY_MEDIUM, 0, 0, 0); loadEnemyAssets(enemyTemplates[1]);
    resetEnemy(enemyTemplates[2], ENEMY_LARGE, 0, 0, 0);  loadEnemyAssets(enemyTemplates[2]);

    loadPowerUpAssets(powerUp);

    bgTex      = iLoadImage("assets/menu_background.png");
    titleTex   = iLoadImage("assets/title.png");
    arenaBgTex = iLoadImage("assets/background.png");
    groundTex  = iLoadImage("assets/ground.png");
    heartFullTex  = iLoadImage("assets/hearts/heart_full.png");
    heartEmptyTex = iLoadImage("assets/hearts/heart_empty.png");
}

void copyEnemyTextures(Enemy &dst, Enemy &src)
{
    memcpy(dst.idleTex,   src.idleTex,   sizeof(dst.idleTex));
    memcpy(dst.walkTex,   src.walkTex,   sizeof(dst.walkTex));
    memcpy(dst.attackTex, src.attackTex, sizeof(dst.attackTex));
    memcpy(dst.hurtTex,   src.hurtTex,   sizeof(dst.hurtTex));
    memcpy(dst.deadTex,   src.deadTex,   sizeof(dst.deadTex));
}

void spawnEnemiesForDay(int day)
{
    enemyCount = 0;
    DayWave wave = getDayWave(day);
    double spacing = 140;
    double startX = 500;

    for (int i = 0; i < wave.smallCount && enemyCount < MAX_ENEMIES; i++)
    {
        double x = startX + enemyCount * spacing;
        resetEnemy(enemies[enemyCount], ENEMY_SMALL, x, x - 70, x + 70);
        copyEnemyTextures(enemies[enemyCount], enemyTemplates[0]);
        enemyCount++;
    }
    for (int i = 0; i < wave.mediumCount && enemyCount < MAX_ENEMIES; i++)
    {
        double x = startX + enemyCount * spacing;
        resetEnemy(enemies[enemyCount], ENEMY_MEDIUM, x, x - 70, x + 70);
        copyEnemyTextures(enemies[enemyCount], enemyTemplates[1]);
        enemyCount++;
    }
    for (int i = 0; i < wave.largeCount && enemyCount < MAX_ENEMIES; i++)
    {
        double x = startX + enemyCount * spacing;
        resetEnemy(enemies[enemyCount], ENEMY_LARGE, x, x - 70, x + 70);
        copyEnemyTextures(enemies[enemyCount], enemyTemplates[2]);
        enemyCount++;
    }
}

void updateCamera()
{
    double target = player.x - WINDOW_W / 2.0;
    if (target < 0) target = 0;
    if (target > WORLD_W - WINDOW_W) target = WORLD_W - WINDOW_W;
    cameraX = target;
}

void beginDay()
{
    resetPlayer(player, 150);
    spawnEnemiesForDay(currentDay);
    resetPowerUp(powerUp);
    cameraX = 0;
    deathHandled = false;
    dayClearHandled = false;
    pendingGameOver = false;
    appState = STATE_PLAYING;
}

void beginDayTransition()
{
    dayTransitionTimer = DAY_TRANSITION_TICKS;
    appState = STATE_DAY_TRANSITION;
}

void startNewGame()
{
    currentDay = 1;
    livesRemaining = TOTAL_LIVES;
    saveData.currentDay = currentDay;
    saveData.livesRemaining = livesRemaining;
    writeSave(saveData);
    beginDayTransition();
}

void continueGame()
{
    if (!saveFileExists()) return;
    loadSave(saveData);
    currentDay = saveData.currentDay;
    livesRemaining = saveData.livesRemaining;
    beginDayTransition();
}

void onDayCleared()
{
    if (currentDay >= TOTAL_DAYS)
    {
        nameEntryLen = 0;
        nameEntryBuffer[0] = '\0';
        appState = STATE_NAME_ENTRY;
    }
    else
    {
        currentDay++;
        saveData.currentDay = currentDay;
        saveData.livesRemaining = livesRemaining;
        writeSave(saveData);
        beginDayTransition();
    }
}

void triggerPlayerDeathSequence()
{
    livesRemaining--;
    if (livesRemaining <= 0)
    {
        deleteSave();
        pendingGameOver = true;
    }
    else
    {
        saveData.currentDay = currentDay;
        saveData.livesRemaining = livesRemaining;
        writeSave(saveData);
        pendingGameOver = false;
    }
    deathPauseTimer = DEATH_PAUSE_TICKS;
    appState = STATE_DEATH_PAUSE;
}

void triggerDayClearSequence()
{
    dayClearPauseTimer = DAY_CLEAR_PAUSE_TICKS;
    appState = STATE_DAY_CLEAR_PAUSE;
}

void resolvePlayerAttack()
{
    HitBox pBox = getPlayerBox(player);
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i].alive || enemies[i].state == DEAD) continue;
        HitBox eBox = getEnemyBox(enemies[i]);
        if (aabbOverlap(pBox, eBox))
        {
            applyDamageToEnemy(enemies[i], getPlayerAttackDamage(player));
            break;
        }
    }
}

void resolveEnemyAttack(Enemy &e)
{
    if (player.state == DEAD) return;
    HitBox eBox = getEnemyBox(e);
    HitBox pBox = getPlayerBox(player);
    if (aabbOverlap(eBox, pBox))
        applyDamageToPlayer(player, e.stats.attackDamage);
}

int buildMainMenuButtons(Button out[6])
{
    double w = 320, h = 48, gap = 10;
    double x = WINDOW_W / 2 - w / 2;
    double topY = 300;
    out[0] = makeButton(x, topY,                w, h, "New Game");
    out[1] = makeButton(x, topY - 1*(h+gap),    w, h, "Continue");
    out[2] = makeButton(x, topY - 2*(h+gap),    w, h, "Options");
    out[3] = makeButton(x, topY - 3*(h+gap),    w, h, "High Scores");
    out[4] = makeButton(x, topY - 4*(h+gap),    w, h, "About the Game");
    out[5] = makeButton(x, topY - 5*(h+gap),    w, h, "Exit");
    return 6;
}

int buildConfirmButtons(Button out[2])
{
    double w = 140, h = 44;
    double y = WINDOW_H / 2 - 60;
    out[0] = makeButton(WINDOW_W/2 - w - 15, y, w, h, "OK");
    out[1] = makeButton(WINDOW_W/2 + 15,     y, w, h, "Cancel");
    return 2;
}

int buildOptionsButtons(Button out[2])
{
    double w = 280, h = 48, gap = 12;
    double x = WINDOW_W/2 - w/2;
    double topY = 280;
    out[0] = makeButton(x, topY,            w, h, "Audio Settings");
    out[1] = makeButton(x, topY - (h+gap),  w, h, "Back");
    return 2;
}

int buildAudioButtons(Button out[5])
{
    double w = 50, h = 40;
    out[0] = makeButton(WINDOW_W/2 - 160, 300, w, h, "-");
    out[1] = makeButton(WINDOW_W/2 + 110, 300, w, h, "+");
    out[2] = makeButton(WINDOW_W/2 - 160, 220, w, h, "-");
    out[3] = makeButton(WINDOW_W/2 + 110, 220, w, h, "+");
    out[4] = makeButton(WINDOW_W/2 - 90, 130, 180, 44, "Back");
    return 5;
}

int buildBackOnlyButton(Button out[1], double y)
{
    double w = 180, h = 44;
    out[0] = makeButton(WINDOW_W/2 - w/2, y, w, h, "Back");
    return 1;
}

int buildPauseButtons(Button out[2])
{
    double w = 220, h = 46, gap = 14;
    double x = WINDOW_W/2 - w/2;
    double topY = WINDOW_H/2 + 20;
    out[0] = makeButton(x, topY,            w, h, "Save & Quit");
    out[1] = makeButton(x, topY - (h+gap),  w, h, "Continue");
    return 2;
}

int buildReturnButton(Button out[1])
{
    double w = 220, h = 44;
    out[0] = makeButton(WINDOW_W/2 - w/2, 150, w, h, "Return to Menu");
    return 1;
}

void drawMainMenuScreen()
{
    drawBackgroundImage(bgTex);
    if (titleTex != 0)
        iShowImage(WINDOW_W/2 - 250, WINDOW_H - 150, 500, 120, titleTex);

    Button btns[6];
    int n = buildMainMenuButtons(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawNewGameConfirmScreen()
{
    double boxW = 520, boxH = 180;
    double boxX = WINDOW_W/2 - boxW/2;
    double boxY = WINDOW_H/2 - boxH/2;

    iSetColor(20, 20, 25);
    iFilledRectangle(boxX, boxY, boxW, boxH);
    iSetColor(255, 255, 255);
    iRectangle(boxX, boxY, boxW, boxH);

    drawCenteredText(WINDOW_W/2, boxY + 130, "Your previous session will be lost.", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(WINDOW_W/2, boxY + 105, "Would you like to start over?", GLUT_BITMAP_HELVETICA_18);

    Button btns[2];
    int n = buildConfirmButtons(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawOptionsMenuScreen()
{
    drawBackgroundImage(bgTex);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H - 100, "Options", GLUT_BITMAP_HELVETICA_18);

    Button btns[2];
    int n = buildOptionsButtons(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawAudioSettingsScreen()
{
    drawBackgroundImage(bgTex);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H - 100, "Audio Settings", GLUT_BITMAP_HELVETICA_18);

    char sfxLabel[32];
    char musicLabel[32];
    sprintf(sfxLabel,   "SFX Volume: %d",   settings.sfxVolume);
    sprintf(musicLabel, "Music Volume: %d", settings.musicVolume);
    drawCenteredText(WINDOW_W/2, 330, sfxLabel,   GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(WINDOW_W/2, 250, musicLabel, GLUT_BITMAP_HELVETICA_18);

    Button btns[5];
    int n = buildAudioButtons(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawAboutMenuScreen()
{
    drawBackgroundImage(bgTex);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H - 100, "About the Game", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2, "Credits: (coming soon)", GLUT_BITMAP_HELVETICA_18);

    Button btns[1];
    int n = buildBackOnlyButton(btns, 150);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawHighScoresScreen()
{
    drawBackgroundImage(bgTex);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H - 100, "High Scores", GLUT_BITMAP_HELVETICA_18);

    if (highScores.count == 0)
    {
        drawCenteredText(WINDOW_W/2, WINDOW_H/2, "No winners yet.", GLUT_BITMAP_HELVETICA_18);
    }
    else
    {
        double y = WINDOW_H/2 + 60;
        for (int i = 0; i < highScores.count; i++)
        {
            drawCenteredText(WINDOW_W/2, y, highScores.names[i], GLUT_BITMAP_HELVETICA_18);
            y -= 35;
        }
    }

    Button btns[1];
    int n = buildBackOnlyButton(btns, 150);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawNameEntryScreen()
{
    drawDimOverlay();
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 80, "You saved the village! Enter your name:", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 20, nameEntryBuffer, GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 - 40, "Press ENTER to confirm (Backspace to edit)", GLUT_BITMAP_HELVETICA_18);
}

void drawDayTransitionScreen()
{
    drawDimOverlay();
    char label[16];
    sprintf(label, "Day %d", currentDay);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2, label, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawPauseMenuScreen()
{
    double boxW = 400, boxH = 220;
    double boxX = WINDOW_W/2 - boxW/2;
    double boxY = WINDOW_H/2 - boxH/2;

    iSetColor(20, 20, 25);
    iFilledRectangle(boxX, boxY, boxW, boxH);
    iSetColor(255, 255, 255);
    iRectangle(boxX, boxY, boxW, boxH);
    drawCenteredText(WINDOW_W/2, boxY + boxH - 30, "Paused", GLUT_BITMAP_HELVETICA_18);

    Button btns[2];
    int n = buildPauseButtons(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawGameOverScreen()
{
    drawDimOverlay();
    iSetColor(230, 60, 60);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 60, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 10, "The village has fallen to the Destroyers.", GLUT_BITMAP_HELVETICA_18);

    Button btns[1];
    int n = buildReturnButton(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawGameCompleteScreen()
{
    drawDimOverlay();
    iSetColor(255, 215, 60);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 80, "VICTORY", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255, 255, 255);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 30, "You actually saved the village from every", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(WINDOW_W/2, WINDOW_H/2 + 5,  "wave attacks of the destroyers.", GLUT_BITMAP_HELVETICA_18);

    Button btns[1];
    int n = buildReturnButton(btns);
    for (int i = 0; i < n; i++)
        drawButton(btns[i], isPointInButton(btns[i], iMouseX, iMouseY));
}

void drawPlayerHealthBar()
{
    double x = 30, y = WINDOW_H - 90, barW = 250, barH = 22;

    iSetColor(40, 40, 40);
    iFilledRectangle(x, y, barW, barH);

    double pct = (double)player.health / (double)PLAYER_MAX_HEALTH;
    if (pct < 0) pct = 0;
    if (pct > 0.5) iSetColor(30, 200, 60);
    else if (pct > 0.25) iSetColor(230, 200, 30);
    else iSetColor(200, 30, 30);
    iFilledRectangle(x, y, barW * pct, barH);

    iSetColor(255, 255, 255);
    iRectangle(x, y, barW, barH);
    iText(x, y + barH + 4, (char*)"PROTECTOR");
}

void drawHeartsUI()
{
    double startX = 30, y = WINDOW_H - 30, size = 26, spacing = 32;

    for (int i = 0; i < TOTAL_LIVES; i++)
    {
        double x = startX + i * spacing;
        bool filled = (i < livesRemaining);
        unsigned int tex = filled ? heartFullTex : heartEmptyTex;

        if (tex != 0)
        {
            iShowImage(x, y, (int)size, (int)size, tex);
        }
        else
        {
            if (filled) iSetColor(230, 40, 60);
            else iSetColor(70, 70, 70);
            iFilledCircle(x + size / 2, y + size / 2, size / 2);
            iSetColor(255, 255, 255);
            iCircle(x + size / 2, y + size / 2, size / 2);
        }
    }
}

void drawScrollingWorldBackground()
{
    int tileW = WINDOW_W;
    int startTile = (int)(cameraX / tileW) - 1;
    int endTile = (int)((cameraX + WINDOW_W) / tileW) + 1;

    for (int t = startTile; t <= endTile; t++)
    {
        double tileScreenX = (t * tileW) - cameraX;

        if (arenaBgTex != 0)
            iShowImage(tileScreenX, 0, tileW, WINDOW_H, arenaBgTex);
        else
        {
            iSetColor(25, 25, 40);
            iFilledRectangle(tileScreenX, 0, tileW, WINDOW_H);
        }

        if (groundTex != 0)
            iShowImage(tileScreenX, 0, tileW, GROUND_Y, groundTex);
        else
        {
            iSetColor(80, 60, 40);
            iFilledRectangle(tileScreenX, 0, tileW, GROUND_Y);
        }
    }
}

void drawGameplayScene()
{
    drawScrollingWorldBackground();

    for (int i = 0; i < enemyCount; i++)
        drawEnemy(enemies[i], cameraX);

    drawPlayer(player, cameraX);
    drawPowerUp(powerUp, cameraX);
    drawHeartsUI();
    drawPlayerHealthBar();

    char hud[64];
    sprintf(hud, "Day %d / %d", currentDay, TOTAL_DAYS);
    iSetColor(255, 255, 255);
    iText(20, WINDOW_H - 120, hud);
    iText(20, WINDOW_H - 140, (char*)"A/D move  W jump  J attack  ESC pause");
}

void handleMainMenuClicks()
{
    if (!g_mouseClicked) return;
    Button btns[6];
    int n = buildMainMenuButtons(btns);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);

    if (idx == 0)       { if (saveFileExists()) appState = STATE_NEW_GAME_CONFIRM; else startNewGame(); }
    else if (idx == 1)  { continueGame(); }
    else if (idx == 2)  { appState = STATE_OPTIONS_MENU; }
    else if (idx == 3)  { loadHighScores(highScores); appState = STATE_HIGHSCORES_MENU; }
    else if (idx == 4)  { appState = STATE_ABOUT_MENU; }
    else if (idx == 5)  { exit(0); }
}

void handleNewGameConfirmClicks()
{
    if (!g_mouseClicked) return;
    Button btns[2];
    int n = buildConfirmButtons(btns);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);

    if (idx == 0) startNewGame();
    else if (idx == 1) appState = STATE_MAIN_MENU;
}

void handleOptionsMenuClicks()
{
    if (!g_mouseClicked) return;
    Button btns[2];
    int n = buildOptionsButtons(btns);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);

    if (idx == 0) appState = STATE_AUDIO_SETTINGS;
    else if (idx == 1) appState = STATE_MAIN_MENU;
}

void handleAudioSettingsClicks()
{
    if (!g_mouseClicked) return;
    Button btns[5];
    int n = buildAudioButtons(btns);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);

    if (idx == 0) decreaseSfx(settings);
    else if (idx == 1) increaseSfx(settings);
    else if (idx == 2) decreaseMusic(settings);
    else if (idx == 3) increaseMusic(settings);
    else if (idx == 4) appState = STATE_OPTIONS_MENU;
}

void handleAboutMenuClicks()
{
    if (!g_mouseClicked) return;
    Button btns[1];
    int n = buildBackOnlyButton(btns, 150);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);
    if (idx == 0) appState = STATE_MAIN_MENU;
}

void handleHighScoresMenuClicks()
{
    if (!g_mouseClicked) return;
    Button btns[1];
    int n = buildBackOnlyButton(btns, 150);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);
    if (idx == 0) appState = STATE_MAIN_MENU;
}

void handleNameEntryInput()
{
    for (int c = 'a'; c <= 'z'; c++)
        if (keyJustPressed((unsigned char)c) && nameEntryLen < MAX_NAME_LEN - 1)
        { nameEntryBuffer[nameEntryLen++] = (char)c; nameEntryBuffer[nameEntryLen] = '\0'; }

    for (int c = 'A'; c <= 'Z'; c++)
        if (keyJustPressed((unsigned char)c) && nameEntryLen < MAX_NAME_LEN - 1)
        { nameEntryBuffer[nameEntryLen++] = (char)c; nameEntryBuffer[nameEntryLen] = '\0'; }

    if (keyJustPressed(' ') && nameEntryLen < MAX_NAME_LEN - 1)
    { nameEntryBuffer[nameEntryLen++] = ' '; nameEntryBuffer[nameEntryLen] = '\0'; }

    if (keyJustPressed(8) && nameEntryLen > 0)
    { nameEntryLen--; nameEntryBuffer[nameEntryLen] = '\0'; }

    if (keyJustPressed(13) && nameEntryLen > 0)
    {
        addHighScore(highScores, nameEntryBuffer);
        deleteSave();
        appState = STATE_GAME_COMPLETE;
    }
}

void handleDayTransition()
{
    dayTransitionTimer--;
    if (dayTransitionTimer <= 0)
        beginDay();
}

void handleGameplay()
{
    if (keyJustPressed(27))
    {
        appState = STATE_PAUSE_MENU;
        return;
    }

    if (updatePlayer(player))
        resolvePlayerAttack();

    for (int i = 0; i < enemyCount; i++)
        if (updateEnemy(enemies[i], player.x))
            resolveEnemyAttack(enemies[i]);

    updateCamera();

    updatePowerUp(powerUp);
    if (powerUp.active)
    {
        HitBox pBox = getPlayerBox(player);
        HitBox puBox = getPowerUpBox(powerUp);
        if (aabbOverlap(pBox, puBox))
        {
            if (powerUp.type == POWERUP_HEAL)
                healPlayer(player, POWERUP_HEAL_AMOUNT);
            else
                applyPlayerDamageBoost(player, POWERUP_DAMAGE_BONUS, POWERUP_DAMAGE_DURATION_TICKS);

            powerUp.active = false;
            powerUp.nextSpawnTimer = POWERUP_MIN_SPAWN_TICKS +
                (rand() % (POWERUP_MAX_SPAWN_TICKS - POWERUP_MIN_SPAWN_TICKS + 1));
        }
    }

    if (!deathHandled && player.state == DEAD &&
        player.frameIndex == ANIM_DEAD_COUNT - 1 && player.frameTimer == 0)
    {
        deathHandled = true;
        triggerPlayerDeathSequence();
        return;
    }

    if (!dayClearHandled)
    {
        bool anyAlive = false;
        for (int i = 0; i < enemyCount; i++)
            if (enemies[i].alive) { anyAlive = true; break; }

        if (!anyAlive)
        {
            dayClearHandled = true;
            triggerDayClearSequence();
        }
    }
}

void handleDeathPause()
{
    deathPauseTimer--;
    if (deathPauseTimer <= 0)
    {
        if (pendingGameOver) appState = STATE_GAME_OVER;
        else beginDayTransition();
    }
}

void handleDayClearPause()
{
    dayClearPauseTimer--;
    if (dayClearPauseTimer <= 0)
        onDayCleared();
}

void handlePauseMenuClicks()
{
    if (keyJustPressed(27))
    {
        appState = STATE_PLAYING;
        return;
    }
    if (!g_mouseClicked) return;

    Button btns[2];
    int n = buildPauseButtons(btns);
    int idx = getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY);

    if (idx == 0)
    {
        saveData.currentDay = currentDay;
        saveData.livesRemaining = livesRemaining;
        writeSave(saveData);
        appState = STATE_MAIN_MENU;
    }
    else if (idx == 1) appState = STATE_PLAYING;
}

void handleGameOverInput()
{
    bool triggered = false;
    if (g_mouseClicked)
    {
        Button btns[1];
        int n = buildReturnButton(btns);
        if (getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY) == 0)
            triggered = true;
    }
    if (keyJustPressed(13)) triggered = true;
    if (triggered) appState = STATE_MAIN_MENU;
}

void handleGameCompleteInput()
{
    bool triggered = false;
    if (g_mouseClicked)
    {
        Button btns[1];
        int n = buildReturnButton(btns);
        if (getClickedButtonIndex(btns, n, g_mouseClickX, g_mouseClickY) == 0)
            triggered = true;
    }
    if (keyJustPressed(13)) triggered = true;
    if (triggered) appState = STATE_MAIN_MENU;
}

void iDraw()
{
    iClear();

    switch (appState)
    {
        case STATE_MAIN_MENU:         drawMainMenuScreen(); break;
        case STATE_NEW_GAME_CONFIRM:  drawMainMenuScreen(); drawNewGameConfirmScreen(); break;
        case STATE_OPTIONS_MENU:      drawOptionsMenuScreen(); break;
        case STATE_AUDIO_SETTINGS:    drawAudioSettingsScreen(); break;
        case STATE_ABOUT_MENU:        drawAboutMenuScreen(); break;
        case STATE_HIGHSCORES_MENU:   drawHighScoresScreen(); break;
        case STATE_NAME_ENTRY:        drawNameEntryScreen(); break;
        case STATE_DAY_TRANSITION:    drawDayTransitionScreen(); break;
        case STATE_PLAYING:           drawGameplayScene(); break;
        case STATE_DEATH_PAUSE:       drawGameplayScene(); break;
        case STATE_DAY_CLEAR_PAUSE:   drawGameplayScene(); break;
        case STATE_PAUSE_MENU:        drawGameplayScene(); drawPauseMenuScreen(); break;
        case STATE_GAME_OVER:         drawGameOverScreen(); break;
        case STATE_GAME_COMPLETE:     drawGameCompleteScreen(); break;
    }
}

void fixedUpdate()
{
    switch (appState)
    {
        case STATE_MAIN_MENU:         handleMainMenuClicks(); break;
        case STATE_NEW_GAME_CONFIRM:  handleNewGameConfirmClicks(); break;
        case STATE_OPTIONS_MENU:      handleOptionsMenuClicks(); break;
        case STATE_AUDIO_SETTINGS:    handleAudioSettingsClicks(); break;
        case STATE_ABOUT_MENU:        handleAboutMenuClicks(); break;
        case STATE_HIGHSCORES_MENU:   handleHighScoresMenuClicks(); break;
        case STATE_NAME_ENTRY:        handleNameEntryInput(); break;
        case STATE_DAY_TRANSITION:    handleDayTransition(); break;
        case STATE_PLAYING:           handleGameplay(); break;
        case STATE_DEATH_PAUSE:       handleDeathPause(); break;
        case STATE_DAY_CLEAR_PAUSE:   handleDayClearPause(); break;
        case STATE_PAUSE_MENU:        handlePauseMenuClicks(); break;
        case STATE_GAME_OVER:         handleGameOverInput(); break;
        case STATE_GAME_COMPLETE:     handleGameCompleteInput(); break;
    }

    updatePrevKeyState();
    g_mouseClicked = false;
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        g_mouseClicked = true;
        g_mouseClickX = mx;
        g_mouseClickY = my;
    }
}

void main()
{
    srand((unsigned int)time(0));

    iInitialize(WINDOW_W, WINDOW_H, "Justice Avenger - Protector vs Destroyer");

    loadAllAssets();
    loadSettings(settings);
    loadHighScores(highScores);

    appState = STATE_MAIN_MENU;

    iStart();
}
