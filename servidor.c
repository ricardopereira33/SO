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

int numComand = 0;

/**
  * A função fim descrementa a variavél global, numComand. Esta função é utilizada, para alterar um sinal, que indica quando um processo tenha acado, 
  * o que decrementa desta foram o numComand.
  *
  */

void fim (){
    printf("sdvsd\n");
    numComand--;
}

/**
  * A função criaPastas, é a função responsável por criar as pastas onde as cópias de seguranças ( backup's ) vão ficar armazenadas. 
  *
  */

void criaPastas (){

    char backup_data[BUFFER_SIZE];
    char metadata[BUFFER_SIZE]; 
  
    strcpy(backup_data,getenv("HOME"));
    strcat(backup_data,"/.Backup");
    mkdir(backup_data,0777);

    strcpy(metadata,backup_data);
    strcat(backup_data,"/data");
    mkdir(backup_data,0777);
    
    strcat(metadata,"/metadata");
    mkdir(metadata,0777);
}

/**
  * A função readln é uma função auxiliar, que dado uma String, separa-a em várias Strings, criando uma lista com essas Strings. A String original, é 
  * separada na porção de String equivalente a front.
  *
  */

char** readln(char *buf,int *n,char* front){
    char** buff=malloc(32*sizeof(char*));
    int i=0;
    char* token;
    token = strtok(buf, front);
   
    while( token != NULL ) {
      buff[i]=malloc(strlen(token)*sizeof(char));
      strcpy(buff[i],token);
      buff[i][strlen(token)]=0;
      i++;
      token = strtok(NULL, front);
   }
    buff[i]=NULL;
    *n=i-1;
    return buff;
}

/**
  * A função exist verifica se um determinado ficheiro existe numa certa directoria. A função recorre ao auxilio do comando ls.
  *
  */

int exist (char* file, char* dest){
    int pfd[2],n,i;
    char codigo[128];
    char** lista_ficheiros;

    pipe(pfd);
    if(!fork()){
        close(pfd[0]);
        dup2(pfd[1],1);
        execlp("ls","ls",dest,NULL);
        perror("erro");
        _exit(1);
    }
    wait(NULL);
    
    close(pfd[1]);
    dup2(pfd[0],0);
    close(pfd[0]);
    
    read(0,codigo,128);
    codigo[strlen(codigo)]=0;
    
    lista_ficheiros=readln(codigo,&n,"\n");

    for(i=0;lista_ficheiros[i]!=NULL;i++){
        if(!strcmp(lista_ficheiros[i],file)) return 1;
    }
    return 0;
}

/**
  * A função backup, uma das principais do programa, realiza a cópia de segurança de um ficheiro. Para isso, recorre aos comandos sha1sum,gzip,cp,mv e ln. 
  *
  */

int backup(char* file){
    int pfd[2],n;
    char codigo[128];
    char data[BUFFER_SIZE],metadata[BUFFER_SIZE];
    char destino_ficheiro_data[BUFFER_SIZE],destino_codigo[BUFFER_SIZE],destino_ficheiro_metadata[BUFFER_SIZE];
    char** fileName;

    strcpy(data,getenv("HOME"));
    strcat(data,"/.Backup/data/");
    strcpy(metadata,getenv("HOME"));
    strcat(metadata,"/.Backup/metadata/");

    pipe(pfd);
    if(!fork()){
        close(pfd[0]);
        dup2(pfd[1],1);
        execlp("sha1sum","sha1sum",file,NULL);
        perror("erro");
        _exit(1);
    }
    wait(NULL);
    
    close(pfd[1]);
    dup2(pfd[0],0);
    close(pfd[0]);
    
    /* output do sha1sum */
    read(0,codigo,128);
    codigo[strlen(codigo)-1]=0;
    fileName=readln(codigo,&n,"  ");

    strcpy(destino_ficheiro_data,data);
    strcat(destino_ficheiro_data,file);

    if(!fork()){
           execlp("gzip","gzip",destino_ficheiro_data,NULL);
           perror("error");
           _exit(1);
        }
    wait(NULL);

    strcat(destino_ficheiro_data,".gz");
    strcpy(destino_codigo,data);
    strcat(destino_codigo,fileName[0]);

    rename(destino_ficheiro_data,destino_codigo);

    strcpy(destino_ficheiro_metadata,metadata);
    strcat(destino_ficheiro_metadata,fileName[1]);

    if(!fork()){
        execlp("ln","ln",destino_codigo,destino_ficheiro_metadata,NULL);
        perror("error");
        _exit(1);
    }
    wait(NULL);
    return 1;
}

