


#ifndef BACKGROUND_NPC_H
#define BACKGROUND_NPC_H

#include "GameCommon.h"
#include <stdlib.h>
#include <stdio.h>


#define BG_NPC_MAX          10        
#define BG_NPC_TYPE_COUNT   3        
#define BG_NPC_WALK_FRAMES  2        


static const int g_npcWidths[BG_NPC_TYPE_COUNT]  = { 82, 78, 85 };
static const int g_npcHeights[BG_NPC_TYPE_COUNT] = { 96, 90, 100 };

#define BG_NPC_SPEED_MIN    3.0     
#define BG_NPC_SPEED_MAX    4.0


#define BG_NPC_Y_MIN  (GROUND_Y + 10)   
#define BG_NPC_Y_MAX  (GROUND_Y + 30)  


#define BG_NPC_SPAWN_MIN  (2 * TICKS_PER_SECOND)
#define BG_NPC_SPAWN_MAX  (5 * TICKS_PER_SECOND)

#define BG_NPC_ANIM_DELAY  8    


static unsigned int g_npcWalkTex[BG_NPC_TYPE_COUNT][BG_NPC_WALK_FRAMES];
static bool         g_npcAssetsLoaded = false;

struct BgNpc
{
    bool   active;
    int    type;       
    double x;          
    double y;          
    double speed;      
    int    facing;     
    int    frameIndex;
    int    frameTimer;
};

static BgNpc  g_bgNpcs[BG_NPC_MAX];
static int    g_npcSpawnTimer = 0;

static inline bool npcCheckFile(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fclose(f);
    return true;
}


inline void loadBgNpcAssets()
{
    if (g_npcAssetsLoaded) return;
    char path[128];

    for (int t = 0; t < BG_NPC_TYPE_COUNT; t++)
    {
        for (int i = 0; i < BG_NPC_WALK_FRAMES; i++)
        {
            g_npcWalkTex[t][i] = 0;

            
            sprintf(path, "assets/npc/npc_%d_walk_%d.png", t, i);
            if (npcCheckFile(path)) g_npcWalkTex[t][i] = iLoadImage(path);

            
            if (g_npcWalkTex[t][i] == 0)
            {
                sprintf(path, "assets/npc/npc%d_walk_%d.png", t, i);
                if (npcCheckFile(path)) g_npcWalkTex[t][i] = iLoadImage(path);
            }

            
            if (g_npcWalkTex[t][i] == 0)
            {
                sprintf(path, "assets/npc/type_%d/walk_%d.png", t, i);
                if (npcCheckFile(path)) g_npcWalkTex[t][i] = iLoadImage(path);
            }

            
            if (g_npcWalkTex[t][i] == 0 && t == 0)
            {
                sprintf(path, "assets/npc/npc_walk_%d.png", i);
                if (npcCheckFile(path)) g_npcWalkTex[t][i] = iLoadImage(path);
            }

            if (g_npcWalkTex[t][i] != 0)
                printf("[BgNPC] Loaded Type %d frame %d: %s\n", t, i, path);
        }
    }
    g_npcAssetsLoaded = true;

    
    for (int i = 0; i < BG_NPC_MAX; i++)
        g_bgNpcs[i].active = false;

    
    g_npcSpawnTimer = BG_NPC_SPAWN_MIN +
        rand() % (BG_NPC_SPAWN_MAX - BG_NPC_SPAWN_MIN + 1);
}


inline void spawnBgNpc(double cameraX)
{
    
    int slot = -1;
    for (int i = 0; i < BG_NPC_MAX; i++)
        if (!g_bgNpcs[i].active) { slot = i; break; }
    if (slot < 0) return;   

    BgNpc& n = g_bgNpcs[slot];
    n.active     = true;
    n.type       = rand() % BG_NPC_TYPE_COUNT;   
    n.frameIndex = 0;
    n.frameTimer = 0;
    n.speed      = BG_NPC_SPEED_MIN +
        (double)rand() / RAND_MAX * (BG_NPC_SPEED_MAX - BG_NPC_SPEED_MIN);
    n.y          = BG_NPC_Y_MIN +
        rand() % (BG_NPC_Y_MAX - BG_NPC_Y_MIN + 1);

    int w = g_npcWidths[n.type];

    
    if (rand() % 2 == 0)
    {
        
        n.facing = 1;
        n.x      = cameraX - w;
    }
    else
    {
        
        n.facing = -1;
        n.x      = cameraX + WINDOW_W + w;
    }
}


inline void resetBgNpcs()
{
    for (int i = 0; i < BG_NPC_MAX; i++)
        g_bgNpcs[i].active = false;
    g_npcSpawnTimer = BG_NPC_SPAWN_MIN +
        rand() % (BG_NPC_SPAWN_MAX - BG_NPC_SPAWN_MIN + 1);
}


inline void updateBgNpcs(double cameraX)
{
    
    g_npcSpawnTimer--;
    if (g_npcSpawnTimer <= 0)
    {
        spawnBgNpc(cameraX);
        g_npcSpawnTimer = BG_NPC_SPAWN_MIN +
            rand() % (BG_NPC_SPAWN_MAX - BG_NPC_SPAWN_MIN + 1);
    }

    
    for (int i = 0; i < BG_NPC_MAX; i++)
    {
        if (!g_bgNpcs[i].active) continue;
        BgNpc& n = g_bgNpcs[i];

        n.x += n.facing * n.speed;

        
        n.frameTimer++;
        if (n.frameTimer >= BG_NPC_ANIM_DELAY)
        {
            n.frameTimer = 0;
            n.frameIndex = (n.frameIndex + 1) % BG_NPC_WALK_FRAMES;
        }

        
        int w = g_npcWidths[n.type];
        double screenX = n.x - cameraX;
        if (n.facing == 1  && screenX > WINDOW_W + w * 2)
            n.active = false;
        if (n.facing == -1 && screenX < -w * 2)
            n.active = false;
    }
}


inline void drawBgNpcs(double cameraX)
{
    for (int i = 0; i < BG_NPC_MAX; i++)
    {
        if (!g_bgNpcs[i].active) continue;
        BgNpc& n = g_bgNpcs[i];

        int w = g_npcWidths[n.type];
        int h = g_npcHeights[n.type];

        double screenX = n.x - cameraX - w / 2.0;
        double screenY = n.y;

        unsigned int tex = g_npcWalkTex[n.type][n.frameIndex];

        if (tex != 0)
        {
            if (n.facing == 1)
            {
                iShowImage((int)screenX, (int)screenY, w, h, tex);
            }
            else
            {
                
                double cx = screenX + w / 2.0;
                glPushMatrix();
                glTranslatef((float)cx, 0.0f, 0.0f);
                glScalef(-1.0f, 1.0f, 1.0f);
                glTranslatef((float)-cx, 0.0f, 0.0f);
                iShowImage((int)screenX, (int)screenY, w, h, tex);
                glPopMatrix();
            }
        }
        else
        {
            
            if (n.type == 0)      iSetColor(70, 190, 210);  
            else if (n.type == 1) iSetColor(230, 160, 50);  
            else                  iSetColor(190, 80, 200);  

            iFilledRectangle(screenX, screenY, w, h);

           
            iSetColor(255, 255, 255);
            iRectangle(screenX, screenY, w, h);
        }
    }
}

#endif 
