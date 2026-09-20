#ifndef PROJECTILE_H
#define PROJECTILE_H


#define FIREBALL_USE_PICTURES  0   


#include "iGraphics.h"
#include "GameCommon.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define BOMB_USE_PICTURES   0

#define MAX_PROJECTILES     64
#define PROJ_GRAVITY        0.55    
#define BOMB_FUSE_TICKS     70
#define BLAST_TICKS         26
#define BLAST_RADIUS        70.0

enum ProjKind { PROJ_ARROW, PROJ_FIREBALL, PROJ_BOMB };

struct Projectile
{
	bool   active;
	bool   fromEnemy;       
	bool   exploding;
	bool   hasHit;          
	ProjKind kind;

	double x, y;           
	double vx, vy;
	double w, h;
	int    facing;          
	int    damage;
	int    life;           
	int    fuse;           
	int    explodeTimer;
	int    animTimer;
	int    animFrame;
};

Projectile projectiles[MAX_PROJECTILES];

unsigned int arrowTex = 0;
unsigned int fireballTex = 0;
unsigned int bombTex[5] = { 0, 0, 0, 0, 0 };  
unsigned int blastTex = 0;



inline void loadProjectileAssets()
{
	arrowTex = iLoadImage("assets/projectiles/arrow.png");
	fireballTex = iLoadImage("assets/projectiles/fireball.png");

#if BOMB_USE_PICTURES
	blastTex = iLoadImage("assets/enemy_large/exploision_01.png");

	for (int i = 0; i < 5; i++)
	{
		char fn[128];
		sprintf(fn, "assets/enemy_large/bom_%02d.png", i + 2);
		bombTex[i] = iLoadImage(fn);
	}
#endif
}

inline void clearProjectiles()
{
	for (int i = 0; i < MAX_PROJECTILES; i++)
		projectiles[i].active = false;
}

inline Projectile* getFreeProjectile()
{
	for (int i = 0; i < MAX_PROJECTILES; i++)
	if (!projectiles[i].active) return &projectiles[i];
	return 0;   
}

inline void initProjectile(Projectile &p, ProjKind kind, double x, double y,
	int facing, int damage, bool fromEnemy)
{
	p.active = true;
	p.fromEnemy = fromEnemy;
	p.exploding = false;
	p.hasHit = false;
	p.kind = kind;
	p.x = x;
	p.y = y;
	p.facing = (facing >= 0) ? 1 : -1;
	p.damage = damage;
	p.vx = 0;
	p.vy = 0;
	p.fuse = BOMB_FUSE_TICKS;
	p.explodeTimer = 0;
	p.animTimer = 0;
	p.animFrame = 0;
	p.life = 300;
}

inline void spawnArrow(double x, double y, int facing, int damage, bool fromEnemy = true)
{
	Projectile *p = getFreeProjectile();
	if (!p) return;
	initProjectile(*p, PROJ_ARROW, x, y, facing, damage, fromEnemy);
	p->w = 34; p->h = 10;
	p->vx = 9.0 * p->facing;
	p->vy = 0;
	p->life = 200;
}

inline void spawnFireball(double x, double y, int facing, int damage, bool fromEnemy = true)
{
	Projectile *p = getFreeProjectile();
	if (!p) return;
	initProjectile(*p, PROJ_FIREBALL, x, y, facing, damage, fromEnemy);
	p->w = 28; p->h = 28;
	p->vx = 5.5 * p->facing;
	p->vy = 0;
	p->life = 240;
}



inline void spawnBomb(double x, double y, int facing, int damage,
	double targetDist, bool fromEnemy = true)
{
	Projectile *p = getFreeProjectile();
	if (!p) return;
	initProjectile(*p, PROJ_BOMB, x, y, facing, damage, fromEnemy);
	p->w = 36; p->h = 36;

	if (targetDist < 80)  targetDist = 80;
	if (targetDist > 420) targetDist = 420;

	double flightTicks = 55.0;
	p->vx = (targetDist / flightTicks) * p->facing;
	p->vy = 0.5 * PROJ_GRAVITY * flightTicks;   
	p->life = 400;
}

inline void startExplosion(Projectile &p)
{
	p.exploding = true;
	p.hasHit = false;
	p.explodeTimer = BLAST_TICKS;
	p.vx = p.vy = 0;
	p.w = p.h = BLAST_RADIUS * 2;
}