/**
  * A função restore, uma das principais do programa, recupera ficheiros que tenham cópia de segurança.
  *
  */

void restore (char* file){
    char destino[128];
    char fileZip[128];

    strcpy(destino,getenv("HOME"));
    strcat(destino,"/.Backup/metadata/");

    if(!fork()){
        strcat(destino,file);
        execlp("cp","cp",destino,file,NULL);
        perror("erro");
        _exit(1);
    }
    wait(NULL);

    strcpy(fileZip,file);
    strcat(fileZip,".gz");

    if(!fork()){
            execlp("mv","mv",file,fileZip,NULL);
            perror("error");
            _exit(1);
    }
    wait(NULL);

    if(!fork()){
            execlp("gunzip","gunzip",fileZip,NULL);
            perror("error");
            _exit(1);
    }
    wait(NULL);
}

void delete (char* file){
    char destino[128];
    strcpy(destino,getenv("HOME"));
    strcat(destino,"/.Backup/metadata/");

    if(exist(file,destino)){
        strcat(destino,file);
        if(!fork()){
            execlp("rm","rm",destino,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
    }
    else printf("Não existe o ficheiro %s.\n",file);
}

/*void gc (){

}*/

/**
  * A main do programa, que chama todas as funcões, e é responsável por manter o programa a correr em background.
  *
  */

int main(){
    
    criaPastas();

    if(!fork()){
        char destino_pipe[BUFFER_SIZE]; /* destino do pipe */
        char destino_file[BUFFER_SIZE]; /* destino para onde vai o ficheiro */
        char destino_data[BUFFER_SIZE]; /* pasta onde estão os ficheiros */
        strcpy(destino_pipe,getenv("HOME"));
        strcat(destino_pipe,"/.Backup/pipe");
        
        mkfifo(destino_pipe,0666);
    	int n,i,pid_pipe,idFile;


        signal(SIGINT,fim);

        INFO info = initInfo();

        strcpy(destino_data, getenv("HOME"));
        strcat(destino_data,"/.Backup/data");
        pid_pipe = open(destino_pipe,O_RDONLY);
        while(1){
            
            n = read(pid_pipe,info,sizeof(*info));

            if(n!=0){  
                if(info->fim){
                sprintf(destino_file,"%s/%s", destino_data,info->NomeFicheiro);
                idFile = open(destino_file,O_WRONLY | O_CREAT | O_APPEND, 0600);
                write(idFile,info->Ficheiro,info->tamanho);
                close(idFile);
                }
                else{

                    if(strcmp(info->comando,"backup")==0){
                        i=backup(info->NomeFicheiro);
                        if(i)
                            n=kill(info->pidProcesso,SIGALRM);
                        else n=kill(info->pidProcesso,SIGUSR1);
                    }

                    if(strcmp(info->comando,"restore")==0){
                        restore(info->NomeFicheiro);
                        n=kill(info->pidProcesso,SIGINT);
                    }

                    if(strcmp(info->comando,"delete")==0){
                        delete(info->NomeFicheiro);
                        n=kill(info->pidProcesso,SIGUSR2);
                    }

                    if(strcmp(info->comando,"gc")==0){
                        printf("gc\n");
                    }

                    if(n==-1) kill(info->pidProcesso,SIGHUP);
                }
            }
            
        /*    close(pid_pipe); */
        }
    }
    return 0;
}
