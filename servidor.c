#include "servidor.h"

int numComand = 0;


/**
  * A main do programa, que chama todas as funcões, e é responsável por manter o programa a correr em background.
  *
  */

int main(){
    
    criaPastas();

    if(!fork()){
        char destino_pipe[BUFFER_SIZE]; /* destino do pipe */ /* destino para onde vai o ficheiro */
        char destino_data[BUFFER_SIZE]; /* pasta onde estão os ficheiros */
        char destino_metadata[BUFFER_SIZE];
        strcpy(destino_pipe,getenv("HOME"));
        strcat(destino_pipe,"/.Backup/pipe");
        
        mkfifo(destino_pipe,0666);

        int n,pid_pipe,caso_backup,caso_restore,verifica=1;

        signal(SIGINT,fim);

        INFO info = initInfo();
        info->fim=0;
        strcpy(destino_data, getenv("HOME"));
        strcat(destino_data,"/.Backup/data");

        strcpy(destino_metadata,getenv("HOME"));
        strcat(destino_metadata,"/.Backup/metadata");
        
        pid_pipe = open(destino_pipe,O_RDONLY);

        INFO_PIPE infoPipe = initInfoPipe();

        char pipeName[BUFFER_SIZE];
        char comando[BUFFER_SIZE];
        char fileName[BUFFER_SIZE];

        int pid_pipe_fork,sair,m,pidProcesso;

        while(1){
            
            n = read(pid_pipe,infoPipe,sizeof(*infoPipe));  
            while(numComand>=MAX){
                pause();
            }

            if(n!=0){
                strcpy(pipeName,infoPipe->pipeName);
                strcpy(comando,infoPipe->comando);
                strcpy(fileName,infoPipe->fileName);
                pidProcesso=infoPipe->pidProcesso;
                numComand++;
                if(!fork()){

                    if(!strcmp(comando,"backup")) pid_pipe_fork = open(pipeName,O_RDONLY);
                    else if(!strcmp(comando,"restore")) pid_pipe_fork = open(pipeName,O_WRONLY);

                    sair=1;
                    while(sair){

                        m = read(pid_pipe_fork,info,sizeof(*info));   

                        if(m!=0){

                            sair=checkComandAndFile(info,sair,&verifica,&caso_backup,&caso_restore,destino_data,destino_metadata,fileName,comando,pidProcesso,pid_pipe_fork);
                        }
                        verifica=1;
                    } 
                kill(getppid(),SIGINT);
                _exit(1);
                }
                
            }
            else{
                close(pid_pipe);
                pid_pipe = open(destino_pipe,O_RDONLY);
            }

        }
    
    }
    return 0;
}

int checkComandAndFile(INFO info,int sair,int* verifica,int* caso_backup,int* caso_restore,char* destino_data,char* destino_metadata,char* fileName,char* comando,int pidProcesso,int pid_pipe_fork){
    
    char existFile[BUFFER_SIZE];
    char destino_file[BUFFER_SIZE];
    int idFile;

    if(!strcmp(info->comando,"backup"))
        *caso_backup = verificaFicheiros(info,destino_metadata,destino_data);

    if(*verifica && !strcmp(comando,"restore")){
                      
        strcpy(existFile,destino_metadata);
        strcat(existFile,"/");
        strcat(existFile,fileName);
                        
        if(access(existFile,F_OK)==0)  *caso_restore=1;
        else *caso_restore=0;
        *verifica=0; 
    }

    if(info->fim  && (*caso_backup==2 || *caso_backup==3 || *caso_backup==4) && !strcmp(info->comando,"backup")){
        if(*caso_backup!=4){
            sprintf(destino_file,"%s/%s", destino_data,info->NomeFicheiro);
            idFile = open(destino_file,O_WRONLY | O_CREAT | O_APPEND, 0666);
            write(idFile,info->Ficheiro,info->tamanho);
            close(idFile);
        }   
    }
    else {
        sair=0;
        chooseComand(info,comando,fileName,pidProcesso,pid_pipe_fork,*caso_restore,*caso_backup);
    }
    return sair;
}


void chooseComand(INFO info,char* comando,char*fileName,int pidProcesso,int pid_pipe_fork, int caso_restore, int caso_backup){
        int i;
        i=0;
        if(strcmp(info->comando,"backup")==0){
            i=backup(info->NomeFicheiro,info->Codigo,caso_backup);
            usleep(100);
            if(i)
                kill(pidProcesso,SIGALRM);
            else kill(pidProcesso,SIGUSR1);
        }
        
        if(strcmp(comando,"restore")==0){

            if(caso_restore){
                kill(pidProcesso,SIGCONT);
                restore(fileName,pid_pipe_fork);
            }
            else kill(pidProcesso,SIGFPE);
        }

        if(strcmp(comando,"delete")==0){
            i=delete(fileName);
            if(i)    
                kill(pidProcesso,SIGUSR2);
            else kill(pidProcesso,SIGHUP);
        }

        if(strcmp(comando,"gc")==0){
            i=gc();
            if(i)
                kill(pidProcesso,SIGUSR2);
            else kill(pidProcesso,SIGHUP);
        }
}

