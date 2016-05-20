#ifndef __CLIENTE_H__
#define __CLIENTE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "info.h"

#define command_index 1
#define BUFFER_SIZE 128
#define BLOCK_FILE_SIZE 4096

void comandoBackup(INFO_PIPE infoPipe, INFO info,char* destino_pipeAll,int pid_pipe,char* file,char* comando);
void comandoRestore(INFO_PIPE infoPipe, INFO info,char* destino_pipeAll,int pid_pipe,char* file);
void comandoDelete (INFO_PIPE infoPipe,int pid_pipe,char* file);
void comandoGc(INFO_PIPE infoPipe,int pid_pipe);
char* obterCodigo(char* file);
int verificaCmd(char** cmd, int argc);
int exist (char* file);
char** readln(char *buf,char* front);
void mudarSinal ();
void alarmBackup();
void alarmRestore();
void alarmErro();
void alarmVoid();
void alarmVoid2();
void alarmDelete();
void alarmVoid3();
void alarmJaExiste();

#endif