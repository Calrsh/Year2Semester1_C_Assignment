#include "fish.h"
#include "display.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filehandling.h"
#include "state.h"
#include "scheduling.h"


/**
 * Placeholder function - this is used to not exit menus as our genericMenu will check our menuItem struct for NULL
 * values in place of wanting to exit the genericMenu loop.
 */
void doNothing() {
}


/**
 * This is our main menu - the first menu that should be displayed
 */
void mainDisplay(void) {
    char opMode[100] = "Curr Op Mode=";
    strcat(opMode, getOpState());
    char conversion[30];
    char feedSinceStart[100] = "No.feeds since auto=";
    sprintf(conversion, "%d", getNoFeedsSinceAuto());
    strcat(feedSinceStart, conversion);


    char nextFeedTime[100] = "Next Feed=";
    char conversionTime[30];
    sprintf(conversionTime, "%d:%d:0", getNextFeedTime().hour, getNextFeedTime().minute);
    strcat(nextFeedTime, conversionTime);


    const MenuItem mainMenu[] = {
        {nextFeedTime, doNothing},
        {feedSinceStart, doNothing},
        {opMode, doNothing},
        {"Config Menu Access", configMenu},
        {"Refresh page info", mainDisplay},
    };
    genericMenu(mainMenu, 5, true);
}

/**
 * This is a simple call to our displayText function
 * @param ourStruct is a pointer to the content struct we want to process
 */
void drawThis(const text_content *ourStruct) {
    displayText(ourStruct->x, ourStruct->y, ourStruct->text, ourStruct->size);
}

/**
 * This is like the drawThis method however it occurs for N times.
 * @param ourStruct is the main display elements struct we want to process. This will hold n amount of text content structs
 * @param amount is the amount of elements we wish to draw
 */
void drawThese(const main_display_elements *ourStruct, int amount) {
    displayColour("white", "black"); // foreground , background
    displayClear();
    for (int i = 0; i < amount; ++i) {
        text_content contentOfStruct = ourStruct->content[i];
        displayText(contentOfStruct.x, contentOfStruct.y, contentOfStruct.text, contentOfStruct.size);
    }
}

/**
 * Will lock the program out of doing anything else whilst it does a full food rotation.
 */
void oneFullRotation(void) {
    displayClear();
    drawThis(&(text_content){"-Feed in progress-", 0, 0, 1});
    drawThis(&(text_content){"-CYCLES LEFT-", 0, 10, 1});
    //max 1 rotation per 20 seconds
    for (int i = 360; i >= 0; --i) {
        char temp[15];
        sprintf(temp, "%3d", i);
        drawThis(&(text_content){temp, 0, 20, 1});
        motorStep();
        msleep(40L);
    }
    configMenu();
}

/**
 * Rotate for x amount of times rather than 1
 * @param amount the number of times to rotate
 */
void fullRotationPerAmount(int amount) {
    displayColour("white", "black");
    displayClear();
    drawThis(&(text_content){"-Feed in progress-", 0, 0, 1});
    drawThis(&(text_content){"-CYCLES LEFT-", 0, 10, 1});
    //max 1 rotation per 20 seconds
    for (int i = (360 * amount); i >= 0; --i) {
        char temp[10];
        sprintf(temp, "%3d", i);
        drawThis(&(text_content){temp, 0, 20, 1});
        motorStep();
        msleep(40L);
    }
    displayClear();
}

/**
 * This is used to highlight the current part of the menu the user has selected.
 * @param ourStruct are the elements that we want to process
 * @param itemCount is the amount of items in our menu
 */
void flashThis(const main_display_elements *ourStruct, int itemCount) {
    text_content test = ourStruct->content[ourStruct->currentSelected];
    displayColour("black", "white"); // foreground , background
    displayText(test.x, test.y, test.text, test.size);

    if (ourStruct->currentSelected == 0) {
        test = ourStruct->content[itemCount - 1];
        displayColour("white", "black"); // foreground , background
        displayText(test.x, test.y, test.text, test.size);
    } else {
        test = ourStruct->content[ourStruct->currentSelected - 1];
        displayColour("white", "black"); // foreground , background
        displayText(test.x, test.y, test.text, test.size);
    }
}


