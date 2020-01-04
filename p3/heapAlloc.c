////////////////////////////////////////////////////////////////////////////////
// Main File:        heapAlloc.c
// This File:        heapAlloc.c
// Other Files:      heapAlloc.c
// Semester:         CS 354 Fall 2019
//
// Author:           Rohan Mendiratta
// Email:            rmendiratta@wisc.edu
// CS Login:         rmendiratta
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
// Persons:          NONE
//                   Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
// Online sources:  NONE
//                  avoid web searches to solve your problems, but if you do
//                  search, be sure to include Web URLs and description of
//                  of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "heapAlloc.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct blockHeader {
    int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    *
    * End Mark:
    *  The end of the available memory is indicated using a size_status of 1.
    *
    * Examples:
    *
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    *
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} blockHeader;

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

blockHeader *heapStart = NULL;

/*
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void DumpMem() {

    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end   = NULL;
    int t_size;

    blockHeader *current = heapStart;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used   = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");

    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;

        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status,
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);

        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}

/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use NEXT-FIT PLACEMENT POLICY to chose a free block
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* allocHeap(int size) {
    // Checks for a valid heap allocation size
    if (size <= 1) {
        return NULL;
    }
    // total block size including header
    int totalBlockSize = (size + sizeof(blockHeader));

    // constant to check for double word alignment
    int DOUBLE_WORD_SIZE = 8;

    // Makes the total block size 8 if it is less
    if (totalBlockSize < DOUBLE_WORD_SIZE) {
        totalBlockSize = DOUBLE_WORD_SIZE;
    }
        // pads block size to double word alignment spec
    else if (totalBlockSize % DOUBLE_WORD_SIZE != 0) {
        totalBlockSize = totalBlockSize + (DOUBLE_WORD_SIZE - totalBlockSize % 8);
    }

    blockHeader *curr = heapStart; // start of heap
    int sizeStatusLSB = 0; // holds the LSB of the size status
    int sizeBlock = 0; // size of the current block
    blockHeader *foundBlock = NULL; // block that is found in iteration
    int foundBlockSize = 0; // size of block found in iteration

    if (curr == NULL) {
        return NULL;
    }

    // iterates through looking for free block
    while ((curr->size_status) > 1) {
        // gets LSB value and size of block
        sizeStatusLSB = (curr->size_status) % DOUBLE_WORD_SIZE;
        sizeBlock = (curr->size_status) - sizeStatusLSB;

        // checks for block allocation
        if ((sizeStatusLSB % 2) != 0) {
            curr = (blockHeader*)((char*)curr + sizeBlock);
        }
        else {
            // block fits perfectly into
            if (sizeBlock == totalBlockSize) {
                foundBlock = curr;
                if (sizeStatusLSB == 2) {
                    foundBlock->size_status = totalBlockSize + 3;
                }
                else {
                    foundBlock->size_status = totalBlockSize + 1;
                }

                // sets prev to previous block for later fitting
                blockHeader *prev = (blockHeader*)((char*)foundBlock + totalBlockSize);
                // sets block header status of previous block
                if (prev->size_status != 1) {
                    prev->size_status = (prev->size_status) + 2;
                }
                return (blockHeader*)((char*)foundBlock + 4);
            }
                // iterates through looking for a block big enough for allocation
            else if (sizeBlock > totalBlockSize) {
                foundBlock = curr;
                foundBlockSize = (foundBlock->size_status) - ((foundBlock->size_status) % DOUBLE_WORD_SIZE);
                break;
            }
            else {
                curr = (blockHeader*)((char*)curr + sizeBlock); // continue for too small
                //sized block
            }
        }
    }

    // could not find a block big enough for allocation
    if (foundBlock == NULL) {
        return NULL;
    }

    // block size that will not be needed for allocation
    int breakBlockSize = (foundBlockSize - totalBlockSize);

    // break block and set header
    blockHeader *breakBlockHeader = (blockHeader*)((char*)foundBlock + totalBlockSize);
    breakBlockHeader->size_status = (breakBlockSize) + 2;

    // add footer to unused block
    blockHeader *breakBlockFooter = (blockHeader*)((char*)breakBlockHeader + breakBlockSize - 4);
    breakBlockFooter->size_status = breakBlockSize;

    // check if previous block can be used
    blockHeader *prev = (blockHeader*)((char*)breakBlockHeader + breakBlockSize);

    int sizeStatusPrev = prev->size_status;

    // check that size status of previous block us valid
    if (sizeStatusPrev != 1 && sizeStatusPrev % 2 == 0) {
        int prevBlockSize = sizeStatusPrev - (sizeStatusPrev % DOUBLE_WORD_SIZE);

        // set new sizes for block header and footer
        breakBlockHeader->size_status = (breakBlockHeader->size_status) + prevBlockSize;
        breakBlockHeader  = (blockHeader*)((char*)breakBlockHeader + breakBlockSize +
                                           prevBlockSize - 4);
        breakBlockFooter->size_status = breakBlockSize + prevBlockSize;
    }
    // set size status of found block
    if (sizeStatusLSB == 2) {
        foundBlock->size_status = totalBlockSize + 3;
    }
    else {
        foundBlock->size_status = totalBlockSize + 1;
    }

    // return the address of the payload
    blockHeader *returnBlock = (blockHeader*)((char*)foundBlock + 4);
    return returnBlock;
}

/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */
int freeHeap(void *ptr) {

    if (ptr == NULL) {
        return -1;
    }

    // constant to check for double word alignment
    int DOUBLE_WORD_SIZE = 8;

    // header of block to be freed
    blockHeader *freeBlock = (blockHeader*)((char*)ptr - 4);

    int sizeStatusLSB = freeBlock->size_status % DOUBLE_WORD_SIZE;

    // check if already free
    if (sizeStatusLSB == 0) {
        return -1;
    }
    // size of free block
    int sizeFreeBlock = freeBlock->size_status - sizeStatusLSB;

    // check for double word alignment
    if (sizeFreeBlock % DOUBLE_WORD_SIZE != 0) {
        return -1;
    }
    else {

        // check if block is free
        freeBlock->size_status = (freeBlock->size_status) - 1;

        // make footer and set size status
        blockHeader *freeBlockFooter = (blockHeader*)((char*)freeBlock +
                                                      sizeFreeBlock - 4);
        freeBlockFooter->size_status = sizeFreeBlock;

        // initialize next block
        blockHeader *nextBlock = (blockHeader*)((char*)freeBlock + sizeFreeBlock);

        // check that next block was freed
        if (nextBlock->size_status != 1) {
            nextBlock->size_status = (nextBlock->size_status) - 2;
        }

        // get LSB and size of next block
        int nextBlockSizeStatus = nextBlock->size_status;
        int nextBlockLSB = nextBlockSizeStatus % DOUBLE_WORD_SIZE;

        // check for coalesce
        if ((nextBlockSizeStatus != 1) && nextBlockLSB % 2 == 0) {

            int nextBlockSize = nextBlockSizeStatus - nextBlockLSB;

            // update size
            freeBlock->size_status = (freeBlock->size_status) + nextBlockSize;

            // make footer and set size status
            freeBlockFooter = (blockHeader*)((char*)freeBlock + sizeFreeBlock + nextBlockSize - 4);
            freeBlockFooter->size_status = sizeFreeBlock + nextBlockSize;
        }


        sizeStatusLSB = freeBlock->size_status % DOUBLE_WORD_SIZE;
        sizeFreeBlock = freeBlock->size_status - sizeStatusLSB;

        // block is free and prev block is free so coalesce
        if (sizeStatusLSB == 0) {
            blockHeader *prevBlockFooter = (blockHeader*)((char*)freeBlock - 4);

            int prevBlockSize = prevBlockFooter->size_status;
            blockHeader *prevBlockHdr = (blockHeader*)((char*)freeBlock - prevBlockSize);
            prevBlockHdr->size_status = (prevBlockHdr->size_status) + sizeFreeBlock;
            freeBlock = prevBlockHdr;
            freeBlockFooter->size_status = sizeFreeBlock + prevBlockSize;
        }
    }
    return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int initHeap(int sizeOfRegion) {

    static int allocated_once = 0; //prevent multiple initHeap calls

    int pagesize;  // page size
    int padsize;   // size of padding when heap size not a multiple of page size
    int allocsize; // size of requested allocation including padding
    void* mmap_ptr; // pointer to memory mapped area
    int fd;

    blockHeader* endMark;

    if (0 != allocated_once) {
        fprintf(stderr,
                "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    allocsize = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }

    allocated_once = 1;

    // for double word alignment and end mark
    allocsize -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heapStart = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    endMark = (blockHeader*)((void*)heapStart + allocsize);
    endMark->size_status = 1;

    // Set size in header
    heapStart->size_status = allocsize;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heapStart->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((char*)heapStart + allocsize - 4);
    footer->size_status = allocsize;

    return 0;
}

