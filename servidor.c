#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void criaPastas (){
    char destino2[128];
    char destino3[128]; 

    strcpy(destino2,getenv("HOME"));
    strcat(destino2,"/.Backup");
    mkdir(destino2,0777);
    strcpy(destino3,destino2);
    strcat(destino2,"/data");
    mkdir(destino2,0777);
    strcat(destino3,"/metadata");
    mkdir(destino3,0777);
}

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


void backup(char* file){
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
    }
    else printf("Ja tem o backup realizado.\n");
}

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
    	int n,i,pid_pipe;
        /*int numeroUtilizadores=0;*/
    	int sair=1;
        
        while(sair){
            pid_pipe = open(destino, O_RDONLY);
    	    while ( (n = read(pid_pipe, buf, 128))>0 ){
                buf[n]=0;
                /*numeroUtilizadores++;*/
                strcpy(bufcopy,buf);
                buf2=readln(bufcopy,&i,"\n");

                if(strcmp(buf2[1],"exit")==0){
                    sair=0;
                    /*numeroUtilizadores--;*/
                    n=kill(atoi(buf2[0]),SIGQUIT);
                    if(n==-1) printf("Erro\n");
                    break;
                }

                if(strcmp(buf2[1],"backup")==0){
                    backup(buf2[2]);
                    /*numeroUtilizadores--;*/
                    n=kill(atoi(buf2[0]),SIGALRM);
                }

                if(strcmp(buf2[1],"restore")==0){
                    /*usleep(100);*/
                    restore(buf2[2]);
                    /*numeroUtilizadores--;*/
                    n=kill(atoi(buf2[0]),SIGINT);
                }
                if(n==-1) kill(atoi(buf2[0]),SIGHUP);
               
           }
           close(pid_pipe);
        }   
    	_exit(0);
    	}
    return 0;
}