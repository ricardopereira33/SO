#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char** readln(char *buf){
    char** buff=malloc(10*sizeof(char*));
    int i=0;
    char* token;
    token = strtok(buf, "\n");
   
    /* walk through other tokens */
    while( token != NULL ) {
      buff[i]=malloc(strlen(token)*sizeof(char));
      strcpy(buff[i],token);
      i++;
      token = strtok(NULL, "\n");
   }
    buff[i]=NULL;
    return buff;
}

int main(int argc, char** argv) {
   	
    if(!fork()){
        mkfifo("/Users/Ricardo/Desktop/clientes",0666);
    	char buf[128];
        char bufcopy[128];
        char** buf2;
    	int n,sair=1;
    	int pid_pipe ;
    	int pid_log = open("/Users/Ricardo/Desktop/log.txt",O_WRONLY | O_CREAT | O_TRUNC, 0666);
        
        while(sair){
            pid_pipe = open("/Users/Ricardo/Desktop/clientes", O_RDONLY);
    	    while ( (n = read(pid_pipe, buf, 128))>0 )  {
                buf[n]=0;
                strcpy(bufcopy,buf);
                buf2=readln(bufcopy);

                if(strcmp(buf,"exit\n")==0){
                    sair=0;
                    break;
                }
                //execvp(buf2[0],buf2);
                write(pid_log, buf, n);
           }
           close(pid_pipe);
        }   
    	
    	close(pid_log);
    	_exit(0);
    	}

    return 0;
}