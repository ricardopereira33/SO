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
    numComand--;
}

void copy(char* orig, char *dest){
    int n;
    int file_orig;
    int file_dest;
    char buffer[1024];

    file_orig = open(orig,O_RDONLY);
    file_dest = open(dest,O_WRONLY | O_CREAT | O_APPEND, 0600);
    while((n=read(file_orig,buffer,1024)))
        write(file_dest,buffer,n);
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
    char** buff=malloc(BUFFER_SIZE*sizeof(char*));
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
        _exit(0);
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

int backup(char* file,char* codigo,int caso){

    char data[BUFFER_SIZE],metadata[BUFFER_SIZE];
    char destino_ficheiro_data[BUFFER_SIZE],destino_codigo[BUFFER_SIZE],destino_ficheiro_metadata[BUFFER_SIZE];

    strcpy(data,getenv("HOME"));
    strcat(data,"/.Backup/data/");
    strcpy(metadata,getenv("HOME"));
    strcat(metadata,"/.Backup/metadata/");


    if(caso==1){
        strcpy(destino_codigo,data);
        strcat(destino_codigo,codigo);

        strcpy(destino_ficheiro_metadata,metadata);
        strcat(destino_ficheiro_metadata,file);

        if(!fork()){
            execlp("ln","ln",destino_codigo,destino_ficheiro_metadata,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
        return 1;
    }
    else if(caso==2 || caso==3){
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
        strcat(destino_codigo,codigo);

        rename(destino_ficheiro_data,destino_codigo);

        strcpy(destino_ficheiro_metadata,metadata);
        strcat(destino_ficheiro_metadata,file);

        if(caso==2) unlink(destino_ficheiro_metadata);

        if(!fork()){
            execlp("ln","ln","-s",destino_codigo,destino_ficheiro_metadata,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
        return 1;
        }
    return 0;
}

/**
  * A função restore, uma das principais do programa, recupera ficheiros que tenham cópia de segurança.
  *
  */

void restore (char* file,int pid_pipe){

    int  idFile, tamanho;
    char ficheiro_metadata[BUFFER_SIZE];
    char ficheiro_zip[BUFFER_SIZE];
    char ficheiro_zip_2[BUFFER_SIZE];
    char buffer[4096];

    strcpy(ficheiro_metadata,getenv("HOME"));
    strcat(ficheiro_metadata,"/.Backup/metadata/");

    strcat(ficheiro_metadata,file);
    
    tamanho = readlink(ficheiro_metadata,ficheiro_zip, BUFFER_SIZE);
    
    ficheiro_zip[tamanho]=0;
    
    strcpy(ficheiro_zip_2,ficheiro_zip);
    strcat(ficheiro_zip,".gz");
    
    rename(ficheiro_zip_2,ficheiro_zip);
    
    if(!fork()){
            execlp("gunzip","gunzip",ficheiro_zip,NULL);
            perror("error");
            _exit(1);
    }
    wait(NULL);

    INFO info = initInfo();
    idFile = open(ficheiro_zip, O_RDONLY);

    while((tamanho=read(idFile,buffer,4096))>0){
        memcpy(info->Ficheiro,buffer,tamanho);
        info->tamanho=tamanho;
        info->pidProcesso=getpid();
        strcpy(info->NomeFicheiro,file);
        strcpy(info->comando,"restore");
        info->fim=1;
        write(pid_pipe, info,sizeof(*info));
    } 

    info->fim=0;
    info->tamanho=0;
    info->pidProcesso=getpid();
    strcpy(info->NomeFicheiro,file);
    strcpy(info->comando,"restore");
    write(pid_pipe,info,sizeof(*info)); 

    close(pid_pipe);
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
        char destino_pipe2[BUFFER_SIZE]; /* destino do pipe */
        char destino_file[BUFFER_SIZE]; /* destino para onde vai o ficheiro */
        char destino_data[BUFFER_SIZE]; /* pasta onde estão os ficheiros */
        char destino_metadata[BUFFER_SIZE];
        strcpy(destino_pipe,getenv("HOME"));
        strcat(destino_pipe,"/.Backup/pipe");
        strcpy(destino_pipe2,getenv("HOME"));
        strcat(destino_pipe2,"/.Backup/pipe2");
        
        mkfifo(destino_pipe,0666);
        mkfifo(destino_pipe2,0666);

    	int n,i,pid_pipe,pid_pipe2,idFile,caso_backup,caso_restore,verifica=1;

        signal(SIGINT,fim);

        INFO info = initInfo();
        info->fim=0;
        strcpy(destino_data, getenv("HOME"));
        strcat(destino_data,"/.Backup/data");

        strcpy(destino_metadata,getenv("HOME"));
        strcat(destino_metadata,"/.Backup/metadata");
        
        pid_pipe = open(destino_pipe,O_RDONLY);

        char existCodigo[BUFFER_SIZE];
        char existFile[BUFFER_SIZE];

        while(1){
            if(numComand>=MAX){
                pause();
            }
            
            n = read(pid_pipe,info,sizeof(*info));
            /*printf("PIPE ->%d\n",info->fim);*/
            if(n!=0){
             
                if(verifica && !strcmp(info->comando,"backup")){
                    strcpy(existCodigo,destino_data);
                    strcat(existCodigo,"/");
                    strcat(existCodigo,info->Codigo);
                  
                    strcpy(existFile,destino_metadata);
                    strcat(existFile,"/");
                    strcat(existFile,info->NomeFicheiro);

                    if(access(existCodigo, F_OK)==0 && access(existFile,F_OK)==-1) caso_backup=1;
                
                    else if(access(existFile, F_OK)==0 && access(existCodigo,F_OK)==-1 ) caso_backup=2;

                    else if(access(existCodigo,F_OK)==-1 && access(existFile,F_OK)==-1 ) caso_backup=3;

                    else caso_backup=4; 

                    verifica=0;
                }
                if(verifica && !strcmp(info->comando,"restore")){
                    strcpy(existFile,destino_metadata);
                    strcat(existFile,"/");
                    strcat(existFile,info->NomeFicheiro);

                    if(access(existFile,F_OK)==0)  caso_restore=1;
                    else caso_restore=0;
                    verifica=0;
                }
               
                if(info->fim  && (caso_backup==2 || caso_backup==3 || caso_backup==4) && !strcmp(info->comando,"backup")){
                    if(caso_backup!=4){
                        sprintf(destino_file,"%s/%s", destino_data,info->NomeFicheiro);
                        idFile = open(destino_file,O_WRONLY | O_CREAT | O_APPEND, 0600);
                        write(idFile,info->Ficheiro,info->tamanho);
                        close(idFile);
                    }
                }
                else {
                 /*printf("%d\n",caso_backup);*/
                  numComand++;
                  if(!fork()){
                    if(strcmp(info->comando,"backup")==0){
                        i=backup(info->NomeFicheiro,info->Codigo,caso_backup);
                        if(i)
                            n=kill(info->pidProcesso,SIGALRM);
                        else n=kill(info->pidProcesso,SIGUSR1);
                    }

                    if(strcmp(info->comando,"restore")==0){
                        if(caso_restore){
                            pid_pipe2=open(destino_pipe2,O_WRONLY);
                            restore(info->NomeFicheiro,pid_pipe2);
                            /*n=kill(info->pidProcesso,SIGINT);*/
                        }
                        /*else n=kill(info->pidProcesso,SIGINT);*/
                    }

                    if(strcmp(info->comando,"delete")==0){
                        delete(info->NomeFicheiro);
                        n=kill(info->pidProcesso,SIGUSR2);
                    }

                    if(strcmp(info->comando,"gc")==0){
                        printf("gc\n");
                    }

                    if(n==-1) kill(info->pidProcesso,SIGHUP);
                    
                    kill(getppid(),SIGINT);
                    _exit(1);
                    }
                }
            }
            else{
                close(pid_pipe);
                pid_pipe = open(destino_pipe,O_RDONLY);
                verifica=1;
                /*caso_backup=0;*/
            }

        }
    
    }
    return 0;
}
