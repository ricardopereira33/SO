#ifndef __INFO_H__
#define __INFO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 128
#define BLOCK_FILE_SIZE 4096
#define COMAND_SIZE 16
#define CODE_SIZE 256
#define FILE_NAME_SIZE 512

typedef struct info {
	int pidProcesso;
	int fim;
	int tamanho;
	char Codigo[CODE_SIZE];
	char NomeFicheiro[FILE_NAME_SIZE];
	char comando[COMAND_SIZE];
	char Ficheiro[BLOCK_FILE_SIZE];
}*INFO;

typedef struct infoPipe {
	char pipeName[CODE_SIZE];
	char comando[COMAND_SIZE];
	char fileName[BUFFER_SIZE];
	int pidProcesso;
}*INFO_PIPE;

INFO initInfo();
INFO_PIPE initInfoPipe();

#endif