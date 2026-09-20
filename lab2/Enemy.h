#ifndef ENEMY_H
#define ENEMY_H


#include "GameCommon.h"
#include "Projectile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ENEMY_CORPSE_TICKS     45    
#define ENEMY_ART_FACES_RIGHT  1     
#define ENEMY_BOMBER_ART_FACES_RIGHT  0   
enum EnemyType {
	ENEMY_SMALL, ENEMY_MEDIUM, ENEMY_LARGE,
	ENEMY_ARCHER, ENEMY_FLYER, ENEMY_SHIELD,
	ENEMY_FIRE, ENEMY_BOMBER, ENEMY_SUMMONER,
	ENEMY_TYPE_COUNT
};
enum NpcMode { NPC_PATROL, NPC_CHASE, NPC_ATTACK_MODE };

struct EnemyStats
{
	double scale;
	int    maxHealth;
	double moveSpeed;
	int    attackCooldown;
	int    attackDamage;
	int    detectRange;
	int    attackRange;
	bool   canJump;
	double jumpSpeed;
	double flyOffsetY;      
	double flyPhase;        
	int    shieldHP;        
	int    rangedCooldown;  
	int    summonCooldown;
	int    summonsLeft;
	bool   bombThrown;
};

inline EnemyStats getEnemyStats(EnemyType type, int day)
{
	EnemyStats s = EnemyStats();  

	double difficulty;
	switch (day)
	{
	case 1:  difficulty = 1.0;  break;
	case 2:  difficulty = 1.15; break;
	case 3:  difficulty = 1.3;  break;
	case 4:  difficulty = 1.5;  break;
	case 5:  difficulty = 1.8;  break;
	case 6:  difficulty = 2.0;  break;
	case 7:  difficulty = 2.2;  break;
	case 8:  difficulty = 2.45; break;
	case 9:  difficulty = 2.7;  break;
	default: difficulty = 3.0;  break;
	}
	switch (type)
	{
	case ENEMY_SMALL:
		s.scale = 2.0;
		s.maxHealth = (int)(60 * difficulty);
		s.moveSpeed = 3.0 + 0.15 * (day - 1);
		s.attackCooldown = 40 - 2 * (day - 1);
		if (s.attackCooldown < 20) s.attackCooldown = 20;
		s.attackDamage = (int)(4 * difficulty);
		s.detectRange = 260 + 10 * (day - 1);
		s.attackRange = 70;
		break;

	case ENEMY_MEDIUM:
		s.scale = 3.0;
		s.maxHealth = (int)(100 * difficulty);
		s.moveSpeed = 2.2 + 0.1 * (day - 1);
		s.attackCooldown = 55 - 2 * (day - 1);
		if (s.attackCooldown < 30) s.attackCooldown = 30;
		s.attackDamage = (int)(7 * difficulty);
		s.detectRange = 280 + 10 * (day - 1);
		s.attackRange = 85;
		break;

	case ENEMY_LARGE:
		s.scale = 4.0;
		s.maxHealth = (int)(150 * difficulty);
		s.moveSpeed = 1.5 + 0.07 * (day - 1);
		s.attackCooldown = 75 - 2 * (day - 1);
		if (s.attackCooldown < 45) s.attackCooldown = 45;
		s.attackDamage = (int)(12 * difficulty);
		s.detectRange = 300 + 10 * (day - 1);
		s.attackRange = 100;
		break;

	case ENEMY_ARCHER:
		s.scale = 2.4;
		s.maxHealth = 90;    
		s.moveSpeed = 1.2;
		s.attackCooldown = 90;
		s.attackDamage = (int)(6 * difficulty);
		s.detectRange = 460;
		s.attackRange = 55;             
		break;

	case ENEMY_FLYER:
		s.scale = 2.2;
		s.maxHealth = 120;   
		s.moveSpeed = 2.6 + 0.1 * (day - 1);
		s.attackCooldown = 45;
		s.attackDamage = (int)(6 * difficulty);
		s.detectRange = 420;
		s.attackRange = 75;
		s.flyOffsetY = 45;              
		break;

	case ENEMY_SHIELD:                   
		s.scale = 4.0;
		s.maxHealth = (int)(150 * difficulty);
		s.moveSpeed = 1.5 + 0.07 * (day - 1);
		s.attackCooldown = 75;
		s.attackDamage = (int)(12 * difficulty);
		s.detectRange = 300;
		s.attackRange = 100;
		break;

	case ENEMY_FIRE:
		s.scale = 2.8;
		s.maxHealth = (int)(70 * difficulty);
		s.moveSpeed = 1.0;
		s.attackCooldown = 110;
		s.attackDamage = (int)(8 * difficulty);
		s.detectRange = 380;
		s.attackRange = 55;
		break;

	case ENEMY_BOMBER:
		s.scale = 4.0;
		s.maxHealth = 240;   
		s.moveSpeed = 1.0;
		s.attackCooldown = 150;
		s.attackDamage = (int)(15 * difficulty);
		s.detectRange = 650;
		s.attackRange = 430;             
		break;

	case ENEMY_SUMMONER:
		s.scale = 4.5;
		s.maxHealth = (int)(240 * difficulty);
		s.moveSpeed = 1.6;
		s.attackCooldown = 70;
		s.attackDamage = (int)(11 * difficulty);
		s.detectRange = 620;
		s.attackRange = 110;
		break;
	}
	s.canJump = (day >= 4 && (type == ENEMY_SMALL || type == ENEMY_MEDIUM));
	s.jumpSpeed = 9.5;

	return s;
}