/**
 * This is used to display the clock element
 * @param second is an integer that we pass through - should be the second update
 */
void clockElement(int second) {
    // display graphic lines around time
    displayColour("white", "black");
    //displayLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1);
    //displayLine(0, SCREEN_HEIGHT - CHAR_HEIGHT * 2 - 1, SCREEN_WIDTH, SCREEN_HEIGHT - CHAR_HEIGHT * 2 - 1);
    // display the time value
    char time[LINE_SIZE];
    snprintf(time, LINE_SIZE, "%02i-%02i-%02i", clockHour(), clockMinute(), second);
    displayText(SCREEN_WIDTH / 2 - 4 * CHAR_WIDTH, SCREEN_HEIGHT - CHAR_HEIGHT * 1.5, time, 1);
}

/**
 * This is the schedule logic to be checked every 1/2 a second that the program is running.
 * If the mode is in auto it can commence an feed once the time is right. It will lock the user out of the program
 * and display the amount of cycles. It will then add the number of feeds since auto mode enabled, set the next feed time
 * then redraw the menu that the program is currently on.
 *
 * If the program is in paused mode. Reset the number of feeds since auto.
 *
 * If skip next. Do the same as auto but just abstract the rotating functionality.
 * @param menu are the menu items last displayed - needed for redraw
 * @param amount needed for redraw
 */
void scheduleLogic(main_display_elements menu, int amount) {
    char *opState = getOpState();
    if (strcmp(opState, "Auto") == 0) {
        const int hour = clockHour();
        const int minute = clockMinute();
        if (hour == getNextFeedTime().hour && minute == getNextFeedTime().minute) {
            fullRotationPerAmount(getNextFeedTime().rotations);
            addNoFeedsSinceAuto();
            setNextFeedTimeToNext();
            drawThese(&menu, amount);
        };
    } else if (strcmp(opState, "Paused") == 0) {
        resetNoFeedsSinceAuto();
    } else if (strcmp(opState,"Skip Next") == 0) {
        const int hour = clockHour();
        const int minute = clockMinute();
        if (hour > getNextFeedTime().hour && minute > getNextFeedTime().minute) {
            setNextFeedTimeToNext();
        }
    }
}


/**
 * This is the generic menu handler - all functions handling menus should be called to this
 * @param menuItem is an array that holds the text we want to display and the associated function if we call it
 * @param itemCount the amount of items in that array that are actually in use.
 * @param displaySecond is a boolean value that decides to display the clock element.
 */

void genericMenu(const MenuItem *menuItem, const int itemCount, const bool displaySecond) {
    // Set initial display colors
    displayColour("white", "black");


    char *result = NULL;
    main_display_elements menu;
    menu.currentSelected = 0;

    // Populate menu content
    for (int i = 0; i < itemCount; i++) {
        menu.content[i] = (struct TextContent){menuItem[i].text, 0, i * 10, 1};
    }

    // Initialize variables for blanking the display
    int timer = 0;
    bool blankDisplay = false;

    // Clear display and draw initial menu
    displayClear();
    drawThese(&menu, itemCount);
    displayColour("black", "white"); // Set foreground and background colors
    drawThis(&menu.content[0]);

    // Main loop flag
    bool flag = true;

    while (flag) {
        msleep(500L);

        scheduleLogic(menu, itemCount);

        // Update button state
        free(result);
        result = buttonState();

        // Handle timer for display blanking
        int second = clockSecond();
        int prev_sec = -1;
        if (second != prev_sec) {
            timer++;
            if (timer == 120) {
                blankDisplay = true;
                displayClear();
            }

            // Display time/date if enabled and display is not blank
            if (displaySecond && !blankDisplay) {
                clockElement(second);
            }
        }

        // Handle button presses
        if (strcmp(result, "SHORT_PRESS") == 0) {
            if (blankDisplay) {
                drawThese(&menu, itemCount);
                blankDisplay = false;
            }

            timer = 0;
            menu.currentSelected = (menu.currentSelected + 1) % itemCount;
            flashThis(&menu, itemCount);
        } else if (strcmp(result, "LONG_PRESS") == 0) {
            if (blankDisplay) {
                drawThese(&menu, itemCount);
                blankDisplay = false;
            }

            timer = 0;
            if (!menuItem[menu.currentSelected].action) {
                flag = false;
            } else {
                menuItem[menu.currentSelected].action();
            }
        }
    }

    // Clean up
    assert(result != NULL);
    free(result);
}

