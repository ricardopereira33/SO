CC = gcc 

CFLAGS = -ansi -Wunreachable-code -O2 -Wuninitialized -Wunused-parameter -Wall -Wextra 

compile: 
	$(CC) servidor.c $(CFLAGS) -o sobusrv
	$(CC) cliente.c $(CFLAGS) -o sobucli 

run: 
	$(CC) servidor.c $(CFLAGS) -o sobusrv
	$(CC) cliente.c $(CFLAGS) -o sobucli 
	./sobusrv

clean:
	rm -f sobusrv
	rm -f sobucli

exit:
	pkill -f ./sobusrv 


#  -pedantic -Wunused-parameter -Wall -Wextra 