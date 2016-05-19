#ifndef __INFO_H__
#define __INFO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct info {
	int pidProcesso;
	int fim;
	int tamanho;
	char Codigo[256];
	char NomeFicheiro[512];
	char comando[16];
	char Ficheiro[4096];
}*INFO;

typedef struct infoPipe {
	char pipeName[256];
	char comando[16];
	char fileName[128];
}*INFO_PIPE;

INFO initInfo();
INFO_PIPE initInfoPipe();

#endif