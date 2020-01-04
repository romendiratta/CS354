///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        n_in_a_row
// This File:        n_in_a_row
// Other Files:      NONE
// Semester:         CS 354 Fall 2019
//
// Author:           Rohan Mendiratta
// Email:            rmendiratta@wisc.edu
// CS Login:         rmendiratta
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          NONE
//
// Online sources:   NONE
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>     

char *COMMA = ",";  

/*
 * Retrieves from the first line of the input file,
 * the size of the board (number of rows and columns).
 * 
 * fp: file pointer for input file
 * size: pointer to size
 */
void get_dimensions(FILE *fp, int *size) {      
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fp) == -1) {
        printf("Error in reading the file\n");
        exit(1);
    }

    char *token = NULL;
    token = strtok(line, COMMA);
    *size = atoi(token);
}   

/*
 * Checks the count of x's and o's on the game board
 *
 * @param board game board
 * @param size length of side of board is size
 * @return 1 if one more x than o, 2  if same amount of x's and o's, 0 if invalid number of x's and o's
 */
int check_x_and_o_count(int **board, int size) {
    // counts x's and y's in the board
    int count_x = 0;
    int count_o = 0;
    for(int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (*(*(board + i) + j) == 1) {
                count_x++;
            }
            if (*(*(board + i) + j)== 2) {
                count_o++;
            }
        }
    }
    // returns number based on x:o
    if (count_x  == count_o + 1) {
        return 1;
    }
    if (count_x == count_o ) {
        return 2;
    }
    return 0;

}
/*
 * Checks for x or o wins on gameboard
 *
 * @param board gameboard
 * @param size  length of side of board is size
 * @return 1 if x wins, 2 if o wins, 3 if no winners, 0 if invalid amount of winners
 */
int check_for_win(int **board, int size) {
    int x_wins = 0; // holds num of x wins
    int o_wins = 0; // holds num of o wins
    int temp_x; // temp var
    int temp_o; // temp var

    // checks for vertical wins
    for(int i = 0; i < size; i++) {
        temp_x = 0;
        temp_o = 0;
        for(int j = 0; j < size; j++){
            int val = *(*(board + j) + i);
            if (val == 1) {
                temp_x++;
            }
            if(val == 2) {
                temp_o++;
            }
        }
        if(temp_x == size) {
            x_wins++;
        }
        if(temp_o == size) {
            o_wins++;
        }
    }

    // check for horizontal wins
    for(int i = 0; i < size; i++) {
        temp_x = 0;
        temp_o = 0;
        for(int j = 0; j < size; j++){
            int val = *(*(board + i) + j);
            if (val == 1) {
                temp_x++;
            }
            if(val == 2) {
                temp_o++;
            }
        }
        if(temp_x == size) {
            x_wins++;
        }
        if(temp_o == size) {
            o_wins++;
        }
    }

    // check for left diagonal wins
    temp_x = 0;
    temp_o = 0;
    for(int i = 0; i < size; i++) {
        int val = *(*(board + i) + i);
        if (val == 1) {
            temp_x++;
        }
        if(val == 2) {
            temp_o++;
        }
        if(temp_x == size) {
            x_wins++;
        }
        if(temp_o == size) {
            o_wins++;
        }
    }

    // check for right diagonal wins
    temp_x = 0;
    temp_o = 0;
    for(int i = 0; i < size; i++) {
        int val = *(*(board + i) + (size-1));
        if (val == 1) {
            temp_x++;
        }
        if(val == 2) {
            temp_o++;
        }
        if(temp_x == size) {
            x_wins++;
        }
        if(temp_o == size) {
            o_wins++;
        }
    }

    if (x_wins == 1 && o_wins == 0) {
        return 1;
    }
    if (x_wins == 0 && o_wins == 1) {
        return 2;
    }
    if (x_wins == 0 && o_wins == 0) {
        return 3;
    }
    if (x_wins == 2 && o_wins == 0) {
        return 4;
    }
    return 0;
}
/*
 * Returns 1 if and only if the board is in a valid state.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * size: number of rows and columns
 */
int n_in_a_row(int **board, int size) {
    int markerCount = check_x_and_o_count(board, size);
    int winCount = check_for_win(board, size);
    // invalid amount of x's and o's on board
    if(markerCount == 0){
	    return 0;
    }
    // invalid number of wins on the board
    if(winCount == 0) {
        return 0;
    }
    // x played the last move, yet o won
    if (markerCount == 1 && winCount == 2) {
        return 0;
    }
    // o played the last move, yet x won
    if (markerCount == 2 && winCount == 1) {
        return 0;
    }
    // x won twice, but same amount of markers on the board
    if (winCount == 4 && markerCount == 2) {
        return 0;
    }
    return 1;   
}     


/*
 * This program prints Valid if the input file contains
 * a game board with either 1 or no winners; and where
 * there is at most 1 more X than O.
 * 
 * argc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[]) {              

    // Checks if number of command-line arguments is correct.
    if (argc != 2) {
        printf("Invalid number of arguments\n");
	exit(1);
    }

    // Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
        printf("Cannot open file for reading\n");
        exit(1);
    }

    int size; // size of board

    get_dimensions(fp, &size); // gets the size of the board

    //  allocate a 2D array of dimensions retrieved above.
    int **board = malloc(sizeof(int*) * size);
    for ( int i = 0; i < size; i++ ) {
	    *(board + i) = malloc(sizeof(int) * size);
    }

    //Read the file line by line.
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < size; i++) {

        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading the file\n");
            exit(1);
        }

        token = strtok(line, COMMA);
        for (int j = 0; j < size; j++) {
            // adds each value to the array
           *(*(board + i) + j) = atoi(token);
            token = strtok(NULL, COMMA);

        }
    }
    int value = n_in_a_row(board, size); // checks state of board

    if(value == 0){
        printf("invalid \n");
    }
    else {
	    printf("valid \n");
    }	

    // frees up allocated memory
    for(int i = 0; i < size; i++){
         free(*(board+i));
    }
    free(board);

    //Close the file.
    if (fclose(fp) != 0) {
        printf("Error while closing the file\n");
        exit(1);
    } 
    return 0;       
}       
