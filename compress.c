#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "compress.h"

void compress( void * a )
{
	struct Arguments args = *((struct Arguments *)a);
	int num_size = 1;
	if( args.partition_id > 10000 )
	{
		num_size = 5;
	}
	else if( args.partition_id > 1000 )
	{
		num_size = 4;
	}
	else if( args.partition_id > 100 )
	{
		num_size = 3;
	}
	else if( args.partition_id > 10 )
	{
		num_size = 2;
	}
	char name[ args.name_size + 5 + num_size + 1 ];
	if( args.partition_id == -1 )
	{
		sprintf( name, "%s_LOLS%c", args.filename, '\0' );
	}
	else
	{
		sprintf( name, "%s_LOLS%d%c", args.filename, args.partition_id, '\0' );
	}
	name[args.period] = '_';
	FILE * start = fopen( args.filename, "r" );
	FILE * finish = fopen( name, "w" );
	fseek( start, args.beggining, SEEK_SET );
	if( args.size == 0 )
	{
		return;
	}
	char symbol;
	int i;
	int amount = 0;
	char work;
	for( i = args.size; i > 0; --i )
	{
		fread( &work, 1, 1, start);
		if( !isalpha( work ) )
		{
			if( !isspace( work ) )
			{
				printf( "ERROR: Non-Compressible Character\n" );
			}
			continue;
		}
		
		if( amount == 0 )
		{
			amount = 1;
			symbol = work;
		}
		else if( symbol != work )
		{
			if( amount == 1 )
			{
				fprintf( finish, "%c", symbol );
			}
			else if( amount == 2 )
			{
				fprintf( finish, "%c%c", symbol, symbol );
			}
			else
			{
				fprintf( finish, "%d%c", amount, symbol );
			}
			amount = 1;
			symbol = work;
		}
		else
		{
			++amount;
		}
	}
	if( amount > 0 )
	{
		if( amount == 1 )
		{
			fprintf( finish, "%c", symbol );
		}
		else if( amount == 2 )
		{
			fprintf( finish, "%c%c", symbol, symbol );
		}
		else
		{
			fprintf( finish, "%d%c", amount, symbol );
		}
	}
	fclose( start );
	fclose( finish );
	pthread_exit(0);
}
