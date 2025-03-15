Author: Callum Stanford-Hoare 
Email: cas113@aber.ac.uk

This is the readme file for the CS23820 C Assignment.

The project was written on a windows machine, thus associated includes and path edits were made for the program to work. 

For the purposes of submission the customejre folder was abstracted as well as data in the debug build file. 
Readd the customejre folder where necessary.
-----------------------

Present inside the folder with this readme file should also be a PDF for the required documentation as well as a 2 video recordings.
blankscreen shows the blank screen functionality of the program
screencasting shows a walkthrough of the general functionality of the program (with voiceover)

-----------------------
Inside the 2024-2025_fish_c the following files should be present:
1.CMakeLists
2.display.c
3.display.h
4.filehandling.c
5.filehandling.h
6.fish.c
7.fish.h
8.fish_debug.c
9.main.c
10.scheduling.c
11.scheduling.h
12.state.c
13.state.h
14.cmake-build-debug (Folder)

Inside 14's folder of note should the schedules.txt file use to read and write data to.
-----------------------

Files should be compiled together. Main method specified in main.


Made use of:
stdio.h
stdlib.h
string.h
assert.h
time.h

-----------------------

compile when in file structure with:
gcc *.c -o Output; ./Output

Or more simply load the program into CLion. Build -> Clean -> Build project