inline void updateProjectiles()
{
	for (int i = 0; i < MAX_PROJECTILES; i++)
	{
		Projectile &p = projectiles[i];
		if (!p.active) continue;

		if (p.exploding)
		{
			p.explodeTimer--;
			if (p.explodeTimer <= 0) p.active = false;
			continue;
		}

		p.x += p.vx;
		p.y += p.vy;

		if (p.kind == PROJ_BOMB)
		{
			p.vy -= PROJ_GRAVITY;

		
			p.animTimer++;
			if (p.animTimer >= 4) { p.animTimer = 0; p.animFrame = (p.animFrame + 1) % 5; }

			if (p.y <= GROUND_Y)          
			{
				p.y = GROUND_Y;
				p.vy = 0;
				p.vx *= 0.45;
			}
			p.fuse--;
			if (p.fuse <= 0) { startExplosion(p); continue; }
		}

		if (p.kind == PROJ_FIREBALL)
		{
			p.animTimer++;
			if (p.animTimer >= 5) { p.animTimer = 0; p.animFrame = (p.animFrame + 1) % 4; }
		}

		p.life--;
		if (p.life <= 0 || p.x < -50 || p.x > WORLD_W + 50)
		{
			if (p.kind == PROJ_BOMB) startExplosion(p);
			else                     p.active = false;
		}
	}
}


inline HitBox getProjectileBox(Projectile &p)
{
	HitBox b;
	b.x = p.x - p.w / 2;
	b.y = p.y;
	b.w = p.w;
	b.h = p.h;
	return b;
}

inline void drawProjectiles(double cameraX)
{
	for (int i = 0; i < MAX_PROJECTILES; i++)
	{
		Projectile &p = projectiles[i];
		if (!p.active) continue;

		double sx = p.x - cameraX;
		if (sx < -250 || sx > WINDOW_W + 250) continue;

	
		if (p.exploding)
		{
			double r = BLAST_RADIUS * (1.0 - (double)p.explodeTimer / BLAST_TICKS) + 20;
			if (BOMB_USE_PICTURES && blastTex != 0)
			{
				double bh = r * 2.0;
				double bw = bh * (281.0 / 196.0);     
				iShowImage(sx - bw / 2, p.y - bh * 0.15, (int)bw, (int)bh, blastTex);
			}
			else
			{
				iSetColor(255, 120, 30);
				iFilledCircle(sx, p.y + 10, r);
				iSetColor(255, 200, 60);
				iFilledCircle(sx, p.y + 10, r * 0.65);
				iSetColor(255, 250, 200);
				iFilledCircle(sx, p.y + 10, r * 0.3);
			}
			continue;
		}

		
		if (p.kind == PROJ_BOMB && !(BOMB_USE_PICTURES && bombTex[p.animFrame % 5] != 0))
		{
			double cx = sx, cy = p.y + p.h / 2, r = p.w / 2;
			iSetColor(35, 35, 40);
			iFilledCircle(cx, cy, r);
			iSetColor(200, 200, 210);
			iCircle(cx, cy, r);
			iSetColor(150, 110, 60);
			iLine(cx + r * 0.5, cy + r * 0.8, cx + r * 0.9, cy + r * 1.3);
			if (p.animFrame % 2 == 0) iSetColor(255, 200, 40); else iSetColor(255, 90, 20);
			iFilledCircle(cx + r * 0.9, cy + r * 1.3, 4);
			continue;
		}
		
		if (p.kind == PROJ_FIREBALL && !(FIREBALL_USE_PICTURES && fireballTex != 0))
		{
			double cx = sx, cy = p.y + p.h / 2, r = p.w / 2;
			double flick = (p.animFrame % 2 == 0) ? 2.0 : 0.0;   

			iSetColor(255, 120, 30);                              
			iFilledCircle(cx - p.facing * r * 1.3, cy, r * 0.55);
			iSetColor(255, 90, 20);
			iFilledCircle(cx, cy, r + flick);
			iSetColor(255, 170, 40);
			iFilledCircle(cx, cy, r * 0.7);
			iSetColor(255, 240, 150);
			iFilledCircle(cx, cy, r * 0.35);
			continue;
		}
		unsigned int tex = 0;
		if (p.kind == PROJ_ARROW)         tex = arrowTex;
		else if (p.kind == PROJ_FIREBALL) tex = fireballTex;
		else                              tex = bombTex[p.animFrame % 5];

		if (tex != 0)
		{
			
			if (p.facing < 0 && p.kind == PROJ_ARROW)
				iShowImage(sx + p.w / 2, p.y, (int)(-p.w), (int)p.h, tex);
			else
				iShowImage(sx - p.w / 2, p.y, (int)p.w, (int)p.h, tex);
		}
		else
		{
			if (p.kind == PROJ_ARROW) iSetColor(220, 220, 200);
			else                      iSetColor(255, 120, 30);
			iFilledRectangle(sx - p.w / 2, p.y, p.w, p.h);
		}
	}
}

#endif