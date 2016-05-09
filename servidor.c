#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

char** readln(char *buf,int *n){
    char** buff=malloc(10*sizeof(char*));
    int i=0;
    char* token;
    token = strtok(buf, "\n");
   
    while( token != NULL ) {
      buff[i]=malloc(strlen(token)*sizeof(char));
      strcpy(buff[i],token);
      buff[i][strlen(token)]=0;
      i++;
      token = strtok(NULL, "\n");
   }
    buff[i]=NULL;
    *n=i-1;
    return buff;
}

int main(int argc, char** argv) {
   	
    if(!fork()){
        mkfifo("/Users/Ricardo/Desktop/clientes",0666);
    	char buf[128];
        char bufcopy[128];
        char** buf2;
    	int n,sair=1,i;
    	int pid_pipe ;
    	int pid_log = open("/Users/Ricardo/Desktop/log.txt",O_WRONLY | O_CREAT | O_TRUNC, 0666);
        
        while(sair){
            pid_pipe = open("/Users/Ricardo/Desktop/clientes", O_RDONLY);
    	    while ( (n = read(pid_pipe, buf, 128))>0 )  {
                buf[n]=0;
                strcpy(bufcopy,buf);
                buf2=readln(bufcopy,&i);

                if(strcmp(buf2[0],"exit")==0){
                    sair=0;
                    kill(atoi(buf2[i]),SIGALRM);
                    break;
                }

                if(strcmp(buf2[0],"backup")==0){
                    printf("backup\n");
                    kill(atoi(buf2[i]),SIGALRM);
                    /*mandar sinal*/
                }

                if(strcmp(buf2[0],"restore")==0){
                    printf("restore\n");
                    kill(atoi(buf2[i]),SIGALRM);
                    /*mandar sinal*/
                }

                //execvp(buf2[0],buf2);
                //write(pid_log, buf, n);

           }
           close(pid_pipe);
        }   
    	
    	close(pid_log);
    	_exit(0);
    	}

    return 0;
}