#include "cliente.h"

int acabou=0;
int existe=0;

/**
  * A main responsável por aplicar todas as funções.
  *
  */

int main(int argc, char** argv) {
	int i,pid_pipe;
	char destino[BUFFER_SIZE]; 
	char destino_pipeAll[BUFFER_SIZE];
	char pipName[BUFFER_SIZE];
	INFO info; 
	
	strcpy(destino,getenv("HOME"));
	strcat(destino,"/.Backup/pipe");

	strcpy(destino_pipeAll,getenv("HOME"));
	strcat(destino_pipeAll,"/.Backup/");

	/*Casos invalidos*/
	if(argc==1) {
		printf("Falta o comando.\n");
		return 1;
	}

	if( strcmp(argv[command_index],"restore")!=0 && !verificaCmd(argv+1,argc) ){
		return 1;
	}

   	pid_pipe = open(destino, O_WRONLY);

	mudarSinal();

	for(i=1;argv[i]!=NULL;i++){
		
		if(!fork()){
			info = initInfo(); 
			INFO_PIPE infoPipe= initInfoPipe();
			sprintf(pipName,"%d",getpid());
			strcat(destino_pipeAll,pipName);
			strcpy(infoPipe->pipeName,destino_pipeAll);
			strcpy(infoPipe->comando,argv[command_index]);
			strcpy(infoPipe->fileName,argv[i]);
			infoPipe->pidProcesso=getpid();
			mkfifo(destino_pipeAll,0666);

			if(!strcmp(argv[command_index],"restore") || !strcmp(argv[command_index],"gc") || !strcmp(argv[command_index],"delete") || (exist(argv[i]) || i==1)){ 

				if(strcmp(argv[command_index],"backup")==0 && i!=1){

						comandoBackup(infoPipe,info,destino_pipeAll,pid_pipe,argv[i],argv[1]);
				}
				else if(strcmp(argv[command_index],"restore")==0 && i!=1 ){

						comandoRestore(infoPipe,info,destino_pipeAll,pid_pipe,argv[i]);
				}

				else if(strcmp(argv[command_index],"delete")==0 && i!=1 ){

						comandoDelete(infoPipe,pid_pipe,argv[i]);
				}
				else if(strcmp(argv[command_index],"gc")==0 && argc==2){
					
					comandoGc(infoPipe,pid_pipe);
				}
				
				else if(i!=1) printf("Comando inválido.\n");

			}
			else printf("Ficheiro %s não existe.\n",argv[i]);
			
			unlink(destino_pipeAll);
			_exit(0);
		}
	}

	for(i=1;argv[i]!=NULL;i++){
		wait(NULL);
	}

	close(pid_pipe);
	return 0;
}

void comandoBackup(INFO_PIPE infoPipe, INFO info,char* destino_pipeAll,int pid_pipe,char* file,char* comando){
	int vazio,idFile,pid_pipe_fork,tamanho;
	char buffer[BLOCK_FILE_SIZE];

	write(pid_pipe,infoPipe,sizeof(*infoPipe));

	pid_pipe_fork=open(destino_pipeAll,O_WRONLY);

	idFile = open(file,O_RDONLY);
				
	strcpy(info->Codigo,obterCodigo(file));
	vazio=0;

	while((tamanho=read(idFile,buffer,BLOCK_FILE_SIZE))>0){
		vazio++;
		memcpy(info->Ficheiro,buffer,tamanho);
		info->tamanho=tamanho;
		info->pidProcesso=getpid();
		strcpy(info->NomeFicheiro,file);
		strcpy(info->comando,comando);
		info->fim=1;
		write(pid_pipe_fork, info, sizeof(*info));
		memset(buffer,0,BLOCK_FILE_SIZE);
	}
	if(vazio){
		info->fim=0;
		info->tamanho=0;
		info->pidProcesso=getpid();
		strcpy(info->NomeFicheiro,file);
		strcpy(info->comando,comando);
		write(pid_pipe_fork,info,sizeof(*info));

		close(idFile);
				
		printf("%s : ",file);
		pause();
	}
	else printf("Ficheiro Vazio.\n");
	close(pid_pipe_fork);
}


