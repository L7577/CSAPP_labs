#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "cachelab.h"

#ifndef _BOOL_
#define _BOOL_
typedef enum {false, true} bool;
#endif

typedef struct cache_line *CacheLine;
struct cache_line
{
    int valid;
    int tag;
    int time_stamp;
};
typedef  CacheLine * Cache;

/*print help messages*/
void usage(char *argv[],bool error)
{
    if(error)
        fprintf(stderr, "%s: Missing required command line argument \n",argv[0] );
    
    printf("Usage: %s [-hv] -s <sum> -E <num> -b <num> -t <file> \n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExample:\n"); 
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n",argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n",argv[0]);

}

//cache simulator
void cachesim(int argc,char **argv)
{   
    bool verbose=false;
    int setindexbit = 0; // S = 2^s
    int lineperset = 0;  // E
    int blockoffset = 0; // offset
    FILE *tracefile = NULL;
    int opt;
    while ((opt = getopt(argc,argv,"hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 's':  
                setindexbit = atoi(optarg);
                break;
            case 'E':
                lineperset = atoi(optarg);
                break;
            case 'b':
                blockoffset = atoi(optarg);
                break;
            case 't':
                tracefile = fopen(optarg,"r");
                if (tracefile == NULL){
                    printf("%s: No such file or directory\n",optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                usage(argv,false);
                exit(0);
            default:
                usage(argv,true);
                exit(EXIT_FAILURE);
        }
    }
    if (setindexbit == 0 || lineperset == 0 || blockoffset == 0){
        usage(argv,true);
        exit(EXIT_FAILURE);
    }

    int setsize = 1 << setindexbit; // S = 2^s
    Cache cache;
    cache = malloc(setsize * sizeof(Cache));
    //initialize cache[S][E];
    for(int i = 0; i < setsize; i++){
        cache[i] = malloc(lineperset * sizeof(struct cache_line));
        for(int j = 0; j < lineperset; j++){
            cache[i][j].valid = -1;
            cache[i][j].tag = -1;
            cache[i][j].time_stamp = -1;
        }
    }

    char operation;
    long unsigned int address;
    int size;
    unsigned int fetch_count = 0; 
    int count = 0;
    unsigned int hit_count = 0;
    unsigned int miss_count = 0;
    unsigned int eviction_count = 0;
     
    while(fscanf(tracefile," %c %lx,%d",&operation,&address,&size)>0){
        count++; // time stamp 
        if (operation == 'I'){
            continue;
        }
        else {
            switch (operation) {
                case 'L':
                    fetch_count = 1;
                    break;
                case 'M':
                    fetch_count = 2;
                    break;
                case 'S':
                    fetch_count = 1;       
                    break;
            }
            int setindex = (address >> blockoffset) & ((-1U) >> (64 - setindexbit));
            int tag = address >> (setindexbit + blockoffset);

            int max_time_stamp = 10000000;
            int max_time_stamp_index = -1;
            bool hit_flag = false;
            bool eviction_flag = true;
            bool miss_flag = true;

            if (verbose && operation != 0){
                printf("%c %lx,%d",operation,address,size);
            }
            // hit 
            for (int i = 0; i < lineperset; i++){
                if ( cache[setindex][i].tag == tag && cache[setindex][i].valid == 1 ){
                    cache[setindex][i].time_stamp = count;
                    hit_count++;
                    hit_flag = true;
                    miss_flag = false;
                    if(verbose){
                        printf(" hit ");
                    }
                    break;
                }
            }
            // if miss , then storage
            if (miss_flag){
                miss_count++;
                if (verbose)
                    printf(" miss ");
                for (int i = 0; i < lineperset; i++){
                    if ( cache[setindex][i].valid == -1 ){
                        cache[setindex][i].valid = 1;
                        cache[setindex][i].tag = tag;
                        cache[setindex][i].time_stamp = count;
                        eviction_flag = false;
                        hit_flag = true;
                        break;
                    }
                }
                // if miss and the cacheset is full , then eviction , use lru .
                if (eviction_flag){
                    for(int i = 0; i < lineperset; i++){
                        if (cache[setindex][i].time_stamp < max_time_stamp){
                            max_time_stamp = cache[setindex][i].time_stamp;
                            max_time_stamp_index = i;
                        }
                    }
                    eviction_count++;
                    hit_flag = true;
                    if (verbose) {
                        printf ("eviction ");
                    }
                    // update old cache[][] tag and time_stamp
                    cache[setindex][max_time_stamp_index].tag = tag;
                    cache[setindex][max_time_stamp_index].time_stamp = count;
                }
            }
            if (fetch_count == 2 && hit_flag ){
                hit_count++;
                if(verbose)
                    printf("hit ");
            }  
            if (verbose)
                printf("\n");
        }
    }
    if ( tracefile != NULL )
        fclose(tracefile);
    for (int i = 0; i < setsize; i++)
        free(cache[i]);
    free(cache);
    printSummary(hit_count, miss_count, eviction_count);

}

int main(int argc, char **argv)
{
    cachesim(argc,argv);
    return 0;
}
