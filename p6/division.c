///////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
// This File:        division.c
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
#include<signal.h>

#define BUFFER_SIZE 100

int divisionCount = 0; // number of divisions

/*
 * Handles any divide by zero signals (SIGFPE).
 * @param sigNum: Number of the incoming signal
 */
static void divisionHandler(int sigNum){
    // print statements
    printf("Error: a division by 0 operation was attempted.\n");
    printf("Total number of operations completed successfully: %d\n", divisionCount);
    printf("The program will be terminated.\n");
    exit(0);
}

/*
 * Handles any interrupt signals (SIGINT).
 * @param sigNum: Number of the incoming signal
 */
static void interruptHandler(int sigNum){
    // print statements
    printf("Total number of operations completed successfully: %d\n", divisionCount);
    printf("The program will be terminated.\n");
    exit(0);
}


/*
 * Main driver of the program that gets user input and divides the two numbers, returning the remainder and quotient.
 */
int main(){

    struct sigaction sa; // create structure
    // register handler for divide by 0
    memset(&sa, 0, sizeof(sa)); // zero memory
    sa.sa_handler = divisionHandler;
    sigaction(SIGFPE, &sa, NULL);

    // register signal user for interrupt
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = interruptHandler;
    sigaction(SIGINT, &sa, NULL); // zero memory

    // infinite loop to get user input and divide
    while(1){
        char* inputBuffer = malloc(BUFFER_SIZE * sizeof(char));

        // input vars
        int num1;
        int num2;
        char* input1 = NULL;
        char* input2 = NULL;

        // get user input
        printf("Enter first integer: ");
        input1 = fgets(inputBuffer, BUFFER_SIZE, stdin);
        num1 = atoi(input1);
        printf("Enter second integer: ");
        input2 = fgets(inputBuffer, BUFFER_SIZE, stdin);
        num2 = atoi(input2);

        // get quotient and remainder
        int quotient = num1 / num2;
        int remainder = num1 % num2;

        divisionCount++;

        printf("%d / %d is %d with a remainder of %d\n", num1, num2, quotient, remainder);
    }

}