/**
 * This is the generic menu handler for number handling. Logic is reversed here so short presses == long presses.
 * This concerns the value - any function handling this should getValue() for whatever it is used for.
 * i.e. setting the time
 * @param menuItem are the menu items we wish to display
 * @param itemCount is the amount of items in said menu - we will be adding 3 to every instance
 * @param lowerBound the lowest value permissible
 * @param upperBound the highest value permissible
 */
void genericMenuNUM(const MenuItem *menuItem, const int itemCount, const int lowerBound, const int upperBound,
                    bool displaySecond) {
    resetValue();
    displayColour("white", "black");
    displayClear();

    char *result = NULL;
    main_display_elements menu;
    menu.currentSelected = 0;

    MenuItem mainMenu[10] = {
        {"<<<", decrementValue},
        {"", doNothing},
        {">>>", incrementValue}
    };

    for (int i = 0; i < itemCount; i++) {
        mainMenu[i + 3] = menuItem[i];
    }
    //<<<
    menu.content[0] = (struct TextContent){mainMenu[0].text, (SCREEN_WIDTH / 2 - 4) - CHAR_WIDTH * 4, 5, 1};
    //>>>
    menu.content[2] = (struct TextContent){mainMenu[2].text, (SCREEN_WIDTH / 2 + 1) + CHAR_WIDTH * 2, 5, 1};

    //This should always be a digit
    menu.content[1] = (struct TextContent){"0", (SCREEN_WIDTH / 2 - 4), 5, 1};


    for (int i = 3; i < itemCount + 3; i++) {
        menu.content[i] = (struct TextContent){mainMenu[i].text, 0, i * 5, 1};
    }
    bool flag = true;
    drawThese(&menu, itemCount + 3);
    displayText(0, SCREEN_HEIGHT-CHAR_HEIGHT, "Button logic reversed", 1);
    displayColour("black", "white"); // foreground , background
    drawThis(&menu.content[0]);
    int timer = 0;
    bool displayOn = true;

    while (flag) {
        msleep(500L);
        scheduleLogic(menu, itemCount + 3);


        int second = clockSecond();
        int prev_sec = -1;
        if (second != prev_sec) {
            timer++;
            if (timer == 120) {
                displayOn = false;
                displayClear();
            }

            // Display time/date if enabled and display is not blank
            if (displaySecond && !displayOn) {
                clockElement(second);
            }
        }

        free(result);
        result = buttonState();
        if (strcmp(result, "SHORT_PRESS") == 0) {
            if (!displayOn) {
                drawThese(&menu, itemCount + 3);
                displayOn = true;
            }
            timer = 0;
            if (!mainMenu[menu.currentSelected].action) {
                flag = false;
            } else {
                mainMenu[menu.currentSelected].action();
            }

            //Check bounds here
            if (getValue() < lowerBound) {
                incrementValue();
                //Wrap around for quality of life
                if (getValue() == lowerBound) {
                    setValue(upperBound);
                }
            } else if (getValue() > upperBound) {
                decrementValue();
                if (getValue() == upperBound) {
                    setValue(lowerBound);
                }
            }

            char conversion[5];
            sprintf(conversion, "%2d", getValue());
            menu.content[1].text = conversion;
            drawThis(&menu.content[1]);
        }
        if (strcmp(result, "LONG_PRESS") == 0) {
            if (!displayOn) {
                drawThese(&menu, itemCount + 3);
                displayOn = true;
            }
            timer = 0;
            menu.currentSelected++;
            if (menu.currentSelected == 1) {
                displayColour("white", "black");
                drawThis(&menu.content[0]);

                menu.currentSelected++;
            }
            if (menu.currentSelected == itemCount + 3) {
                menu.currentSelected = 0;
            }

            flashThis(&menu, itemCount + 3);
        }
    }
    assert(result != NULL);
    free(result);
}

/**
 * This is a part of a series of functions to change our static variables listed in the state - this changes the state to "Paused"
 */
