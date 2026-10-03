
#ifndef HIGHSCORE_MANAGER_H
#define HIGHSCORE_MANAGER_H

#include <stdio.h>
#include <string.h>

#define MAX_HIGHSCORES 5
#define MAX_NAME_LEN   20

struct HighScoreList
{
    char names[MAX_HIGHSCORES][MAX_NAME_LEN];
    int scores[MAX_HIGHSCORES];
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


inline void addHighScore(HighScoreList &list, const char name[], int score)
{
    int insertAt = list.count < MAX_HIGHSCORES ? list.count : -1;

    if (insertAt == -1)
    {
        if (score <= list.scores[MAX_HIGHSCORES - 1]) return;  
        insertAt = MAX_HIGHSCORES - 1;
    }
    else
    {
        list.count++;
    }

    while (insertAt > 0 && list.scores[insertAt - 1] < score)
    {
        list.scores[insertAt] = list.scores[insertAt - 1];
        strcpy(list.names[insertAt], list.names[insertAt - 1]);
        insertAt--;
    }

    list.scores[insertAt] = score;
    strncpy(list.names[insertAt], name, MAX_NAME_LEN - 1);
    list.names[insertAt][MAX_NAME_LEN - 1] = '\0';

    saveHighScores(list);
}


inline void addHighScore(HighScoreList &list, const char name[])
{
    addHighScore(list, name, 0);
}

#endif 
