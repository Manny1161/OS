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
	long child_thread = args->child_thread;
	char** argv = args->argv;
	int row_st = child_thread * SUB + 1;
	int row_en = row_st + SUB;
	int valid_rows = 0;

	for(int i=row_st; i<row_en; i++)
	{
		/*implement logic*/
		valid_rows++;
	}

	/*synchronisation mechanims*/
	pthread_mutex_lock(&mutex);
	count += valid_rows;
	pthread_mutex_unlock(&mutex);
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
	long child_thread = args->child_thread;
	char** argv = args->argv;
	int col_st = child_thread * SUB + 1;
	int col_en = col_st + SUB;
	int valid_cols = 0;

	for(int i=col_st; i<col_en; i++)
	{
		/*implement logic*/
		valid_cols++;
	}

	/*synchronisation mechanims*/
	pthread_mutex_lock(&mutex);
	count += valid_cols;
	pthread_mutex_unlock(&mutex);
	sleep(atoi(argv[2]));

	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

/*Sub grid validator function*/
void* val_subs(void *arg)
{
	T_Parameters* args = (T_Parameters*)arg;
	long child_thread = args->child_thread;
	char** argv = args->argv;
	int sub_st = child_thread * SUB + 1;
	int sub_en = sub_st + SUB;
	int valid_subs = 0;

	for(int i=sub_st; i<sub_en; i++)
	{
		/*implement logic*/
		valid_subs++;
	}

	/*synchronisation mechanims*/
	pthread_mutex_lock(&mutex);
	count += valid_subs;
	pthread_mutex_unlock(&mutex);
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
	int bit;
	T_Parameters args[NUM_CHILDREN];

	/*variable bit used to check if the creation of thread was successful*/
	/*if bit initialised to 0 = successful, else unsuccessful*/
	for(long i=0; i<NUM_CHILDREN; i++)
	{
		args[i].child_thread = i;
		args[i].argv = argv;
		if(i<SUB)
		{
			/*Create child thread to validate sub grids*/
			bit = pthread_create(&threads[i], NULL, val_subs, (void *)&args[i]);
		}
		else if(i<SUB + ROW_T)
		{
			/*Create child thread to validate rows*/
			bit = pthread_create(&threads[i], NULL, val_rows, (void *)&args[i]);
		}
		else
		{
			/*Create child thread to validate columns*/
			bit = pthread_create(&threads[i], NULL, val_cols, (void *)&args[i]);
		}
		if(bit)
		{
			printf("ERROR: Unable to create child thread. Bit = %d\n", bit);
			return 1;
		}
	}
	
	pthread_mutex_lock(&mutex);
	while(count<NUM_CHILDREN)
	{
		pthread_cond_wait(&cond, &mutex);
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}
