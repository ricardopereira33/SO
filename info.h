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
	char NomeFicheiro[512];
	char comando[10];
	char Ficheiro[4096];
}*INFO;

INFO initInfo();

#endif