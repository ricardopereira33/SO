#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void my_alarm(){
    printf("Copiado\n");
}

void my_alarm2(){
    printf("Recuperado\n");
}

void my_alarm3(){
    printf("Saiu\n");
}

int main(int argc, char** argv) {
    int i,sum,comandSize;
    char* cat;
    char* cat2;
    char buffer[128];
    char destino[128]; 
    strcpy(destino,getenv("HOME"));
    strcat(destino,"/.Backup/pipe");

    int pid_pipe = open(destino, O_WRONLY);
    
    switch(argc){
        case 1: printf("Indique o comando que deseja realizar!\n");
                return 1;
        case 2: if(strcmp(argv[1],"exit")!=0){
                    printf("Indentifique os ficheiros!\n");
                    return 1;
                }
    }

    comandSize=strlen(argv[1]);
    sprintf(buffer, "%d\n", getpid());
    signal(SIGALRM,my_alarm);
    signal(SIGINT,my_alarm2);
    signal(SIGQUIT,my_alarm3);

    for(i=1;argv[i]!=NULL;i++){
        
        if(i==1 && strcmp(argv[1],"exit")==0){
            cat = malloc (comandSize*sizeof(char));
            strcpy(cat,argv[1]);
            strcat(cat,"\n");
        }
        else{
    	    sum=comandSize;
            sum+=strlen(argv[i]);
        
            if(i>2)free(cat);
            cat=NULL;
            cat = malloc (sum*sizeof(char));

            strcpy(cat,argv[1]);
            strcat(cat,"\n");
            strcat(cat,argv[i]);
    	    strcat(cat,"\n");
        }
        if(i>1 || (i==1 && strcmp(argv[1],"exit")==0)){
            cat2 = malloc((strlen(cat)+strlen(buffer))*sizeof(char));
            strcpy(cat2,buffer);
            strcat(cat2,cat);
        
            write(pid_pipe, cat2, strlen(cat2));
            free(cat2);
            cat2=NULL;
            
            if(i>1) printf("%s : ",argv[i]);
            pause();
        }
    }
    free(cat);
    close(pid_pipe);
    return 0;
}