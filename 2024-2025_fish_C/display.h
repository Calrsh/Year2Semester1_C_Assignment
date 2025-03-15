#pragma once

#include <stdbool.h>

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LINE_SIZE 80



typedef struct TextContent {
    char *text;
    int x;
    int y;
    int size; // can only be 1 or 2
} text_content;

typedef struct mainDisplayElements {
    int currentSelected;
    text_content content[8];
} main_display_elements;

typedef struct menuItems {
    char *text;
    void (*action)(void); // function pointer
} MenuItem;

void clockSetUp();

void flashThis();

void configMenu();

void selectOperatingMode();

void genericMenuNUM (const MenuItem *menuItem, const int itemCount, const int lowerBound, const int upperBound, bool displayTime);
void genericMenu(const MenuItem *menuItem, const int itemCount, const bool displaySecond);
void schedulingMenu();
void startMenu();
void setClockMenu();
void setClockManually();
void setClockWithWarmStart();
