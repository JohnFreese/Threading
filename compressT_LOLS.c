#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
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
	
	
	pthread_attr_t attribute; 
	pthread_attr_init( &attribute );
	void (*functionPtr)(void*);
	functionPtr = &compress;
	pthread_t tid[number_partitions];
	struct Arguments args[number_partitions];
	
	struct timeval t1, t2;
	double elapsedTime;
	
	gettimeofday(&t1, NULL);
	
	for( i = 0; i < number_partitions; ++i )
	{
		(args[i]).filename = argv[1]; 
		(args[i]).name_size = name_size;
		(args[i]).period = period;  
		(args[i]).partition_id = i;
		if( i == 0)
		{
			(args[0]).beggining = 0;
			(args[0]).size = size_partitions+big_partition;
			if( number_partitions == 1 )
			{
				(args[i]).partition_id = -1;
			}
		}
		else
		{
			(args[i]).beggining = i*size_partitions+big_partition;
			(args[i]).size = size_partitions;
		}
		if( size_partitions == 1 && i >= file_size )
		{
			(args[i]).size = 0;
		}
		if( pthread_create( &tid[i], &attribute, (void*)functionPtr, args+i ) )
		{
			printf( "ERROR: pthread_create Failure\n" );
			number_partitions = i;
			break;
		}
	}
	for ( i = 0; i < number_partitions; i++ )
	{
		pthread_join(tid[i], NULL);
	}
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_usec - t1.tv_usec) / 1000.0;
	printf("elapsedTime: %lfms\n", elapsedTime);
	return 0;
}
