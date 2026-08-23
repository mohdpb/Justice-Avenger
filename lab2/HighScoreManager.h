
#ifndef HIGHSCORE_MANAGER_H
#define HIGHSCORE_MANAGER_H

#include <stdio.h>
#include <string.h>

#define MAX_HIGHSCORES 5
#define MAX_NAME_LEN   20

struct HighScoreList
{
    char names[MAX_HIGHSCORES][MAX_NAME_LEN];
    int count;
};

inline void loadHighScores(HighScoreList &list)
{
    FILE *f = fopen("highscores.dat", "rb");
    if (!f) { list.count = 0; return; }
    fread(&list, sizeof(HighScoreList), 1, f);
    fclose(f);
}

inline void saveHighScores(HighScoreList &list)
{
    FILE *f = fopen("highscores.dat", "wb");
    if (!f) return;
    fwrite(&list, sizeof(HighScoreList), 1, f);
    fclose(f);
}


inline void addHighScore(HighScoreList &list, const char name[])
{
    if (list.count < MAX_HIGHSCORES)
    {
        strncpy(list.names[list.count], name, MAX_NAME_LEN - 1);
        list.names[list.count][MAX_NAME_LEN - 1] = '\0';
        list.count++;
    }
    else
    {
      
        for (int i = 1; i < MAX_HIGHSCORES; i++)
            strcpy(list.names[i - 1], list.names[i]);
        strncpy(list.names[MAX_HIGHSCORES - 1], name, MAX_NAME_LEN - 1);
        list.names[MAX_HIGHSCORES - 1][MAX_NAME_LEN - 1] = '\0';
    }
    saveHighScores(list);
}

#endif 
