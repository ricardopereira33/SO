#ifndef __SERVIDOR_H__
#define __SERVIDOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "info.h"

#define MAX 5
#define pid_index 0
#define command_index 1
#define file_index 2
#define BUFFER_SIZE 128
#define BLOCK_FILE_SIZE 4096

void delete (char* file);
void restore (char* file,int pid_pipe);
int backup(char* file,char* codigo,int caso);
int exist (char* file, char* dest);
char** readln(char *buf,int *n,char* front);
void criaPastas ();
void copy(char* orig, char *dest);
void fim ();
void chooseComand(INFO info,char* comando,char*filename,int pid_pipe_fork, int caso_restore, int caso_backup);
int checkComandAndFile(INFO info,int sair,int* verifica,int* caso_backup,int* caso_restore,char* destino_data,char* destino_metadata,char* fileName,char* comando,int pid_pipe_fork);

#endif