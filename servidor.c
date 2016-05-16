#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define MAX 5
#define pid_index 0
#define command_index 1
#define file_index 2

int numComand=0;

/**
  * A função fim descrementa a variavél global, numComand. Esta função é utilizada, para alterar um sinal, que indica quando um processo tenha acado, 
  * o que decrementa desta foram o numComand.
  *
  */

void fim (){
    numComand--;
}

/**
  * A função criaPastas, é a função responsável por criar as pastas onde as cópias de seguranças ( backup's ) vão ficar armazenadas. 
  *
  */

void criaPastas (){
    char destino2[128];
    char destino3[128]; 
   /* int pfd[2];
    char codigo[128];*/

    strcpy(destino2,getenv("HOME"));
    strcat(destino2,"/.Backup");

    /*if(!fork()){
        pipe(pfd);
        if(!fork()){
            close(pfd[0]);
            dup2(pfd[1],1);
            execlp("find","find",destino2,NULL);
            perror("erro");
            _exit(1);
        }
        wait(NULL);
    
        close(pfd[1]);
        dup2(pfd[0],0);
        close(pfd[0]);

        read(0,codigo,128);
    
        if(strlen(codigo)!=0){
            if(!fork()){
                execlp("mkdir","mkdir",destino2,NULL);
                perror("error");
                _exit(1);
            }
            wait(NULL);*/
            mkdir(destino2,0777);
            strcpy(destino3,destino2);
            strcat(destino2,"/data");
            /*if(!fork()){
                execlp("mkdir","mkdir",destino2,NULL);
                perror("error");
                _exit(1);
            }
            wait(NULL);*/
            mkdir(destino2,0777);
            strcat(destino3,"/metadata");
            /*if(!fork()){
                execlp("mkdir","mkdir",destino3,NULL);
                perror("error");
                _exit(1);
            }
            wait(NULL);*/
            mkdir(destino3,0777);
     /*       }
    _exit(1);
    }
    wait(NULL);*/
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
    char** aux;

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
    
    aux=readln(codigo,&n,"\n");

    for(i=0;aux[i]!=NULL;i++){
        if(!strcmp(aux[i],file)) return 1;
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
    char destino1[128],destino2[128],aux[128],aux2[128],aux3[128];
    char** fileName;

    strcpy(destino1,getenv("HOME"));
    strcat(destino1,"/.Backup/data/");
    strcpy(destino2,getenv("HOME"));
    strcat(destino2,"/.Backup/metadata/");

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
    
    fgets(codigo,128,stdin);
    codigo[strlen(codigo)-1]=0;
    
    fileName=readln(codigo,&n,"  ");

    if(exist(fileName[0],destino1) && !exist(fileName[1],destino2)){
        
        strcpy(aux2,destino1);
        strcat(aux2,fileName[0]);

        strcpy(aux3,destino2);
        strcat(aux3,fileName[1]);

        if(!fork()){
            execlp("ln","ln",aux2,aux3,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
        return 1;
    }
    else if(!exist(fileName[0],destino1)){
        /*meter em auxiliar devia ficar melhor*/
        if(!fork()){
            execlp("cp","cp",file,destino1,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);

        strcpy(aux,destino1);
        strcat(aux,file);

        if(!fork()){
            execlp("gzip","gzip",aux,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);

        strcat(aux,".gz");
        strcpy(aux2,destino1);
        strcat(aux2,fileName[0]);

        if(!fork()){
            execlp("mv","mv",aux,aux2,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);

        strcpy(aux3,destino2);
        strcat(aux3,fileName[1]);

        if(!fork()){
            execlp("ln","ln",aux2,aux3,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
        return 1;
    }
    else return 0;

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

int main() {
    
    criaPastas();

    if(!fork()){
        char destino[128];
        strcpy(destino,getenv("HOME"));
        strcat(destino,"/.Backup/pipe");
        mkfifo(destino,0666);

    	char buf[128];
        char bufcopy[128];
        char** buf2;
    	int n,i,info,pid_pipe;

        signal(SIGINT,fim);

        while(1){
            if(numComand >= MAX){
                pause();
            }
            else {
                pid_pipe = open(destino, O_RDONLY);
                /*devo ter while ou if????*/
    	        while ( (n = read(pid_pipe, buf, 128))>0 ){
                    if(!fork()){
                    numComand++;
                    buf[n]=0;
                    strcpy(bufcopy,buf);
                    buf2=readln(bufcopy,&i,"\n");

                    if(strcmp(buf2[command_index],"backup")==0){
                        info=backup(buf2[file_index]);
                        /*sleep(3);*/
                        if(info)
                            n=kill(atoi(buf2[pid_index]),SIGALRM);
                        else n=kill(atoi(buf2[pid_index]),SIGUSR1);
                    }

                    if(strcmp(buf2[command_index],"restore")==0){
                        restore(buf2[file_index]);
                        n=kill(atoi(buf2[pid_index]),SIGINT);
                    }

                    if(strcmp(buf2[command_index],"delete")==0){
                        delete(buf2[file_index]);
                        n=kill(atoi(buf2[pid_index]),SIGUSR2);
                    }

                    if(strcmp(buf2[command_index],"gc")==0){
                        printf("gc\n");
                    }

                    if(n==-1) kill(atoi(buf2[pid_index]),SIGHUP);
                    
                    kill(getppid(),SIGINT);
                    _exit(0);
            }
          }
           close(pid_pipe);
          }
        }
        /*fazer wait's*/  
    	/*_exit(0);*/
    }
    return 0;
}
