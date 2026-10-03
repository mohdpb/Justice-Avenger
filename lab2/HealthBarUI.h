

#ifndef HEALTH_BAR_UI_H
#define HEALTH_BAR_UI_H

#include "iGraphics.h"
#include <stdio.h>


static unsigned int g_playerHpBgTex   = 0;
static unsigned int g_playerHpFillTex = 0;
static unsigned int g_enemyHpBgTex    = 0;
static unsigned int g_enemyHpFillTex  = 0;

static inline bool uiCheckFile(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fclose(f);
    return true;
}

static inline unsigned int loadUiTex(const char* path1, const char* path2 = NULL)
{
    if (uiCheckFile(path1))
    {
        printf("[UI] Loaded HP bar texture: %s\n", path1);
        return iLoadImage((char*)path1);
    }
    if (path2 && uiCheckFile(path2))
    {
        printf("[UI] Loaded HP bar texture: %s\n", path2);
        return iLoadImage((char*)path2);
    }
    return 0;
}

inline void loadHealthBarAssets()
{
    
    g_playerHpBgTex   = loadUiTex("assets/ui/player_hp_bg.png",   "assets/player_hp_bg.png");
    g_playerHpFillTex = loadUiTex("assets/ui/player_hp_fill.png", "assets/player_hp_fill.png");

    
    g_enemyHpBgTex   = loadUiTex("assets/ui/enemy_hp_bg.png",   "assets/enemy_hp_bg.png");
    g_enemyHpFillTex = loadUiTex("assets/ui/enemy_hp_fill.png", "assets/enemy_hp_fill.png");

    
    if (g_playerHpBgTex == 0)   g_playerHpBgTex   = loadUiTex("assets/ui/hp_bg.png",   "assets/hp_bg.png");
    if (g_playerHpFillTex == 0) g_playerHpFillTex = loadUiTex("assets/ui/hp_fill.png", "assets/hp_fill.png");
    if (g_enemyHpBgTex == 0)    g_enemyHpBgTex    = loadUiTex("assets/ui/hp_bg.png",   "assets/hp_bg.png");
    if (g_enemyHpFillTex == 0)  g_enemyHpFillTex  = loadUiTex("assets/ui/hp_fill.png", "assets/hp_fill.png");
}


inline void drawClippedImage(double x, double y, double width, double height, double pct, unsigned int texture)
{
    if (texture == 0 || pct <= 0.0) return;
    if (pct > 1.0) pct = 1.0;

    double fillW = width * pct;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f((GLfloat)x, (GLfloat)y);

        glTexCoord2f((GLfloat)pct, 0.0f);
        glVertex2f((GLfloat)(x + fillW), (GLfloat)y);

        glTexCoord2f((GLfloat)pct, -1.0f);
        glVertex2f((GLfloat)(x + fillW), (GLfloat)(y + height));

        glTexCoord2f(0.0f, -1.0f);
        glVertex2f((GLfloat)x, (GLfloat)(y + height));
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


inline void drawHealthBar(double x, double y, double w, double h, double pct,
                          unsigned int bgTex, unsigned int fillTex,
                          int fallbackR = 200, int fallbackG = 40, int fallbackB = 40)
{
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;

    if (bgTex != 0 && fillTex != 0)
    {
        iShowImage((int)x, (int)y, (int)w, (int)h, bgTex);

        drawClippedImage(x, y, w, h, pct, fillTex);
    }
    else
    {
        iSetColor(40, 40, 40);
        iFilledRectangle(x, y, w, h);

        iSetColor(fallbackR, fallbackG, fallbackB);
        iFilledRectangle(x, y, w * pct, h);

        iSetColor(255, 255, 255);
        iRectangle(x, y, w, h);
    }
}

#endif
