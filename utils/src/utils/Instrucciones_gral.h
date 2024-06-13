
#ifndef INS_GRAL_H
#define INS_GRAL_H

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include "mensajesPropios.h"
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

typedef enum {
    SET,
    MOV_IN,
    MOV_OUT, 
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIIT
} instrucciones;

#endif