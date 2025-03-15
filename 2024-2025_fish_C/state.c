#include "state.h"
#include <stdio.h>
#include <stdlib.h>
#include "fish.h"

static char *opState = "N/A";
static int noFeedsSinceAuto = 0;
static int value = 0;
static int numberOfSchedules = 0;
static list_schedules scheduleList;
static int nextFeedPosition = 0;
static one_schedule nextFeedTime = {0, 0, 0};
static long long warmStartNumber = 0;

void setWarmStartNumber(long long input) {
    warmStartNumber = input;
}

long long getWarmStartNumber() {
    return warmStartNumber;
}

void setNextFeedTimeToNext() {
    nextFeedPosition++;
    if (nextFeedPosition >= scheduleList.sizeOfList) {
        nextFeedPosition = 0;
    }
    nextFeedTime = getScheduleFromList(nextFeedPosition);
}

/**
 * This will initialise the first feed time variable - this also runs a number of checks on the time in order for the
 * next scheduled feed time to be accessible by the program.
 */
void initializeFirstFeedTime() {
    if (scheduleList.sizeOfList != 0) {
        nextFeedTime = getScheduleFromList(0);
        int totalMinutes1 = nextFeedTime.hour * 60 + nextFeedTime.minute;
        int totalMinutes2 = clockHour() * 60 + clockMinute();
        while (totalMinutes1 < totalMinutes2) {
            setNextFeedTimeToNext();
            totalMinutes1 = nextFeedTime.hour * 60 + nextFeedTime.minute;
            totalMinutes2 = clockHour() * 60 + clockMinute();
            if (nextFeedPosition == 0) {
                break;
            }
        }
    }
}

one_schedule getNextFeedTime() {
    return nextFeedTime;
}


/**
 * Set the operating state.
 * @param state is a char* which we want to set the operating state
 */
void setOpState(char *state) {
    opState = state;
}

/**
 * Return the operating state as a char*
 */
char *getOpState() {
    return opState;
}

/**
 * Currently increments the number of feeds since auto by 1
 * Possibly change this to a number of add another function to reset to 0
 */
void addNoFeedsSinceAuto() {
    noFeedsSinceAuto++;
}

/**
 * Return the number of feeds since auto
 */
int getNoFeedsSinceAuto() {
    return noFeedsSinceAuto;
}


/**
 * Sets the number of feeds since auto to 0
 */
void resetNoFeedsSinceAuto() {
    noFeedsSinceAuto = 0;
}

void incrementValue() {
    value++;
}

void decrementValue() {
    value--;
}

void resetValue() {
    value = 0;
}

int getValue() {
    return value;
}

void setValue(int valueToSet) {
    value = valueToSet;
}

int getNumberOfSchedules() {
    return numberOfSchedules;
}

void incrementNumberOfSchedules() {
    numberOfSchedules++;
}

void decrementNumberOfSchedules() {
    numberOfSchedules--;
}

/**
 * Reset the number of schedules - this acts as a pointer
 */
void resetNumberOfSchedules() {
    numberOfSchedules = 0;
}

/**
 * Gets one schedule from the list at a specified index.
 * @param index
 * @return
 */
struct oneSchedule getScheduleFromList(int index) {
    return scheduleList.list[index];
}

/**
 * Get the schedule list should it be needed.
 * @return the schedule list
 */
list_schedules getScheduleList() {
    return scheduleList;
}

/**
 * Used for our quicksort functionality
 * @param a
 * @param b
 * @return
 */
int compareSchedules(const void *a, const void *b) {
    const one_schedule *s1 = (const one_schedule *) a;
    const one_schedule *s2 = (const one_schedule *) b;

    if (s1->hour < s2->hour) {
        return -1;
    }
    if (s1->hour > s2->hour) {
        return 1;
    }

    if (s1->minute < s2->minute) {
        return -1;
    }
    if (s1->minute > s2->minute) {
        return 1;
    }

    return 0;
}

/**
 * Runs a quick sort on our schedule list
 */
void sortScheduleList() {
    qsort(scheduleList.list, scheduleList.sizeOfList, sizeof(one_schedule), compareSchedules);
}


/**
 * This is the method to add schedules to the main list. Does reallocation to make size for new schedules
 * @param thisSchedule is the schedule to be added
 */
void addScheduleToList(one_schedule thisSchedule) {
    one_schedule *newArray = realloc(scheduleList.list, (scheduleList.sizeOfList + 1) * sizeof(one_schedule));
    scheduleList.list = newArray;
    scheduleList.list[scheduleList.sizeOfList] = thisSchedule;
    scheduleList.sizeOfList++;
    sortScheduleList();
}

/**
 * This is to be run on program start to initialise the variables
 */
void initialiseScheduleList() {
    scheduleList.list = malloc(scheduleList.sizeOfList * sizeof(list_schedules));
    if (scheduleList.list == NULL) {
        printf("Error allocating memory for schedule list\n");
        exit(1);
    }
    scheduleList.sizeOfList = 0;
}
