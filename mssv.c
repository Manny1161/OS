#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "definitions.h"

/*Shared variables used by threads*/
Sol sudoku;
int Row[ROWS + 1] = {0};
int Col[COLS + 1] = {0};
int Sub[SUB + 1] = {0};
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/*Row validator function*/
void* val_rows(void* arg)
{
	T_Parameters* args = (T_Parameters*)arg;
	char** argv = args->argv;
	int row_st = args->row_st;
	int row_en = args->row_en;
	int* result = args->result;
	int valid_rows = 0;
	int valid_subs = 0;
	/*validate rows*/
	for(int i=row_st; i<=row_en; i++)
	{
		int row_set[ROWS + 1] = {0};
		for(int j=0;j<ROWS;j++)
		{
			int val = sudoku.grid[i-1][j];
			if(val<1 || val>9 || row_set[val]==1)
			{
				*result = 0;
				pthread_exit(NULL);
			}
			row_set[val] = 1;
		}
		valid_rows++;
		Row[i] = 1;
	}

	for (int i = row_st; i <= row_en; i += 3)
    {
        for (int j = 0; j < COLS; j += 3)
        {
            int sub_set[COLS + 1] = {0};
            for (int r = i; r < i + 3; r++)
            {
                for (int c = j; c < j + 3; c++)
                {
                    int val = sudoku.grid[r - 1][c];
                    if (val < 1 || val > 9 || sub_set[val] == 1)
                    {
                        *result = 0;
                        pthread_exit(NULL);
                    }
                    sub_set[val] = 1;
                }
            }
            valid_subs++;
        }
    }

	/*synchronisation mechanims*/
	pthread_mutex_lock(&mutex);
	count += valid_rows;
	count+= valid_subs;
	pthread_mutex_unlock(&mutex);

	/*Valid*/
	*result = 1;
	sleep(atoi(argv[2]));

	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

/*Column validator function*/
void*  val_cols(void *arg)
{
	T_Parameters* args = (T_Parameters*)arg;
	char** argv = args->argv;
	int col_st = args->col_st;
	int col_en = args->col_en;
	int *result = args->result;
	int valid_cols = 0;
	for(int i=col_st; i<=col_en; i++)
	{
		int col_set[COLS + 1] = {0};
		for(int j=0;j<COLS;j++)
		{
			int val = sudoku.grid[j][i-1];
			/*Invalid*/
			if(val<1 || val>9 || col_set[val] == 1)
			{
				*result = 0;
				pthread_exit(NULL);
			}
			col_set[val] = 1;
		}
		valid_cols++;
	}


	/*synchronisation mechanims*/
	pthread_mutex_lock(&mutex);
	for(int i=col_st;i<=col_en;i++)
	{
		*result = 1;
	}
	count += valid_cols;
	pthread_mutex_unlock(&mutex);
	
	/*Valid
	*result = 1;*/
	sleep(atoi(argv[2]));

	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("USAGE: %s <input_file> <integer_between_1-10>\n", argv[0]);
		return 1;	
	}
	FILE *file = fopen(argv[1], "r");
	if(file == NULL)
	{
		printf("ERROR: Could not open file %s\n", argv[1]);
		return 1;
	}
	if( atoi(argv[2]) < 1 || atoi(argv[2]) > 10)
	{
		printf("ERROR: Integer must be between 1 and 10!\n");
		return 1;
	}
	/*read initial input file (9x9) grid*/
	for(int i=0; i<ROWS; i++)
	{
		for(int j=0; j<COLS; j++)
		{
			/*Check validity of input file*/
			if(fscanf(file, "%d", &sudoku.grid[i][j]) != 1)
			{
				printf("ERROR: file contains invalid format!\n");
				fclose(file);
				return 1;
			}
		}
	}
	fclose(file);

	pthread_t threads[NUM_CHILDREN];
	T_Parameters args[NUM_CHILDREN];
	int result[NUM_CHILDREN] = {0};

	int index = 0;
	for(int i=0;i<NUM_CHILDREN;i++)
	{
		if(i<3)
		{
			args[i].row_st = index * 3 + 1;
			args[i].row_en = (index + 1) * 3;
			args[i].argv = argv;
			args[i].child_id = i+1;
			args[i].result = &result[i];
			index++;
			pthread_create(&threads[i], NULL, val_rows, (void *)&args[i]);
		}
		else
		{
			args[i].col_st = 1;
			args[i].col_en = COLS;
			args[i].argv = argv;
			args[i].child_id = i+1;
			args[i].result = &result[i];
			pthread_create(&threads[i], NULL, val_cols, (void *)&args[i]);
		}
	}

	int last_child;
	for(int i=0;i<NUM_CHILDREN;i++)
	{
		pthread_join(threads[i], NULL);
		if(result[i] == 1)
		{
			last_child = i+1;
		}
	}

	/*Print results*/
	for(int i=0;i<NUM_CHILDREN;i++)
	{
		if(result[i] == 1)
		{
			printf("Thread ID-%d: valid\n", i+1);
		}
		else
		{
			if(i<3)
			{
				printf("Thread ID-%d: row %d, row %d are invalid\n", i+1, args[i].row_st, args[i].row_en);
			}
			else
			{
				printf("Thread ID-%d: column %d is invalid\n", i+1, args[i].col_st);
			}
		}
	}

	printf("There are %d valid sub-grids, and thus the solution is %s.\n", count, (count == 27) ? "valid" : "invalid");
	printf("Thread ID-%d is the last thread\n", last_child);

	return 0;
}
