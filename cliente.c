#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void my_alarm(){
    printf("Acabou\n");
}

int main(int argc, char** argv) {
    int i,sum=0,n;
    char* cat;
    char buffer[128];
    int pid_pipe = open("/Users/Ricardo/Desktop/clientes", O_WRONLY);

    for(i=1;argv[i]!=NULL;i++){
    	sum+=strlen(argv[i]);
    }

    cat = malloc ((sum+strlen(buffer))*sizeof(char));

    for(i=1;argv[i]!=NULL;i++){
    	strcat(cat,argv[i]);
    	strcat(cat,"\n");
    }

    sprintf(buffer, "%d\n", getpid());

    strcat(cat,buffer);
    
    write(pid_pipe, cat, strlen(cat));
    
    signal(SIGALRM,my_alarm);
    
    pause();

   	free(cat);
    close(pid_pipe);
    return 0;
}