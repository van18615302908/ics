#include "cachelab.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csim.h"
#include <getopt.h>
#include <limits.h>
#include <math.h>

// print usage info

void usage()
{
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
    exit(1);
}

// parse command line and get the parameters
void parseline(int argc, char **argv)
{
    int opt;
    int num = 0;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            usage();
            break;
        case 'v':
            verbose = 1;
            break;
        case 's':
            num = atoi(optarg);
            if (num == 0 && optarg[0] != '0')
            {
                printf("./csim: Missing required command line argument\n");
                usage();
            }
            numSet = num;
            break;
        case 'E':
            num = atoi(optarg);
            if (num == 0 && optarg[0] != '0')
            {
                printf("./csim: Missing required command line argument\n");
                usage();
            }
            associativity = num;
            break;
        case 'b':
            num = atoi(optarg);
            if (num == 0 && optarg[0] != '0')
            {
                printf("./csim: Missing required command line argument\n");
                usage();
            }
            blockSize = num;
            break;
        case 't':
            strcpy(filePath, optarg);
            break;
        case ':':
            printf("./csim: Missing required command line argument\n");
            usage();
            break;
        case '?':
            usage();
            break;
        default:
            printf("getopt error");
            exit(1);
            break;
        }
    }
}


void Creat_Cache()
{
    if (cache)
        free(cache);
    cache = (Cache *)malloc(sizeof(Cache));
    cache->S = numSet;
    cache->E = associativity;
    cache->B = blockSize;
    cache->line = (Cache_line **)malloc(sizeof(Cache_line *) * numSet);
    for (int i = 0; i < numSet; i++)
    {
        cache->line[i] = (Cache_line *)malloc(sizeof(Cache_line) * associativity);
        for (int j = 0; j < associativity; j++)
        {
            cache->line[i][j].valid = 0; 
            cache->line[i][j].tag = -1;
            cache->line[i][j].counter = 0;
        }
    }
}

void free_Cache()
{
    int S = cache->S;
    for (int i = 0; i < S; i++)
    {
        free(cache->line[i]);
    }
    free(cache->line);
    free(cache);
}



int is_full(int op_s)
{
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->line[op_s][i].valid == 0)
            return i;
    }
    return -1;
}

void update(int i, int op_s, int op_tag){
    
    cache->line[op_s][i].valid=1;
    cache->line[op_s][i].tag = op_tag;
    for(int k = 0; k < cache->E; k++)
         if(cache->line[op_s][k].valid==1)
             cache->line[op_s][k].counter++;
     cache->line[op_s][i].counter = 0;
}


int get_earest_index(int op_s)
{
    int index = 0;
    int max_counter = 0;
    for(int i = 0; i < cache->E; i++){
        if(cache->line[op_s][i].counter > max_counter){
            max_counter = cache->line[op_s][i].counter;
            index = i;
        }
    }
    return index;
}



void update_info(unsigned address)
{
    int op_tag = address/(numSet*blockSize);
    int op_s = (address/blockSize )%numSet;
    int index = -1;
    for (int i = 0; i < cache->E; i++)
    {
        if (cache->line[op_s][i].valid && cache->line[op_s][i].tag == op_tag)
            index = i;
    }

    if (index == -1)
    {
        miss++;
        if (verbose)
            printf("miss");
        int i = is_full(op_s);
        if(i==-1){
            eviction++;
            if(verbose) printf("eviction");
            i = get_earest_index(op_s);
        }
        update(i,op_s,op_tag);
    }
    else{
        hit++;
        if(verbose)
            printf("hit");
        update(index,op_s,op_tag);    
    }
}

void Simulate() {
    Creat_Cache();
    FILE *File;
    char identifier;
    unsigned address;
    int size;    
    File = fopen(filePath, "r");

    while (fscanf(File, " %c %x,%d", &identifier, &address, &size) > 0) 
    {
        if (verbose)
            printf("%c %x,%d ", identifier, address, size);
        switch (identifier)
        {
        case 'M': 
            update_info(address);
            update_info(address);
            break;
        case 'L':
            update_info(address);
            break;
        case 'S':
            update_info(address);
            break;
        }
    }
    fclose(File);
}



int main(int argc, char *argv[])
{
    parseline(argc, argv);
    Simulate();
    free_Cache();
    printSummary(hit, miss, eviction);
    return 0;
}
