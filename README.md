# Justice Avenger

## Game Description

Justice Avenger is a 2D side-scrolling survival brawler built using the iGraphics library in C/C++. A village has fallen under raid by Destroyers, and you play the Protector — the village's last line of defense — fighting through 8 terrific days of combat to drive them out.

## Features
- 8-day survival campaign with escalating difficulty and a unique enemy wave each day
- Small, medium, and large Destroyers, plus special enemy types: archers, flyers, shield-bearers, fire enemies, bombers, and a boss-tier Summoner
- A climactic boss fight closing out the campaign
- Melee combat, plus an unlockable fireball ability from Day 5 onward
- Power-ups: health recovery and temporary damage boosts
- Animated background villager NPCs that react to nearby combat
- Full menu system: story intro, pause menu, save/continue, audio settings, and a high score leaderboard
- Background music and sound effects for menus, combat, and key story beats
- Save/load system (progress, audio settings, and high scores all persist between sessions)

## Project Details
IDE: Visual Studio 2010/2013

Language: C, C++

Platform: Windows PC

Library: iGraphics (built on OpenGL/GLUT)

Genre: 2D side-scrolling action/survival brawler

## How to Run the Project

Make sure you have the following installed:
- **Visual Studio 2013**
- **MinGW Compiler** (if needed)
- **iGraphics Library** (included in this repository)

Open the project in Visual Studio 2013
- Open Visual Studio 2013.
- Go to File → Open → Project/Solution.
- Locate and select the .sln file from the cloned repository.
- Click Build → Build Solution
- Run the program by clicking Debug → Start Without Debugging

## How to Play

### Controls
| Action | Key |
| Move Left | `A` |
| Move Right | `D` |
| Jump | `W` |
| Punch | `J` |
| Fireball (unlocked Day 5+) | `K` |
| Pause | `ESC` |

### Game Rules
- You start each day with full health and a limited number of lives.
- Punching deals base damage to whichever enemy is closest and in range; a landed power-up can temporarily boost that damage.
- From Day 5, fireballs become available with limited ammo per day and deal heavier damage at range.
- Every enemy type has its own health, damage, and attack pattern, and difficulty increases with each day.
- Losing all your health costs a life; losing all your lives ends the game.
- Clearing every enemy on a day advances you to the next one. Defeating the final boss on Day 8 completes the game and lets you add your score to the High Scores leaderboard.

## Project Contributors

1. Mohammad Plabon
2. Shulogno Imteaz Prithy
3. Ifrat Zamin Roza

