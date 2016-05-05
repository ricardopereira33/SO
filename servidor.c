#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
   	
    if(!fork()){
        mkfifo("clientes",0666);
    	char buf[128];
    	int n,sair=1;
    	int pid_pipe ;
    	int pid_log = open("log.txt",O_WRONLY | O_CREAT | O_TRUNC, 0666);
        
        while(sair){
            pid_pipe = open("clientes", O_RDONLY);
    	    while ( (n = read(pid_pipe, buf, 128))>0 )  {
                buf[n]=0;
                if(strcmp(buf,"exit\n")==0){
                    sair=0;
                    break;
                }
                write(pid_log, buf, n);
           }
           close(pid_pipe);
        }   
    	
    	close(pid_log);
    	_exit(0);
    	}

    return 0;
}