inline int specialEnemyLimit(EnemyType type, int day)
{
	if (type == ENEMY_ARCHER) return (day == 6) ? 1 : 0;
	if (type == ENEMY_FLYER)  return (day == 7) ? 1 : 0;
	if (type == ENEMY_SHIELD) return (day == 8) ? 1 : 0;
	if (type == ENEMY_BOMBER) return (day == 8) ? 1 : 0;
	return 99;
}



struct Enemy
{
	EnemyType    type;
	EnemyStats   stats;
	FighterState state;
	NpcMode      mode;

	double x, y, vy;
	double patrolMinX, patrolMaxX;
	int    patrolDir;
	int    facing;           

	int  health;
	bool alive;               
	bool onGround;
	bool isBoss;

	int  frameIndex, frameTimer;
	int  hurtTimer;
	int  attackTimer;         
	bool attackLanded;       
	int  deadTimer;

	
	int    shieldHP, shieldMax;
	double flyPhase;
	int    rangedCooldown;
	int    summonCooldown;
	int    summonsLeft;

	unsigned int idleTex[ANIM_IDLE_COUNT];
	unsigned int walkTex[ANIM_WALK_COUNT];
	unsigned int attackTex[ANIM_ATTACK_COUNT];
	unsigned int hurtTex[ANIM_HURT_COUNT];
	unsigned int deadTex[ANIM_DEAD_COUNT];
};

inline double enemyWidth(Enemy &e)  { return BASE_UNIT_W * e.stats.scale; }
inline double enemyHeight(Enemy &e) { return BASE_UNIT_H * e.stats.scale; }


inline void resetEnemy(Enemy &e, EnemyType type, double x, double minX, double maxX, int day)
{
	e.type = type;
	e.stats = getEnemyStats(type, day);
	e.state = IDLE;
	e.mode = NPC_PATROL;

	e.x = x;
	e.y = GROUND_Y;
	e.vy = 0;
	e.patrolMinX = minX;
	e.patrolMaxX = maxX;
	e.patrolDir = 1;
	e.facing = -1;

	e.health = e.stats.maxHealth;
	e.alive = true;
	e.onGround = true;
	e.isBoss = false;

	e.frameIndex = 0;
	e.frameTimer = 0;
	e.hurtTimer = 0;
	e.attackTimer = 20;
	e.attackLanded = false;
	e.deadTimer = 0;

	e.shieldHP = 0;
	e.shieldMax = 0;
	e.flyPhase = 0;
	e.rangedCooldown = 0;
	e.summonCooldown = 0;
	e.summonsLeft = 0;
}

inline void makeBoss(Enemy &e)
{
	e.isBoss = true;
	e.stats.maxHealth = (int)(e.stats.maxHealth * 1.3);
	e.health = e.stats.maxHealth;
	e.stats.attackDamage = (int)(e.stats.attackDamage * 1.15);
	if (e.type == ENEMY_LARGE) e.stats.scale = 4.6;   // the day-5 boss looks bigger
}



