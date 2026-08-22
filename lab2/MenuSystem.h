#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "GameCommon.h"
#include <string.h>

#define MAX_BUTTON_LABEL 64

struct Button
{
    double x, y, w, h;
    char label[MAX_BUTTON_LABEL];
};

inline Button makeButton(double x, double y, double w, double h, const char *label)
{
    Button b;
    b.x = x; b.y = y; b.w = w; b.h = h;
    strncpy(b.label, label, MAX_BUTTON_LABEL - 1);
    b.label[MAX_BUTTON_LABEL - 1] = '\0';
    return b;
}

inline bool isPointInButton(const Button &b, int mx, int my)
{
    return (mx >= b.x && mx <= b.x + b.w && my >= b.y && my <= b.y + b.h);
}


inline int getClickedButtonIndex(Button buttons[], int count, int mx, int my)
{
    for (int i = 0; i < count; i++)
        if (isPointInButton(buttons[i], mx, my))
            return i;
    return -1;
}

inline void drawCenteredText(double centerX, double centerY, const char *text, void *font)
{
    int len = (int)strlen(text);
    double approxCharWidth = 10.0;
    double textX = centerX - (len * approxCharWidth) / 2.0;
    double textY = centerY - 6;
    iText(textX, textY, (char*)text, font);
}

inline void drawButton(const Button &b, bool hovered)
{
    if (hovered) iSetColor(150, 150, 150);
    else iSetColor(105, 105, 105);
    iFilledRectangle(b.x, b.y, b.w, b.h);

    iSetColor(230, 230, 230);
    iRectangle(b.x, b.y, b.w, b.h);

    iSetColor(255, 255, 255);
    drawCenteredText(b.x + b.w / 2, b.y + b.h / 2, b.label, GLUT_BITMAP_HELVETICA_18);
}

inline void drawDimOverlay()
{
    iSetColor(10, 10, 15);
    iFilledRectangle(0, 0, WINDOW_W, WINDOW_H);
}

inline void drawBackgroundImage(unsigned int tex)
{
    if (tex != 0)
        iShowImage(0, 0, WINDOW_W, WINDOW_H, tex);
    else
    {
        iSetColor(25, 25, 40);
        iFilledRectangle(0, 0, WINDOW_W, WINDOW_H);
    }
}

#endif 
