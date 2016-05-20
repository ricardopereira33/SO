CC = gcc 

all: servidor cliente

servidor: servidor.c info.c 
	$(CC) servidor.c info.c $(CFLAGS) -o sobusrv

cliente: cliente.c info.c
	$(CC) cliente.c info.c $(CFLAGS) -o sobucli

compile: 
	$(CC) servidor.c info.c $(CFLAGS) -o sobusrv
	$(CC) cliente.c info.c $(CFLAGS) -o sobucli 

.PHONY: run
run: 
	$(CC) servidor.c info.c $(CFLAGS) -o sobusrv
	$(CC) cliente.c info.c $(CFLAGS) -o sobucli 
	./sobusrv

.PHONY: install
install: all
	chmod a+x install.sh
	./install.sh

.PHONY: uninstall
uninstall: 
	chmod a+x uninstall.sh
	./uninstall.sh	

.PHONY: clean
clean:
	rm -f sobusrv
	rm -f sobucli

.PHONY: exit
exit:
	pkill -f sobusrv 

# CFLAGS = -ansi -Wunreachable-code -O2 -Wuninitialized -Wunused-parameter -Wall -Wextra 
#  -pedantic -Wunused-parameter -Wall -Wextra 