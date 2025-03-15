
#ifndef STATE_H
#define STATE_H
#include <stddef.h>

/**
 * This is one item on our list schedules that holds the hour, minute and number of rotations for a schedule
 */
typedef struct oneSchedule {
    int hour;
    int minute;
    int rotations;
}one_schedule;

/**
 * This is our list of schedules which holds an array of single schedules - must be reallocated in size per addition
 * and the size of the list. This acts as a sort of pointer
 */
typedef struct listSchedules {
    one_schedule *list;
    size_t sizeOfList;
}list_schedules;

void setOpState(char* state);
char *getOpState();
int getNoFeedsSinceAuto();
void addNoFeedsSinceAuto();
int getValue();
void resetValue();
void incrementValue();
void decrementValue();
void freeScheduleList();
struct oneSchedule getScheduleFromList(int index);
void addScheduleToList(one_schedule thisSchedule);
void initialiseScheduleList();
list_schedules getScheduleList();
void setNextFeedTimeToNext();
one_schedule getNextFeedTime();
void sortScheduleList();
void initializeFirstFeedTime();
void resetNoFeedsSinceAuto();
void setValue(int valueToSet) ;


#endif //STATE_H
