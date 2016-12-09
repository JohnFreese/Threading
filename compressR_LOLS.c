#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/time.h>
#include "compress.h"

int main( int argc, char ** argv )
{
	if( argc != 3 )
	{
		printf( "ERROR: Too few inputs.\n" );
		return 0;
	}
	FILE * file = fopen( argv[1], "r" );
	if( file == NULL )
	{
		printf( "ERROR: File Does Not Exist.\n" );
		return 0;
	}
	int number_partitions = atoi( argv[2] );
	if( number_partitions < 1 )
	{
		printf( "ERROR: Partition Amount not Positive Integer\n" );
		return 0;
	}
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	int size_partitions = file_size/number_partitions;
	if( size_partitions == 0 )
	{
		size_partitions = 1;
	}
	//partition 0 takes remainder
	int big_partition = file_size - size_partitions*number_partitions;
	if( big_partition < 0 )
	{
		big_partition = 0;
	}
	int i;
	int name_size = strlen( argv[1] );
	int period;
	for( period = 0; period < name_size; ++period )
	{
		if( argv[1][period] == '.' )
		{
			break;
		}
	}
	if( period > name_size )
	{
		printf( "ERROR: Incorrect Format Period\n" );
		return 0;
	}
	pid_t pids[ number_partitions ];
	
	struct timeval t1, t2;
	double elapsedTime;
	
	gettimeofday(&t1, NULL);
	
	for( i = 0; i < number_partitions; ++i )
	{
		//pid_t  pid;
		//fork();
		//pid = getpid();
		pid_t  pid = fork();
		//printf( "run %d, # %d, pid %d\n", i , number_partitions, pid );
		if( pid < 0 )
		{
			number_partitions = i;
			printf( "ERROR: Fork Failure\n" );
			break;
		}
		if( pid != 0 )
		{
			pids[i] = pid;
			continue;
		}
		else
		{
			struct Arguments args;
			//call function if child
			args.filename = argv[1]; 
			args.name_size = name_size; 
			args.period = period;  
			args.partition_id = i;
			if( i == 0)
			{
				args.beggining = 0;
				args.size = size_partitions+big_partition;
				if( number_partitions == 1 )
				{
					args.partition_id = -1;
				}
			}
			else
			{
				args.beggining = i*size_partitions+big_partition;
				args.size = size_partitions;
			}
			if( size_partitions == 1 && i >= file_size )
			{
				args.size = 0;
			}
			compress( (void*)(&args) );
			_exit(0);
		}
	}
	//wait(NULL); //tells parent process to wait until child processes finishes
	for (i = 0; i < number_partitions; i++) {
		while( waitpid(pids[i], NULL, 0) > 0 );
	}
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_usec - t1.tv_usec) / 1000.0;
	printf("elapsedTime: %lfms\n", elapsedTime);
	return 0;
}
