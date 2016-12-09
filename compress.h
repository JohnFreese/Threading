#ifndef COMPRESS_H_
#define COMPRESS_H_

struct Arguments
{
	char * filename; 
	int name_size;
	int period;
	int beggining;
	int size;
	int partition_id ;
};

void compress( void * a );

#endif
