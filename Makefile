CC = gcc 

CFLAGS = -ansi -Wunreachable-code -O2 -Wuninitialized -Wunused-parameter -Wall -Wextra 

compile: 
	$(CC) servidor.c $(CFLAGS) -o /usr/local/bin/sobusrv
	$(CC) cliente.c $(CFLAGS) -o /usr/local/bin/sobucli 

run: 
	$(CC) servidor.c $(CFLAGS) -o /usr/local/bin/sobusrv
	$(CC) cliente.c $(CFLAGS) -o /usr/local/bin/sobucli 
	sobusrv

clean:
	rm -f sobusrv
	rm -f sobucli

exit:
	pkill -f sobusrv 


#  -pedantic -Wunused-parameter -Wall -Wextra 