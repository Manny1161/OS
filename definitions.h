#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define ROWS 9
#define COLS 9
#define NUM_CHILDREN 4
#define SUB 3
#define ROW_T 3

typedef struct
{
    long child_thread;
    char** argv;
} T_Parameters;


typedef struct
{
    int grid[ROWS][COLS];
} Sol;

void* val_rows(void* arg);
void* val_cols(void *arg);
void* val_subs(void *arg);

#endif
