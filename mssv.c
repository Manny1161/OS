#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "definitions.h"

void* child_threads(void* arg)
{
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
	int grid[ROWS][COLS];
	for(int i=0; i<ROWS; i++)
	{
		for(int j=0; j<COLS; j++)
		{
			/*Check validity of input file*/
			if(fscanf(file, "%d", &grid[i][j]) != 1)
			{
				printf("ERROR: file contains invalid format!\n");
				fclose(file);
				return 1;
			}
		}
	}
	fclose(file);

	pthread_t parent;
	pthread_t child[NUM_CHILDREN];
	int bit;

	/*variable bit used to check if the creation of thread was successful*/
	/*if bit initialised to 0 = successful, else unsuccessful*/
	bit = pthread_create(&parent, NULL, child_threads, (void *)0);
	if(bit)
	{
		printf("ERROR: Unable to create parent thread. Bit = %d\n", bit);
		return 1;
	}
	for(long i=0; i<NUM_CHILDREN; i++)
	{
		bit = pthread_create(&child[i], NULL, child_threads, (void *)(i+1));
		if(bit)
		{
			printf("ERROR: Unable to create child thread. Bit = %d\n", bit);
			return 1;
		}
	}
	pthread_join(parent, NULL);
	for(int i=0; i<NUM_CHILDREN; i++)
	{
		pthread_join(child[i], NULL);
	}
	return 0;
}
