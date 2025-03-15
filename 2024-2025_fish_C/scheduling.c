#include <stdbool.h>
#include "display.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "state.h"
#include "scheduling.h"

one_schedule currentSchedule = {0, 0};

/**
 * Set the hour via this function - this is for scheduling hours not clock
 */
void setHour(void) {
    MenuItem thisMenu[] = {{"Save", NULL}};
    genericMenuNUM(thisMenu, 1, 0, 24, false);
    currentSchedule.hour = getValue();
    makeSchedule();
}

/**
 * Set the minute via this function - this is for scheduling minutes not clock
 */
void setMinute(void) {
    MenuItem thisMenu[] = {{"Save", NULL}};
    genericMenuNUM(thisMenu, 1, 0, 59, false);
    currentSchedule.minute = getValue();
    makeSchedule();
}

/**
 * Set the number of rotations
 */
void setNumberOfRotations(void) {
    MenuItem thisMenu[] = {{"Save", NULL}};
    genericMenuNUM(thisMenu, 1, 1, 20, false);
    currentSchedule.rotations = getValue();
    makeSchedule();
}

/**
 * Save the schedule - this is more of adding it to the list of schedules
 */
void saveThisSchedule(void) {
    if(currentSchedule.rotations == 0) {
        currentSchedule.rotations = 1;
    }
    addScheduleToList(currentSchedule);
    schedulingMenu();
}

/**
 * Function for creating a schedule - user is able to set hour, minute, rotations and add to the list or quit
 */
void makeSchedule(void) {
    MenuItem thisMenu[] = {
        {"Set hour", setHour},
        {"Set minute", setMinute},
        {"Set rotations", setNumberOfRotations},
        {"Save Schedule", saveThisSchedule},
        {"Quit", NULL}
    };

    genericMenu(thisMenu, 4, false);
    schedulingMenu();
}