inline bool enemyFileExists(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (!f) return false;
	fclose(f);
	return true;
}

inline void makeEnemyFramePath(char *out, const char *folder, const char *name, int pattern, int i)
{
	if (pattern == 0)      sprintf(out, "assets/%s/%s_%d.png", folder, name, i);
	else if (pattern == 1) sprintf(out, "assets/%s/%s_%d.png", folder, name, i + 1);
	else                   sprintf(out, "assets/%s/%s_%02d.png", folder, name, i + 1);
}

inline void loadEnemyFrames(unsigned int *arr, int count, const char *folder, const char *name)
{
	char path[256];
	for (int i = 0; i < count; i++) arr[i] = 0;

	for (int pattern = 0; pattern < 3; pattern++)
	{
		makeEnemyFramePath(path, folder, name, pattern, 0);
		if (!enemyFileExists(path)) continue;

		for (int i = 0; i < count; i++)
		{
			makeEnemyFramePath(path, folder, name, pattern, i);
			if (enemyFileExists(path)) arr[i] = iLoadImage(path);
			else if (i > 0)            arr[i] = arr[i - 1];
		}
		return;
	}
}


inline const char* enemyFolder(EnemyType t)
{
	switch (t)
	{
	case ENEMY_SMALL:
	case ENEMY_ARCHER:
	case ENEMY_FLYER:
		return "enemy_small";
	case ENEMY_MEDIUM:
	case ENEMY_FIRE:
		return "enemy_medium";
	default:                       
		return "enemy_large";
	}
}

inline void loadEnemyAssets(Enemy &e)
{
	const char *folder = enemyFolder(e.type);

	if (e.type == ENEMY_BOMBER)
	{
		
		loadEnemyFrames(e.walkTex, ANIM_WALK_COUNT, folder, "moved");
		for (int i = 0; i < ANIM_IDLE_COUNT; i++) e.idleTex[i] = e.walkTex[i % ANIM_WALK_COUNT];
		for (int i = 0; i < ANIM_HURT_COUNT; i++) e.hurtTex[i] = e.walkTex[0];
		for (int i = 0; i < ANIM_DEAD_COUNT; i++) e.deadTex[i] = e.walkTex[0];

		char path[256];
		unsigned int prep = 0, rel = 0;
		sprintf(path, "assets/%s/prepare_1.png", folder);
		if (enemyFileExists(path)) prep = iLoadImage(path);
		sprintf(path, "assets/%s/release_01.png", folder);
		if (enemyFileExists(path)) rel = iLoadImage(path);

		e.attackTex[0] = prep;
		e.attackTex[1] = prep;
		e.attackTex[2] = rel ? rel : prep;
		return;
	}

	loadEnemyFrames(e.idleTex, ANIM_IDLE_COUNT, folder, "idle");
	loadEnemyFrames(e.walkTex, ANIM_WALK_COUNT, folder, "walk");
	loadEnemyFrames(e.attackTex, ANIM_ATTACK_COUNT, folder, "attack");
	loadEnemyFrames(e.hurtTex, ANIM_HURT_COUNT, folder, "hurt");
	loadEnemyFrames(e.deadTex, ANIM_DEAD_COUNT, folder, "dead");
}



inline void setEnemyState(Enemy &e, FighterState s)
{
	if (e.state == s) return;
	e.state = s;
	e.frameIndex = 0;
	e.frameTimer = 0;
}

inline void advanceEnemyAnim(Enemy &e, int frameCount)
{
	e.frameTimer++;
	if (e.frameTimer >= ANIM_FRAME_DELAY)
	{
		e.frameTimer = 0;
		e.frameIndex++;
		if (e.frameIndex >= frameCount) e.frameIndex = 0;
	}
}

inline HitBox getEnemyBox(Enemy &e)
{
	HitBox b;
	b.w = enemyWidth(e) * 0.6;
	b.h = enemyHeight(e) * 0.85;
	b.x = e.x - b.w / 2;
	b.y = e.y;
	return b;
}


