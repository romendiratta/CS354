////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013,2019
// Posting or sharing this file is prohibited, including any changes/additions.
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        csim.c
// This File:        csim.c
// Other Files:      None
// Semester:         CS 354 Fall 2019
//
// Author:           Rohan Mendiratta
// Email:            rmendiratta@wisc.edu
// CS Login:         rmendiratta
//
//////////////////////////////////////////////////////////////////////////////// 

/*
 * csim.c:
 * A cache simulator that can replay traces (from Valgrind) and output
 * statistics for the number of hits, misses, and evictions.
 * The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most 1 cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLES **********************************************/

//Globals set by command line args.
int b = 0; //number of block (b) bits
int s = 0; //number of set (s) bits
int E = 0; //number of lines per set

//Globals derived from command line args.
int B; //block size in bytes: B = 2^b
int S; //number of sets: S = 2^s

//Global counters to track cache statistics in access_data().
int hit_count = 0;
int miss_count = 0;
int evict_count = 0;

//Global to control trace output
int verbosity = 0; //print trace if set
/******************************************************************************/


//Type mem_addr_t: Use when dealing with addresses or address masks.
typedef unsigned long long int mem_addr_t;

//Type cache_line_t: Use when dealing with cache lines.
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    //Add a data member as needed by your implementation for LRU tracking.
    long long counter;
} cache_line_t;

//Type cache_set_t: Use when dealing with cache sets
//Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;
//Type cache_t: Use when dealing with the cache.
//Note: A cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache (i.e., pointer var) we're simulating.
cache_t cache;

//this is the overall lru counter for evicted variables
long long lru_count = 0;
/*
* init_cache:
* Allocates the data structure for a cache with S sets and E lines per set.
* Initializes all valid bits and tags with 0s.
*/
void init_cache() {
    // set size
    S = pow(2, s);
    // block size
    B = pow(2, b);
    // line entry
    cache_line_t line;

    // creates a cache based on input parameters
    cache = (cache_set_t*) malloc(S * sizeof(cache_set_t));
    for(int i = 0; i < S; i++){
        cache[i] = (cache_line_t*) malloc(E * sizeof(cache_line_t));
        for(int j = 0; j < E; j++){
            line.valid = 0;
            line.tag = 0;
            line.counter = 0;
            cache[i][j] = line;
        }
    }
}


/*
 * free_cache:
 * Frees all heap allocated memory used by the cache.
 */
void free_cache() {
    // iterates over the cache freeing each one
    for(int i = 0; i < S; i++){
        free(cache[i]);
    }
    free(cache);
}


/*
 * access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If already in cache, increment hit_count
 * If not in cache, cache it (set tag), increment miss_count
 * If a line is evicted, increment evict_count
 */
void access_data(mem_addr_t addr) {
    mem_addr_t set_index = pow(2,s) - 1;
    // gets the set
    int set = (addr >> b) & set_index;
    // gets the tag
    int tag = addr >> (b + s);

    // checks hits in the memory block
    for(int a = 0; a < E; a++) {
        if(cache[set][a].tag == tag && cache[set][a].valid) { // found hit in a cache line
            hit_count++;
            cache[set][a].counter = lru_count++;
            return;
        }
    }

    // add to cache if we have a miss
    for(int i = 0; i < E; i++) {
        if(!cache[set][i].valid) {
            cache[set][i].valid = 1;
            cache[set][i].tag = tag;
            cache[set][i].counter = lru_count++;
            miss_count++;
            return;
        }
    }

    // if no lines are empty, evict and replace a block
    evict_count++;
    miss_count++;
    int lru_index = 0;
    for(int i = 0; i < E; i++) {
        if(cache[set][i].counter < cache[set][lru_index].counter){
            lru_index = i;
        }
    }
    // replace tag and count of old block
    cache[set][lru_index].tag = tag;
    cache[set][lru_index].counter = lru_count;
}


/*
 * replay_trace:
 * Replays the given trace file against the cache.
 *
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE each "L" as a load i.e. 1 memory access
 * TRANSLATE each "S" as a store i.e. 1 memory access
 * TRANSLATE each "M" as a load followed by a store i.e. 2 memory accesses 
 */
void replay_trace(char* trace_fn) {
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    // check that file opened correctly
    if (!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }
    
    // reads through trace file executing the correct commands based on file input 
    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);

            if (verbosity) {
                printf("%c %llx,%u ", buf[1], addr, len);
            }
            if(buf[1] == 'M') {
                access_data(addr);
                access_data(addr);
            }
            if(buf[1] == 'L' || buf[1] == 'S') {
                access_data(addr);
            }
            if (verbosity)
                printf("\n");
        }
    }
    fclose(trace_fp);
}


/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */
void print_usage(char* argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for block offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}


/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */
void print_summary(int hits, int misses, int evictions) {
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}


/*
 * main:
 * Main parses command line args, makes the cache, replays the memory accesses
 * free the cache and print the summary statistics.  
 */
int main(int argc, char* argv[]) {
    char* trace_file = NULL;
    char c;

    // parse the command line flags -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    // checks that all command line arguments are given
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    // initialize cache
    init_cache();

    // replay the trace file
    replay_trace(trace_file);

    // free the cache
    free_cache();

    //Print the statistics to a file.
    //DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_count, miss_count, evict_count);
    return 0;
}           
