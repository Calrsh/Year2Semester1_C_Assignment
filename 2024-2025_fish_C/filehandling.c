#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "display.h"
#include "fish.h"
#include "state.h"



/**
 * This is used for saving data to our file - not only does it save the file list ,but it also saves the
 * value for our warmclockstart
 */
void saveSchedulesToFile() {
    FILE *file = fopen("schedules.txt", "w");
    list_schedules schedules = getScheduleList();

    fprintf(file, "%llu", clockWarmStart(0));

    for (int i = 0; i < schedules.sizeOfList; ++i) {
        one_schedule schedule = schedules.list[i];
        char conversion[15];
        sprintf(conversion, "\n%d-%d-%d", schedule.hour, schedule.minute, schedule.rotations);
        fprintf(file, conversion);
    }
    fclose(file);
}

/**
 * This is used for reading data from the file. If there is no file then announce so. Otherwise it will read
 * the first line of the file for our warmclockstart functionality and then go line by line for each of the presumed
 * available schedules within the list
 */
void readSchedulesFromFile() {
    FILE *file = fopen("schedules.txt", "r");
    if (file != NULL) {
        char word[20];
        fgets(word, sizeof(word), file);
        long long value = strtoll(word, NULL, 10);
        clockWarmStart(value);
        //File format is Hours-Minutes-Rotations
        while (fgets(word, 20, file)) {
            one_schedule schedule;
            char *charPos = strtok(word, "-");
            int num = strtol(charPos, &charPos, 10);
            schedule.hour = num;

            charPos = strtok(NULL, "-");
            num = strtol(charPos, &charPos, 10);
            schedule.minute = num;

            charPos = strtok(NULL, "-");
            num = strtol(charPos, &charPos, 10);
            schedule.rotations = num;

            addScheduleToList(schedule);
        }
        fclose(file);
        setClockWithWarmStart();
    } else {
        printf("File could not be opened\n");
    }
}