inline double enemyStopDistance(Enemy &e)
{
	switch (e.type)
	{
	case ENEMY_ARCHER: return 330;
	case ENEMY_FIRE:   return 220;
	case ENEMY_BOMBER: return 300;
	default:           return e.stats.attackRange * 0.6;
	}
}



inline bool updateEnemy(Enemy &e, double playerX)
{
	if (!e.alive) return false;

	if (e.attackTimer > 0) e.attackTimer--;

	
	e.y += e.vy;
	e.vy -= GRAVITY;
	if (e.y <= GROUND_Y) { e.y = GROUND_Y; e.vy = 0; e.onGround = true; }
	else e.onGround = false;

	
	if (e.state == DEAD)
	{
		e.deadTimer++;
		e.frameTimer++;
		if (e.frameTimer >= ANIM_FRAME_DELAY)
		{
			e.frameTimer = 0;
			if (e.frameIndex < ANIM_DEAD_COUNT - 1) e.frameIndex++;
		}
		if (e.deadTimer >= ENEMY_CORPSE_TICKS) e.alive = false;
		return false;
	}

	
	if (e.state == HURT)
	{
		e.hurtTimer--;
		advanceEnemyAnim(e, ANIM_HURT_COUNT);
		if (e.hurtTimer <= 0) setEnemyState(e, IDLE);
		return false;
	}

	
	if (e.state == ATTACK)
	{
		advanceEnemyAnim(e, ANIM_ATTACK_COUNT);
		bool hit = false;

		if (!e.attackLanded && e.frameIndex == ANIM_ATTACK_COUNT - 1)
		{
			e.attackLanded = true;
			if (e.type == ENEMY_BOMBER)
				spawnBomb(e.x, e.y + enemyHeight(e) * 0.7, e.facing, e.stats.attackDamage,
				fabs(playerX - e.x), true);
			else
				hit = true;
		}
		else if (e.attackLanded && e.frameIndex == 0 && e.frameTimer == 0)
		{
			setEnemyState(e, IDLE);
			e.attackLanded = false;
			e.attackTimer = e.stats.attackCooldown;
		}
		return hit;
	}

	
	double dx = playerX - e.x;
	double dist = fabs(dx);
	bool moving = false;

	if (dist <= e.stats.detectRange)
	{
		e.mode = NPC_CHASE;
		e.facing = (dx >= 0) ? 1 : -1;

		if (dist > enemyStopDistance(e))
		{
			e.x += e.facing * e.stats.moveSpeed;
			moving = true;
		}

		if (e.stats.canJump && e.onGround && dist < 220 && dist > 60 && (rand() % 120) == 0)
		{
			e.vy = e.stats.jumpSpeed;
			e.onGround = false;
		}

		bool inRange;
		if (e.type == ENEMY_BOMBER) inRange = (dist <= e.stats.attackRange && dist >= 90);
		else                        inRange = (dist <= e.stats.attackRange * 0.7);

		if (e.attackTimer == 0 && inRange)
		{
			e.mode = NPC_ATTACK_MODE;
			setEnemyState(e, ATTACK);
			e.attackLanded = false;
			return false;
		}
	}
	else
	{
		e.mode = NPC_PATROL;
		e.x += e.patrolDir * e.stats.moveSpeed * 0.5;
		if (e.x > e.patrolMaxX) e.patrolDir = -1;
		if (e.x < e.patrolMinX) e.patrolDir = 1;
		e.facing = e.patrolDir;
		moving = true;
	}

	if (e.x < 40) e.x = 40;
	if (e.x > WORLD_W - 40) e.x = WORLD_W - 40;

	setEnemyState(e, moving ? WALK : IDLE);
	if (e.state == WALK) advanceEnemyAnim(e, ANIM_WALK_COUNT);
	else                 advanceEnemyAnim(e, ANIM_IDLE_COUNT);

	return false;
}



inline void applyDamageToEnemy(Enemy &e, int amount, int knockDir)
{
	if (!e.alive || e.state == DEAD) return;

	e.health -= amount;
	if (e.health <= 0)
	{
		e.health = 0;
		setEnemyState(e, DEAD);
		e.deadTimer = 0;
		return;
	}

	e.x += knockDir * (e.isBoss ? 4 : 14);
	if (e.x < 40) e.x = 40;
	if (e.x > WORLD_W - 40) e.x = WORLD_W - 40;

	if (!e.isBoss)                
	{
		setEnemyState(e, HURT);
		e.hurtTimer = HURT_DURATION;
		e.attackLanded = false;
	}
}