void setOpModeToPaused(void) {
    setOpState("Paused");
    configMenu();
}

/**
 * This is a part of a series of functions to change our static variables listed in the state - this changes the state to "Auto"
 */
void setOpModeToAuto(void) {
    setOpState("Auto");
    configMenu();
}

/**
 * This is a part of a series of functions to change our static variables listed in the state - this changes the state to "Skip Next"
 */
void setOpModeToSkipNext(void) {
    setOpState("Skip Next");
    configMenu();
}

/**
 * This is our menu and function handler for the selection of the operating mode
 */
void selectOperatingMode(void) {
    const MenuItem temp[] = {
        {"Paused", setOpModeToPaused},
        {"Auto", setOpModeToAuto},
        {"Feed now", oneFullRotation},
        {"Skip next feed ", setOpModeToSkipNext},
        {"Quit", NULL}
    };
    genericMenu(temp, 5, false);
    configMenu();
}

//remove for warm clock start
void setClockWithWarmStart(void) {
    time_t now = clockWarmStart(0);
    struct tm *timeinfo = localtime(&now);
    clockYear(timeinfo->tm_year);
    clockMonth(timeinfo->tm_mon);
    clockHour(timeinfo->tm_hour);
    clockMinute(timeinfo->tm_min);
    clockSecond(timeinfo->tm_sec);
    clockSet(timeinfo->tm_sec, timeinfo->tm_min, timeinfo->tm_hour, timeinfo->tm_yday, timeinfo->tm_mon,
             timeinfo->tm_year);
    free(timeinfo);
}

void setHourManually(void) {
    MenuItem thisMenu[] = {{"Save", NULL}};
    genericMenuNUM(thisMenu, 1, 0, 24, true);
    clockSet(0, clockMinute(), getValue(), clockDay(), clockMonth(), clockYear());
    setClockManually();
}

void setMinuteManually(void) {
    MenuItem thisMenu[] = {{"Save", NULL}};
    genericMenuNUM(thisMenu, 1, 0, 59, true);
    clockSet(0, getValue(), clockHour(), clockDay(), clockMonth(), clockYear());
    setClockManually();
}

void setClockManually(void) {
    const MenuItem thisMenu[] = {
        {"Set hour", setHourManually},
        {"Set minute", setMinuteManually},
        {"Quit", NULL}
    };
    genericMenu(thisMenu, 3, true);
    saveSchedulesToFile();
    setClockMenu();
}

/**
 * This is our menu and function handler for the setting of the clock element
 */
void setClockMenu(void) {
    const MenuItem mainMenu[] = {
        {"Set clock to current", setClockWithWarmStart},
        {"Set manually", setClockManually},
        {"QUIT", NULL}
    };
    genericMenu(mainMenu, 3, false);
    configMenu();
}

void schedulingMenu() {
    MenuItem thisMenu[] = {
        {"Make Schedule", makeSchedule,},
        {"Delete Schedule", NULL},
        {"Save to file", saveSchedulesToFile},
        {"QUIT", NULL}
    };
    genericMenu(thisMenu, 4, false);
    mainDisplay();
}

/**
 * This is our menu and function handler for the configuration menu
 */
void configMenu(void) {
    const MenuItem mainMenu[] = {
        {"Set Clock", setClockMenu},
        {"Config Feed Schedule", schedulingMenu},
        {"Select Operating Mode", selectOperatingMode}, // No action for this option yet
        {"QUIT", NULL}
    };

    genericMenu(mainMenu, 4, false);
    mainDisplay();
}

/**
 * This us used for first setting up the clock.
 */
void clockSetUp(void) {
    displayClear();
    clockSet(0, 0, 14, 25, 05, 1968);

}

void startMenu() {
    fflush(stdout);
    setOpState("Auto");
    resetNoFeedsSinceAuto();

    displayText(0, 0, "Press to Start", 1);
    char *result = NULL;
    bool flag = true;
    while (flag) {
        msleep(500L);
        free(result);
        result = buttonState();
        if (strcmp(result, "SHORT_PRESS") == 0 || strcmp(result, "LONG_PRESS") == 0) {
            flag = false;
        }
    }
    free(result);
    mainDisplay();
}