void comandoRestore(INFO_PIPE infoPipe, INFO info,char* destino_pipeAll,int pid_pipe,char* file){
	int pid_pipe_fork,tamanho_restore,idFile;

	write(pid_pipe,infoPipe,sizeof(*infoPipe));

	pid_pipe_fork=open(destino_pipeAll,O_RDONLY);

	while((tamanho_restore=read(pid_pipe_fork,info,sizeof(*info)))>0){

		if(tamanho_restore!=0){
			if(info->fim){
				idFile = open(file,O_WRONLY | O_CREAT | O_APPEND, 0666);
				write(idFile,info->Ficheiro,info->tamanho);
				close(idFile);
			}
		}
		else close(pid_pipe_fork);
	}

	printf("%s : Recuperado.\n",file);

}

void comandoDelete (INFO_PIPE infoPipe,int pid_pipe,char* file){

	write(pid_pipe,infoPipe,sizeof(*infoPipe));

	printf("%s : ",file);
	pause();
}

void comandoGc(INFO_PIPE infoPipe,int pid_pipe){

	write(pid_pipe,infoPipe,sizeof(*infoPipe));

	pause();
}

char* obterCodigo(char* file){
	int pfd[2];
	char codigo[BUFFER_SIZE];
	char**fileName;
	pipe(pfd);
    if(!fork()){
        close(pfd[0]);
        dup2(pfd[1],1);
        execlp("sha1sum","sha1sum",file,NULL);
        perror("erro");
        _exit(1);
    }
    wait(NULL);
    
    close(pfd[1]);
    dup2(pfd[0],0);
    close(pfd[0]);
    
    /* output do sha1sum */
    read(0,codigo,BUFFER_SIZE);
    codigo[strlen(codigo)-1]=0;
    fileName=readln(codigo,"  ");

    return fileName[0];
}


/**
  * A função readln é uma função auxiliar, que dado uma String, separa-a em várias Strings, criando uma lista com essas Strings. A String original, é 
  * separada na porção de String equivalente a front.
  *
  */

char** readln(char *buf,char* front){
	char** buff=malloc(BUFFER_SIZE*sizeof(char*));
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
	return buff;
}

/**
  * A função exist verifica se um determinado ficheiro existe na directoria atual. A função recorre ao auxilio do comando ls.
  *
  */

int exist (char* file){
	int pfd[2],i;
	char codigo[BLOCK_FILE_SIZE];
	char** aux;

	pipe(pfd);
	if(!fork()){
		close(pfd[0]);
		dup2(pfd[1],1);
		execlp("ls","ls",NULL);
		perror("erro");
		_exit(0);
	}
	wait(NULL);
	
	close(pfd[1]);
	dup2(pfd[0],0);
	close(pfd[0]);
	
	read(0,codigo,BLOCK_FILE_SIZE);
	codigo[strlen(codigo)]=0;
	aux=readln(codigo,"\n");

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
		
		case 2: if(!strcmp(cmd[cmd_index],"gc")){
					return 1;
				}
				else if(comp ){ 
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

void mudarSinal (){
	signal(SIGALRM,alarmBackup);
	signal(SIGINT,alarmRestore);
	signal(SIGHUP,alarmNaoExiste); 
	signal(SIGUSR1,alarmVoid);
	signal(SIGUSR2,alarmDelete);
	signal(SIGFPE,alarmVoid2);
 }


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

void alarmNaoExiste(){
	printf("Não existe.\n");
}

void alarmVoid(){
	printf("Ja tem o backup realizado.\n");
}

void alarmVoid2(){
	printf("Nao existe backup do ficheiro.\n");
}

void alarmDelete(){
	printf("Apagado\n");
}

