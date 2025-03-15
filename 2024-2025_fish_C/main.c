#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fish.h"
#include "display.h"
#include "state.h"
#include "filehandling.h"



void userProcessing() {
    infoMessage("Welcome to the fish feeder test program");
    clockSetUp();
    displayColour("white", "black");
    displayClear();
    foodFill(50); // Fill the food container
    //Setting up our arrays
    initialiseScheduleList();
    readSchedulesFromFile();
    initializeFirstFeedTime();
    startMenu();
}

/**
 * main entry point for the Fish Feeder Emulator program
 * @return
 */
int main() {
    // turn on all console logging for the Java and fish feeder JNI C code. Shouldn't need this
    //logAddInfo(GENERAL | METHOD_ENTRY | JNI_MESSAGES | JFX_MESSAGES | THREAD_NAME | THREAD_ID | STACK_INFO);
    //logAddInfo( GENERAL | JFX_MESSAGES | THREAD_NAME | THREAD_ID );

    //logRemoveInfo(METHOD_ENTRY); // don't show these debug messages in the gui

    // add the GUI debug window showing messages received
    logAddInfo(GUI_INFO_DEBUG);

    // add a log entry for entry to this method
    logAdd(METHOD_ENTRY, "main(). test of Fish Feed Hardware Emulator using a JavaFX GUI and jni");

    // start the JVM and set up the JNI environment
    // this will result in the userProcessing() function being called to run the C part of the program
    if (jniSetup() != 0) return 1;

    // hand over this thread to Java FX to create the fish hardware emulator GUI.
    // FX will terminate the application when the main window is closed
    // or the C application calls the FishFeederEmulator.exit() function.
    javaFx();

    return (EXIT_SUCCESS);
}
