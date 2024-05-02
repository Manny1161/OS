#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define ROWS 9
#define COLS 9
#define NUM_CHILDREN 4
#define SUB 9

typedef struct
{
    int child_id;
    int row_st;
    int row_en;
    int col_st;
    int col_en;
    char** argv;
    int* result;
} T_Parameters;


typedef struct
{
    int grid[ROWS][COLS];
} Sol;

void* val_rows(void* arg);
void* val_cols(void *arg);

#endif
