#ifndef DIALFS_H_
#define DIALFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <commons/bitarray.h>
#include <commons/config.h>

extern int block_size;
extern int block_count;

void inicio_filesystem();

#endif