inline void applyDamageToEnemyWithShield(Enemy &e, int amount, int knockDir)
{
	if (!e.alive || e.state == DEAD) return;

	if (e.shieldHP > 0)
	{
		e.shieldHP -= amount;
		if (e.shieldHP < 0) e.shieldHP = 0;
		e.x += knockDir * 3;
		return;
	}
	applyDamageToEnemy(e, amount, knockDir);
}



inline unsigned int pickEnemyFrame(unsigned int *arr, int count, int idx)
{
	if (idx >= count) idx = count - 1;
	if (idx < 0) idx = 0;
	if (arr[idx] != 0) return arr[idx];
	for (int i = 0; i < count; i++)
	if (arr[i] != 0) return arr[i];
	return 0;
}

inline void drawOneWing(double ax, double ay, int dir, double len, double angleDeg,
	int r, int g, int b)
{
	double a = angleDeg * 3.14159265 / 180.0;
	double dx = dir * cos(a), dy = sin(a);   
	double nx = -dir * sin(a), ny = cos(a);    

	
	double U[6] = { 0.00, 0.20, 0.60, 1.00, 0.70, 0.30 };
	double V[6] = { 0.00, 0.28, 0.32, 0.05, -0.08, -0.10 };

	double px[6], py[6];
	for (int i = 0; i < 6; i++)
	{
		px[i] = ax + len * (U[i] * dx + V[i] * nx);
		py[i] = ay + len * (U[i] * dy + V[i] * ny);
	}

	iSetColor(r, g, b);
	iFilledPolygon(px, py, 6);
	iSetColor(15, 25, 55);
	iPolygon(px, py, 6);
	iLine(ax, ay, px[3], py[3]);               
}

inline void drawFlyerWings(Enemy &e, double cameraX)
{
	if (e.type != ENEMY_FLYER || !e.alive || e.state == DEAD) return;

	double w = enemyWidth(e);
	double h = enemyHeight(e);
	double screenX = e.x - cameraX;
	if (screenX < -w || screenX > WINDOW_W + w) return;

	int    dir = -e.facing;                   
	double flap = sin(e.flyPhase * 5.0);      
	double angle = 55.0 + flap * 25.0;         
	double len = h * 0.5;                     
	double ax = screenX + dir * w * 0.05;   
	double ay = e.y + h * 0.55;

	drawOneWing(ax, ay, dir, len * 0.85, angle - 22.0, 35, 60, 110);   
	drawOneWing(ax, ay, dir, len, angle, 60, 100, 165);  
}
inline void drawEnemy(Enemy &e, double cameraX)
{
	if (!e.alive) return;

	double w = enemyWidth(e);
	double h = enemyHeight(e);
	double screenX = e.x - cameraX;
	if (screenX < -w || screenX > WINDOW_W + w) return;

	unsigned int tex = 0;
	switch (e.state)
	{
	case IDLE:   tex = pickEnemyFrame(e.idleTex, ANIM_IDLE_COUNT, e.frameIndex); break;
	case WALK:   tex = pickEnemyFrame(e.walkTex, ANIM_WALK_COUNT, e.frameIndex); break;
	case ATTACK: tex = pickEnemyFrame(e.attackTex, ANIM_ATTACK_COUNT, e.frameIndex); break;
	case HURT:   tex = pickEnemyFrame(e.hurtTex, ANIM_HURT_COUNT, e.frameIndex); break;
	case DEAD:   tex = pickEnemyFrame(e.deadTex, ANIM_DEAD_COUNT, e.frameIndex); break;
	}
	if (tex == 0) tex = pickEnemyFrame(e.idleTex, ANIM_IDLE_COUNT, 0);

	if (tex != 0)
	{
		bool artFacesRight = (e.type == ENEMY_BOMBER) ? (ENEMY_BOMBER_ART_FACES_RIGHT != 0)
			: (ENEMY_ART_FACES_RIGHT != 0);
		bool flip = artFacesRight ? (e.facing < 0) : (e.facing > 0);
		if (!flip)
		{
			iShowImage(screenX - w / 2, e.y, (int)w, (int)h, tex);
		}
		else
		{
			glPushMatrix();
			glTranslatef((float)screenX, 0.0f, 0.0f);
			glScalef(-1.0f, 1.0f, 1.0f);
			glTranslatef((float)-screenX, 0.0f, 0.0f);
			iShowImage(screenX - w / 2, e.y, (int)w, (int)h, tex);
			glPopMatrix();
		}
	}
	else
	{
		switch (e.type)                       
		{
		case ENEMY_SMALL:    iSetColor(200, 60, 60);   break;
		case ENEMY_MEDIUM:   iSetColor(200, 120, 50);  break;
		case ENEMY_LARGE:    iSetColor(140, 40, 40);   break;
		case ENEMY_ARCHER:   iSetColor(120, 60, 160);  break;
		case ENEMY_FLYER:    iSetColor(60, 160, 200);  break;
		case ENEMY_SHIELD:   iSetColor(70, 90, 160);   break;
		case ENEMY_FIRE:     iSetColor(255, 110, 30);  break;
		case ENEMY_BOMBER:   iSetColor(90, 90, 90);    break;
		default:             iSetColor(160, 30, 120);  break;
		}
		iFilledRectangle(screenX - w / 2, e.y, w, h);
	}

	
	if (e.state != DEAD)
	{
		double bw = e.isBoss ? w * 0.9 : w * 0.6;
		double bx = screenX - bw / 2;
		double by = e.y + h + 6;
		double pct = (double)e.health / (double)e.stats.maxHealth;
		if (pct < 0) pct = 0;

		iSetColor(40, 40, 40);
		iFilledRectangle(bx, by, bw, 5);
		iSetColor(200, 40, 40);
		iFilledRectangle(bx, by, bw * pct, 5);
		iSetColor(255, 255, 255);
		iRectangle(bx, by, bw, 5);
	}
}


