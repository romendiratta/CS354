///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        hill_climb.c
// This File:        hill_climb.c
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
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   
#include <stdlib.h>     
#include <string.h>  

/*
 * Structure representing the Matrix
 * numRows: number of rows
 * numCols: number of columns
 * matrix: 2D heap allocated array of integers
 */
typedef struct {    
    int numRows;
    int numCols;
    int **matrix;
} Matrix;

/*
 * Structure representing the position on the board
 * rowPos: index in the rows
 * colPos: index in the columns
 */
typedef struct {
    int rowPos;
    int colPos;
} Position;

/*
 * Get the dimensions of the matrix that are on
 * the first line of the input file.
 * 
 * infp: file pointer for input file
 * nRows: pointer to number of rows in the matrix   
 * nCols: pointer to number of columns in the matrix
 */

char *COMMA = ","; // hold comma variable

/*
 * Gets the dimesion of the matrix
 * infp: input file pointer
 * nRows: number of rows int pointer
 * nCols: number of columns int pointer
 */
void get_dimensions(FILE *infp, int *nRows, int *nCols) {
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, infp) == -1) {
        printf("Error in reading the file\n");
        exit(1);
    }

    char *token = NULL;
    token = strtok(line, COMMA);
    *nRows = atoi(token);
    token = strtok(NULL, COMMA);
    *nCols = atoi(token);
    free(line);
}

/*
 * Checks the surroundings of a matrix position to find the highest adjacent number
 * mstruct: maxtrix structure
 * pos: positon structure
 * return: -1 if no no adjadent number is bigger, else biggest number
 */
int check_surroundings(Matrix *mstruct, Position *pos) {
    int originalNum = *(*(mstruct->matrix + pos->rowPos) + pos->colPos); // holds original number
    int highestNum = -1; // holds highest number found
    int tempRowPos = pos->rowPos;
    int tempColPos = pos->colPos;

    //check top left
    tempRowPos--;
    tempColPos--;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    //check top middle
    tempColPos++;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    //check top right
    tempColPos++;
    if (tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows ) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    //check left
    tempColPos = tempColPos - 2;
    tempRowPos = tempRowPos + 1;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    // check original position
    tempColPos = tempColPos + 1 ;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    // check right
    tempColPos = tempColPos + 1 ;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    //check bottom left
    tempColPos = tempColPos - 2;
    tempRowPos = tempRowPos + 1;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    //check bottom middle
    tempColPos = tempColPos + 1;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    // check bottom right
    tempColPos = tempColPos + 1;
    if(tempColPos >= 0 && tempRowPos >= 0 && tempColPos < mstruct->numCols && tempRowPos < mstruct->numRows) {
        if (*(*(mstruct->matrix + tempRowPos) + tempColPos) > highestNum) {
            pos->rowPos = tempRowPos;
            pos->colPos = tempColPos;
            highestNum = *(*(mstruct->matrix + tempRowPos) + tempColPos);
        }
    }

    if (highestNum == originalNum) {
        return -1;
    }
    return highestNum;

}

/*
 * Continually find and print the largest neighbor
 * that is also larger than the current position
 * until nothing larger is found.       
 * 
 * outfp: file pointer for output file  
 * mstruct: pointer to the Matrix structure
 */
void hill_climb(FILE *outfp, Matrix *mstruct) {
    int returnVal = 0;
    // creates a position structure
    Position *pos = malloc(sizeof(Position));
    // sets positions to first spot in matrix
    pos->colPos = 0;
    pos->rowPos = 0;
    fprintf(outfp, "%d", *(*(mstruct->matrix)));
    // Checks surroundings and prints to file 
    while(1) {
   	 returnVal = check_surroundings(mstruct, pos);
        if(returnVal == -1){
            break;
        }
        fprintf(outfp, " %d", *(*(mstruct->matrix + pos->rowPos) + pos->colPos));
    }
       fprintf(outfp,"\n");
       free(pos);

}    

/*
 * This program reads an m by n matrix from the input file
 * and outputs to a file the path determined from the
 * hill climb algorithm.    
 *
 * argc: CLA count
 * argv: CLA values
 */

int main(int argc, char *argv[]) {

    // Check if number of command-line arguments is correct
    if (argc != 3) {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    // Open the input file and check if it opened successfully
    FILE *infp = fopen(*(argv + 1), "r");
    if (infp == NULL) {
        printf("Cannot open file for reading\n");
        exit(1);
    }

    // Declare local variables including the Matrix structure
    Matrix board;

    // Call the function get_dimensions
    get_dimensions(infp, &(board.numRows), &(board.numCols));

    // Dynamically allocate a 2D array in the Matrix structure
    board.matrix = malloc(sizeof(int*) * board.numRows);
    for ( int i = 0; i < board.numRows; i++ ) {
        *(board.matrix + i) = malloc(sizeof(int) * board.numCols);
    }

    // Reads the file and fills the matrix
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < board.numRows; i++) {

        if (getline(&line, &len, infp) == -1) {
            printf("Error while reading the file\n");
            exit(1);
        }

        token = strtok(line, COMMA);
        for (int j = 0; j < board.numCols; j++) {
            // adds each value to the array
            *(*(board.matrix + i) + j) = atoi(token);
            token = strtok(NULL, COMMA);
        }
    }
    free(line);

    // Open the output file
    FILE *outfp = fopen(argv[2], "w");
    if (outfp == NULL) {
        printf("Cannot open output file for writing\n");
        exit(1);
    }

    // Call the function hill_climb
    hill_climb(outfp, &board);


    // Free the dynamically allocated memory
    for(int i = 0; i < board.numRows ; i++) {
        free(*(board.matrix + i));
    }
    free(board.matrix);

    // Close the input file
    fclose(infp);

    // Close the output file
    fclose(outfp);

    return 0; 
}