/**
  * A função backup, uma das principais do programa, realiza a cópia de segurança de um ficheiro. Para isso, recorre aos comandos sha1sum,gzip,cp,mv e ln. 
  *
  */

int backup(char* file,char* codigo,int caso){

    char data[BUFFER_SIZE],metadata[BUFFER_SIZE];
    char destino_ficheiro_data[BUFFER_SIZE],destino_codigo[BUFFER_SIZE],destino_ficheiro_metadata[BUFFER_SIZE];

    strcpy(data,getenv("HOME"));
    strcat(data,"/.Backup/data/");
    strcpy(metadata,getenv("HOME"));
    strcat(metadata,"/.Backup/metadata/");


    if(caso==1){
        strcpy(destino_codigo,data);
        strcat(destino_codigo,codigo);

        strcpy(destino_ficheiro_metadata,metadata);
        strcat(destino_ficheiro_metadata,file);

        if(!fork()){
            execlp("ln","ln","-s",destino_codigo,destino_ficheiro_metadata,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
        return 1;
    }
    else if(caso==2 || caso==3){
        strcpy(destino_ficheiro_data,data);
        strcat(destino_ficheiro_data,file);

        if(!fork()){
           execlp("gzip","gzip",destino_ficheiro_data,NULL);
           perror("error");
           _exit(1);
        }
        wait(NULL);

        strcat(destino_ficheiro_data,".gz");
        strcpy(destino_codigo,data);
        strcat(destino_codigo,codigo);

        rename(destino_ficheiro_data,destino_codigo);

        strcpy(destino_ficheiro_metadata,metadata);
        strcat(destino_ficheiro_metadata,file);

        if(caso==2) unlink(destino_ficheiro_metadata);

        if(!fork()){
            execlp("ln","ln","-s",destino_codigo,destino_ficheiro_metadata,NULL);
            perror("error");
            _exit(1);
        }
        wait(NULL);
        return 1;
        }
    return 0;
}

/**
  * A função restore, uma das principais do programa, recupera ficheiros que tenham cópia de segurança.
  *
  */

void restore (char* file,int pid_pipe){

    int  idFile, tamanho;
    char ficheiro_metadata[BUFFER_SIZE];
    char ficheiro_zip[BUFFER_SIZE];
    char ficheiro_zip_2[BUFFER_SIZE];
    char ficheiro_clone[BUFFER_SIZE];
    char codigo[BUFFER_SIZE];
    char buffer[BLOCK_FILE_SIZE];

    strcpy(ficheiro_metadata,getenv("HOME"));
    strcat(ficheiro_metadata,"/.Backup/metadata/");
    strcpy(ficheiro_clone,getenv("HOME"));
    strcat(ficheiro_clone,"/.Backup/data/");
    
    sprintf(codigo,"%d",getpid());
    strcat(ficheiro_clone,codigo);
    strcat(ficheiro_metadata,file);
    
    tamanho = readlink(ficheiro_metadata,ficheiro_zip, BUFFER_SIZE);
    
    ficheiro_zip[tamanho]=0;
    
    strcpy(ficheiro_zip_2,ficheiro_clone);
    
    my_copy(ficheiro_zip,ficheiro_clone);

    strcat(ficheiro_clone,".gz");
    
    rename(ficheiro_zip_2,ficheiro_clone);

    if(!fork()){
            execlp("gunzip","gunzip",ficheiro_clone,NULL);
            perror("error");
            _exit(1);
    }
    wait(NULL);

    INFO info = initInfo();
    idFile = open(ficheiro_zip_2, O_RDONLY);

    while((tamanho=read(idFile,buffer,BLOCK_FILE_SIZE))>0){
        memcpy(info->Ficheiro,buffer,tamanho);   
        info->tamanho=tamanho;
        info->pidProcesso=getpid();
        strcpy(info->NomeFicheiro,file);
        strcpy(info->comando,"restore");
        info->fim=1;
        write(pid_pipe, info,sizeof(*info));
    }
    info->fim=0;
    info->tamanho=0;
    info->pidProcesso=getpid();
    strcpy(info->NomeFicheiro,file);
    strcpy(info->comando,"restore");
    write(pid_pipe,info,sizeof(*info));

    unlink(ficheiro_zip_2);

    close(pid_pipe);
}

int delete (char* file){
    char destino[BUFFER_SIZE];
    strcpy(destino,getenv("HOME"));
    strcat(destino,"/.Backup/metadata/");

    if(exist(file,destino)){
        strcat(destino,file);
        unlink(destino);
        return 1;
    }
    else{ 
        return 0;
    }
}

int gc(){
    char destino_data[BUFFER_SIZE];
    char destino_metadata[BUFFER_SIZE];
    char ficheiro[BUFFER_SIZE];
    char* ficheiro_data;
    char* ficheiro_metadata;

    strcpy(destino_data,getenv("HOME"));
    strcat(destino_data,"/.Backup/data/");

    strcpy(destino_metadata,getenv("HOME"));
    strcat(destino_metadata,"/.Backup/metadata/");
   
    char** lista_data;
    char** lista_metadata;
    char** lista_path;
    int i,j,tamanho,n,notDelete,totalDelete=0;

    lista_data=listaPasta(destino_data);
    lista_metadata=listaPasta(destino_metadata);

    for(i=0;lista_data[i+1]!=NULL;i++){
        ficheiro_data=malloc(sizeof(char)*BUFFER_SIZE);
        strcpy(ficheiro_data,getenv("HOME"));
        strcat(ficheiro_data,"/.Backup/data/");
        strcat(ficheiro_data,lista_data[i]);
        
        for(j=0;lista_metadata[j+1]!=NULL;j++){
            ficheiro_metadata=malloc(sizeof(char)*BUFFER_SIZE);
            strcpy(ficheiro_metadata,destino_metadata);
            strcat(ficheiro_metadata,lista_metadata[j]);
            
            tamanho=readlink(ficheiro_metadata,ficheiro,BUFFER_SIZE);
            ficheiro[tamanho]=0;
            
            lista_path=readln(ficheiro,&n,"/");
            
            if(!strcmp(lista_path[n],lista_data[i]))
                notDelete++;

            free(ficheiro_metadata);
            ficheiro_metadata=NULL;
            if(notDelete>0) break;
        }

        if(notDelete==0){
            unlink(ficheiro_data);
            totalDelete++;
        }
        notDelete=0;
        free(ficheiro_data);
        ficheiro_data=NULL;
    }
    if(totalDelete>0) return 1;
    else return 0;
}


int verificaFicheiros(INFO info,char* destino_metadata,char* destino_data){
    int caso_backup;
    char existCodigo[BUFFER_SIZE];
    char existFile[BUFFER_SIZE];

    strcpy(existCodigo,destino_data);
    strcat(existCodigo,"/");
    strcat(existCodigo,info->Codigo);
                  
    strcpy(existFile,destino_metadata);
    strcat(existFile,"/");
    strcat(existFile,info->NomeFicheiro);

    if(access(existCodigo, F_OK)==0 && access(existFile,F_OK)==-1) caso_backup=1;
                
    else if(access(existFile, F_OK)==0 && access(existCodigo,F_OK)==-1 ) caso_backup=2;

    else if(access(existCodigo,F_OK)==-1 && access(existFile,F_OK)==-1 ) caso_backup=3;

    else caso_backup=4; 
    
    return caso_backup;
}


/**
  * A função fim descrementa a variavél global, numComand. Esta função é utilizada, para alterar um sinal, que indica quando um processo tenha acado, 
  * o que decrementa desta foram o numComand.
  *
  */

void fim (){
    numComand--;
}

void my_copy(char* orig, char *dest){
    int n;
    int file_orig;
    int file_dest;
    char buffer[1024];

    file_orig = open(orig,O_RDONLY);
    file_dest = open(dest,O_WRONLY | O_CREAT | O_APPEND, 0600);
    while((n=read(file_orig,buffer,1024)))
        write(file_dest,buffer,n);
}

/**
  * A função criaPastas, é a função responsável por criar as pastas onde as cópias de seguranças ( backup's ) vão ficar armazenadas. 
  *
  */

void criaPastas (){

    char backup_data[BUFFER_SIZE];
    char metadata[BUFFER_SIZE]; 
  
    strcpy(backup_data,getenv("HOME"));
    strcat(backup_data,"/.Backup");
    mkdir(backup_data,0777);

    strcpy(metadata,backup_data);
    strcat(backup_data,"/data");
    mkdir(backup_data,0777);
    
    strcat(metadata,"/metadata");
    mkdir(metadata,0777);
}

/**
  * A função readln é uma função auxiliar, que dado uma String, separa-a em várias Strings, criando uma lista com essas Strings. A String original, é 
  * separada na porção de String equivalente a front.
  *
  */

char** readln(char *buf,int *n,char* front){
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
    *n=i-1;
    return buff;
}

/**
  * A função exist verifica se um determinado ficheiro existe numa certa directoria. A função recorre ao auxilio do comando ls.
  *
  */

int exist (char* file, char* dest){
    char** lista_ficheiros;
    int i;
    
    lista_ficheiros=listaPasta(dest);

    for(i=0;lista_ficheiros[i]!=NULL;i++){
        if(!strcmp(lista_ficheiros[i],file)) return 1;
    }
    return 0;
}


char** listaPasta(char* dest){
    int pfd[2],n;
    char codigo[BLOCK_FILE_SIZE];
    char** lista_ficheiros;

    pipe(pfd);
    if(!fork()){
        close(pfd[0]);
        dup2(pfd[1],1);
        execlp("ls","ls",dest,NULL);
        perror("erro");
        _exit(0);
    }
    wait(NULL);

    close(pfd[1]);
    dup2(pfd[0],0);
    close(pfd[0]);
    
    read(0,codigo,BLOCK_FILE_SIZE);
    codigo[strlen(codigo)]=0;
    
    lista_ficheiros=readln(codigo,&n,"\n");

    return lista_ficheiros;
}

