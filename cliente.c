#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char** argv) {
    int i,sum=0,n;
    char* cat;
    int pid_pipe = open("/Users/Ricardo/Desktop/clientes", O_WRONLY);

    for(i=1;argv[i]!=NULL;i++){
    	sum+=strlen(argv[i]);
    }

    cat = malloc (sum*sizeof(char));

    for(i=1;argv[i]!=NULL;i++){
    	strcat(cat,argv[i]);
    	strcat(cat,"\n");
    }
    
    write(pid_pipe, cat, strlen(cat));

   	free(cat);
    close(pid_pipe);
    return 0;
}