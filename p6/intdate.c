///////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
// Other Files:      NONE
// Semester:         CS 354 Fall 2019
//
// Author:           Rohan Mendiratta
// Email:            rmendiratta@wisc.edu
// CS Login:         rmendiratta
//
/////////////////////////// OTHER SOURCES OF HELP /////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          NONE
//
// Online sources:   NONE
//
///////////////////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<time.h>

int signalCount = 0; // number of signals

/*
 * Handler for alarm signals (SIGALRM) that prints the PID and current time and date.
 * @param sigNum: Number of the incoming signal
 */
static void alarmHandler(int sigNum){
    // get PID
    int pid = getpid();
    // get time
    time_t currentTime = time(NULL);
    printf("PID: %d | Current Time: %s\n", pid, ctime(&currentTime));
    // reset alarm
    alarm(3);

    return;
}

/*
 * Handler for user signals (SIGUSR1) that prints a statement.
 * @param sigNum: Number of the incoming signal
 */
static void userHandler(int sigNum){
    printf("SIGUSR1 caught!\n");
    signalCount++;
}

/*
 * Handler for interrupt signals (SIGINT) that prints out the number of times that SIGUSR1 was received.
 * @param sigNum: Number of the incoming signal
 */
static void interruptHandler(int sigNum){
    printf("SIGINT received.\n");
    printf("SIGUSR1 was received %d times. Exiting now.\n", signalCount);
    exit(1);
}

int main(){
    // set alarm
    alarm(3);
    struct sigaction sa; // create structure

    // register handler for alarm
    memset(&sa, 0, sizeof(sa)); // zero memory
    sa.sa_handler = alarmHandler;
    sigaction(SIGALRM, &sa, NULL);

    // register handler for user
    memset(&sa, 0, sizeof(sa)); // zero memory
    sa.sa_handler = userHandler;
    sigaction(SIGUSR1, &sa, NULL);

    // register handler for interrupt
    memset(&sa, 0, sizeof(sa)); // zero memory
    sa.sa_handler = interruptHandler;
    sigaction(SIGINT, &sa, NULL);

    while(1){}
}


