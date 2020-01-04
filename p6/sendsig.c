///////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// This File:        sendsig.c
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
#include<unistd.h>
#include<string.h>
#include<signal.h>

/*
 * This program sends either a SIGINT or SIGUSR1 signal to the specified process.
 * @param arg1: type of signal to be send ("-i" = SIGINT) ("-u" = SIGUSR1)
 * @param arg2: PID of the process to send the signal to.
 */
int main(int argc, char* argv[]){

    // get PID from args
    int pid;
    pid = atoi(argv[2]);

    // send SIGINT
    if (strcmp(argv[1], "-i") == 0){
        kill(pid, 2);
    }
    // send SIGUSR1
    if (strcmp(argv[1], "-u") == 0){
        kill(pid, 10);
    }

}