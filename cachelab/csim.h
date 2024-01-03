#pragma once

#define MININT -2147483648

// cache parameters
int numSet;
int associativity;
int blockSize;
char filePath[100];
int verbose = 0;

typedef struct cache_line
{
    int valid;     //有效位
    int tag;       //标记位
    int counter; //计数器
} Cache_line;

typedef struct cache
{
    int S;
    int E;
    int B;
    Cache_line **line;
} Cache;



// final results
int hit;
int miss;
int eviction;
int hit = 0, miss = 0, eviction = 0;
Cache *cache = NULL;

// will be set in getopt() function
extern char *optarg;

// you can define functions here
void usage();
void parseline(int argc, char **argv);
void Creat_Cache();
void free_Cache();
int get_earest_index(int op_s);
int is_full(int op_s);
void update_counter(int i, int op_s, int op_tag);
void update_info(unsigned address);