inline void drawEnemyShieldBar(Enemy &e, double cameraX)
{
	if (!e.alive || e.state == DEAD) return;
	if (e.shieldMax <= 0 || e.shieldHP <= 0) return;

	double w = enemyWidth(e);
	double h = enemyHeight(e);
	double screenX = e.x - cameraX;
	if (screenX < -w || screenX > WINDOW_W + w) return;

	double pct = (double)e.shieldHP / (double)e.shieldMax;

	
	double bw = w * 0.6, bx = screenX - bw / 2, by = e.y + h + 13;
	iSetColor(40, 40, 40);    iFilledRectangle(bx, by, bw, 5);
	iSetColor(80, 140, 255);  iFilledRectangle(bx, by, bw * pct, 5);
	iSetColor(255, 255, 255); iRectangle(bx, by, bw, 5);

	
	double sw = w * 0.24, sh = h * 0.34;
	double cx = screenX + e.facing * (w * 0.30);
	double cy = e.y + h * 0.42;

	double px[7], py[7];
	px[0] = cx - sw / 2;     py[0] = cy + sh / 2;
	px[1] = cx + sw / 2;     py[1] = cy + sh / 2;
	px[2] = cx + sw / 2;     py[2] = cy + sh * 0.05;
	px[3] = cx + sw * 0.25;  py[3] = cy - sh * 0.30;
	px[4] = cx;              py[4] = cy - sh / 2;
	px[5] = cx - sw * 0.25;  py[5] = cy - sh * 0.30;
	px[6] = cx - sw / 2;     py[6] = cy + sh * 0.05;

	iSetColor(70, 110, 200);
	iFilledPolygon(px, py, 7);
	iSetColor(230, 240, 255);
	iPolygon(px, py, 7);

	
	iLine(cx, cy - sh * 0.30, cx, cy + sh * 0.38);
	iLine(cx - sw * 0.3, cy + sh * 0.15, cx + sw * 0.3, cy + sh * 0.15);

	
	iSetColor(20, 30, 60);
	if (pct < 0.66) iLine(cx - sw * 0.4, cy + sh * 0.4, cx + sw * 0.1, cy - sh * 0.1);
	if (pct < 0.33) iLine(cx + sw * 0.4, cy + sh * 0.3, cx - sw * 0.2, cy - sh * 0.35);
}

#endif