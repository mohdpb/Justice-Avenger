//
//  PowerUp.h
//  Justice Avenger - floating HP Recover and Damage Boost items
//
//  ============================================================
//  ASSET SETUP
//  ============================================================
//      assets/powerups/heal.png
//      assets/powerups/damage.png
//
//  Both should be small transparent PNGs (roughly square).
//  ============================================================

#ifndef POWERUP_H
#define POWERUP_H

#include "GameCommon.h"
#include <stdlib.h>

enum PowerUpType { POWERUP_HEAL, POWERUP_DAMAGE };

#define POWERUP_SIZE 34

// Only one power-up is ever on the ground at a time. Once it's picked
// up (or its lifetime runs out unclaimed), a new random delay - up to
// 15 seconds - is rolled before the next one appears.
#define POWERUP_MIN_SPAWN_TICKS (5  * TICKS_PER_SECOND)
#define POWERUP_MAX_SPAWN_TICKS (15 * TICKS_PER_SECOND)
#define POWERUP_LIFETIME_TICKS  (10 * TICKS_PER_SECOND)

#define POWERUP_HEAL_AMOUNT        20
#define POWERUP_DAMAGE_BONUS       6
#define POWERUP_DAMAGE_DURATION_TICKS (8 * TICKS_PER_SECOND)

struct PowerUp
{
    bool active;
    PowerUpType type;
    double x, y;
    int lifeTimer;         // ticks left before it disappears if not picked up
    int nextSpawnTimer;    // ticks left until the next one is allowed to spawn

    unsigned int healTex;
    unsigned int damageTex;
};

inline void loadPowerUpAssets(PowerUp &p)
{
    p.healTex   = iLoadImage("assets/powerups/heal.png");
    p.damageTex = iLoadImage("assets/powerups/damage.png");
}

// Call once per day/run reset.
inline void resetPowerUp(PowerUp &p)
{
    p.active = false;
    p.lifeTimer = 0;
    p.nextSpawnTimer = POWERUP_MIN_SPAWN_TICKS +
        (rand() % (POWERUP_MAX_SPAWN_TICKS - POWERUP_MIN_SPAWN_TICKS + 1));
}

inline void spawnPowerUp(PowerUp &p)
{
    p.active = true;
    p.type = (rand() % 2 == 0) ? POWERUP_HEAL : POWERUP_DAMAGE;
    p.x = 120 + rand() % (WORLD_W - 240);   // anywhere in the scrollable level
    p.y = GROUND_Y;
    p.lifeTimer = POWERUP_LIFETIME_TICKS;
}

inline HitBox getPowerUpBox(PowerUp &p)
{
    HitBox b;
    b.w = POWERUP_SIZE;
    b.h = POWERUP_SIZE;
    b.x = p.x - b.w / 2;
    b.y = p.y;
    return b;
}

// Called every tick during gameplay. Handles the spawn countdown and
// the on-ground lifetime countdown.
inline void updatePowerUp(PowerUp &p)
{
    if (p.active)
    {
        p.lifeTimer--;
        if (p.lifeTimer <= 0)
        {
            // expired unclaimed - reset the countdown to the next one
            p.active = false;
            p.nextSpawnTimer = POWERUP_MIN_SPAWN_TICKS +
                (rand() % (POWERUP_MAX_SPAWN_TICKS - POWERUP_MIN_SPAWN_TICKS + 1));
        }
    }
    else
    {
        p.nextSpawnTimer--;
        if (p.nextSpawnTimer <= 0)
            spawnPowerUp(p);
    }
}

// cameraX shifts world position to screen position, same as drawPlayer/drawEnemy.
inline void drawPowerUp(PowerUp &p, double cameraX)
{
    if (!p.active) return;

    double screenX = p.x - cameraX;
    unsigned int tex = (p.type == POWERUP_HEAL) ? p.healTex : p.damageTex;
    if (tex != 0)
    {
        iShowImage(screenX - POWERUP_SIZE / 2, p.y, POWERUP_SIZE, POWERUP_SIZE, tex);
    }
    else
    {
        // Placeholder: green circle = heal, gold circle = damage boost
        if (p.type == POWERUP_HEAL) iSetColor(60, 220, 90);
        else iSetColor(230, 180, 40);
        iFilledCircle(screenX, p.y + POWERUP_SIZE / 2, POWERUP_SIZE / 2);
    }
}

#endif // POWERUP_H
