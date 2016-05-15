#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define command_index 1

/**
  * A função alarmBackup, imprime o aviso que um ficheiro foi copiado. Provém de um sinal.
  *
  */

void alarmBackup(){
    printf("Copiado\n");
}

/**
  * A função alarmBackup, imprime o aviso que um ficheiro foi recuperado. Provém de um sinal.
  *
  */

void alarmRestore(){
    printf("Recuperado\n");
}

/**
  * A função alarmBackup, imprime o aviso que ocorreu um erro. Provém de um sinal.
  *
  */

void alarmErro(){
    printf("Erro\n");
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
  * A função exist verifica se um determinado ficheiro existe na directoria atual. A função recorre ao auxilio do comando ls.
  *
  */

int exist (char* file){
    int pfd[2],n,i;
    char codigo[128];
    char** aux;

    pipe(pfd);
    if(!fork()){
        close(pfd[0]);
        dup2(pfd[1],1);
        execlp("ls","ls",NULL);
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
  * A função verficaCmd verifca se o input, os argumentos, estão válidos.
  *
  */

int verificaCmd(char** cmd, int argc){
    int cmd_index=0;
    int comp = (strcmp(cmd[cmd_index],"backup")==0||strcmp(cmd[cmd_index],"restore")==0||strcmp(cmd[cmd_index],"delete")==0||strcmp(cmd[cmd_index],"gc")==0);
    
    switch(argc){
        case 1: printf("Indique o comando que deseja realizar!\n");
                return 0;
        
        case 2: if(comp){ 
                    printf("Indentifique os ficheiros!\n");
                }
                else printf("Comando não existe.\n");
                return 0;
        
        default: if(comp){ 
                        return 1;
                 }
                 else { 
                    printf("Comando não existe.\n");
                    return 0;
                }
    }

}

/**
  * A main responsável por aplicar todas as funções.
  *
  */

int main(int argc, char** argv) {
    int i,sum,comandSize;
    char* cat=NULL;
    char* cat2;
    char buffer[128];
    char destino[128]; 
    strcpy(destino,getenv("HOME"));
    strcat(destino,"/.Backup/pipe");

   int pid_pipe = open(destino, O_WRONLY);
    
    /*Casos invalidos*/
    if(!verificaCmd(argv+1,argc)){
        return 1;
    }

    comandSize=strlen(argv[1]);
    
    signal(SIGALRM,alarmBackup);
    signal(SIGINT,alarmRestore);
    signal(SIGHUP,alarmErro);

    for(i=2;argv[i]!=NULL;i++){
        /*if(!fork()){*/
           
    	   sum=comandSize;
           sum+=strlen(argv[i]);
        
            if(i>2)
                free(cat);
            cat=NULL;
            cat = malloc (sum*sizeof(char));

            strcpy(cat,argv[command_index]);
            strcat(cat,"\n");

            if(exist(argv[i])){ 
                strcat(cat,argv[i]);
                strcat(cat,"\n"); 
        
                sprintf(buffer, "%d\n", getpid());
                cat2 = malloc((strlen(cat)+strlen(buffer))*sizeof(char));
                strcpy(cat2,buffer);
                strcat(cat2,cat);
        
                write(pid_pipe, cat2, strlen(cat2));
                free(cat2);
                cat2=NULL;
            
                printf("%s : ",argv[i]);
                pause();
            }
            else printf("Ficheiro %s não existe.\n",argv[i]);

           /* _exit(0);
        }*/
    }

    /*for(i=2;argv[i]!=NULL;i++){
        wait(NULL);
    }*/
    free(cat);
    close(pid_pipe);
    return 0;
}