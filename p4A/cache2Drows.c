////////////////////////////////////////////////////////////////////////////////
// Main File:        cache2Drows.c
// This File:        cache2Drows.c
// Other Files:      cache1D.c, cache2Dcols.c, cache2Dclash.c
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

int arr[3000][500];

int main(int argc, char *argv[]){
	
    for(int i = 0; i < 3000; i++){
	    for(int j = 0; j < 500; j++) {
	        arr[i][j] = i + j;
	    }
    }
    return 0;
}
