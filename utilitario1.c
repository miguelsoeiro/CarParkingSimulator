//Authors: André Cerveira nº21402048 - João Batista nº 21404080 - Miguel Soeiro nº21502788
 
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>
#include <semaphore.h>
#define MAXNUM_THREADS 4
#define MAX_MSGBYTES 24

char fifo[8];
sem_t *full;

FILE *file;

char ficheiro[60];
int t,a=0;
int *taskids[MAXNUM_THREADS];
pthread_t threads[MAXNUM_THREADS];

char *getuuid(){    
 static char str[36];
 uuid_t uuid;
 uuid_clear(uuid);
 uuid_generate_random(uuid);
 uuid_unparse(uuid,str);
 return str;
}

void *writeFIFO2(void *threadid){

	pthread_t id=pthread_self();
    int value;
    int *id_ptr, taskid, wrfd;
    id_ptr = (int *) threadid;
    taskid = *id_ptr;
    char item[38], texto[60],teste[60];
    file=fopen(ficheiro,"r");
    if(MAXNUM_THREADS%4==0){
	   	if(pthread_equal(id,threads[3])){
	   		while(fgets(texto,60,file)!=NULL){
	   			printf("%s",texto);
	   			snprintf(fifo, sizeof(fifo), "fifo%d", taskid);
	          	wrfd = open(fifo, O_WRONLY);
	        	write(wrfd,texto,sizeof(texto));      
	        	sem_getvalue(full, &value);
	   			fgets(texto,60,file);
	   			fgets(texto,60,file);
	   			fgets(texto,60,file);
	   		}
	   	}else if(pthread_equal(id,threads[2])){
	   		while(fgets(texto,60,file)!=NULL){
	   			fgets(texto,60,file);
	   			printf("%s",texto);
	   			snprintf(fifo, sizeof(fifo), "fifo%d", taskid);
	          	wrfd = open(fifo, O_WRONLY);
	        	write(wrfd,texto,sizeof(texto));      
	        	sem_getvalue(full, &value);
	   			fgets(texto,60,file);
	   			fgets(texto,60,file);
	   		}
	   	}else if(pthread_equal(id,threads[1])){
	   		while(fgets(texto,60,file)!=NULL){
	   			fgets(texto,60,file);
	   			fgets(texto,60,file);
	   			printf("%s",texto);
	   			snprintf(fifo, sizeof(fifo), "fifo%d", taskid);
	          	wrfd = open(fifo, O_WRONLY);
	        	write(wrfd,texto,sizeof(texto));      
	        	sem_getvalue(full, &value);
	   			fgets(texto,60,file);
	   		}
	   	}else if(pthread_equal(id,threads[0])){
	   		while(fgets(texto,60,file)!=NULL){
	   			fgets(texto,60,file);
	   			fgets(texto,60,file);
	   			fgets(texto,60,file);
	   			printf("%s",texto);
	   			snprintf(fifo, sizeof(fifo), "fifo%d", taskid);
	          	wrfd = open(fifo, O_WRONLY);
	        	write(wrfd,texto,sizeof(texto));      
	        	sem_getvalue(full, &value);
	   		}
	   	}
	}else{
	    while(fgets(texto,60,file)!=NULL){
	        sem_getvalue(full, &value);
	        snprintf(fifo, sizeof(fifo), "fifo%d", taskid);
	        wrfd = open(fifo, O_WRONLY);
	        printf("%s",texto);
	        write(wrfd,texto,sizeof(texto));      
	        sem_getvalue(full, &value);
	    }
	}
    pthread_exit(NULL);
}

void *writeFIFO1(void *threadid){

    int value;
    int *id_ptr, taskid, wrfd;
    id_ptr = (int *) threadid;
    taskid = *id_ptr;
    char item[38];
    snprintf(fifo, sizeof(fifo), "fifo%d", taskid);//colocar em fifo uma string que indica o nome do ficheiro a ser criado
    wrfd = open(fifo, O_WRONLY);
    snprintf(item, sizeof(item),"%s\n", getuuid());
    printf("%s", item);
    write(wrfd,item,sizeof(item));//escreve para o fifo     
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){

    int rc,nthreads, ret_val, i;
    nthreads=MAXNUM_THREADS;
    full = sem_open("/sem1", O_CREAT, 00700, 0);
    for(;;){
	    int ler;
	    printf("Introduzir um uuid aleatorio ou ler de ficheiro?(1 aleatorio, 2 ler de ficheiro)\n");
	    scanf("%d",&ler);
	    if(ler==1){//Introduzir um uuid unico aleatorio
    		t=1;
	        taskids[t] = (int *) malloc(sizeof(int));
	        *taskids[t] = t;
	        printf("Create fifo %d\n", t);
	        snprintf(fifo, sizeof(fifo), "fifo%d", t);    
	        ret_val = mkfifo(fifo, 0666);
	        if ((ret_val == -1) && (errno != EEXIST)) {
	            perror("Error creating named pipe\n");
	            exit (-1);
	        }
	        rc = pthread_create(&threads[t], NULL, writeFIFO1, (void *) taskids[t]);//criar uma thread
	        if (rc) {
	            printf("ERROR; pthread_create() error code : %d\n", rc);
	            exit(-1);
	        }
	    	sleep(1);

	    }else if(ler==2){//Introduzir uuid a partir de um fucheiro
	    	printf("Introduza o nome do ficheiro(com .txt)\n");
    		scanf("%s",ficheiro);
    		file=fopen(ficheiro,"r");
			if(file==NULL){
				perror("Ficheiro nao encontrado\n");
				return(-1);
			}
	    	for(t=0;t<nthreads;t++) {
		        taskids[t] = (int *) malloc(sizeof(int));
		        *taskids[t] = t;
		        printf("Create thread %d\n", t);
		        snprintf(fifo, sizeof(fifo), "fifo%d", t);    
		        ret_val = mkfifo(fifo, 0666);
		        if ((ret_val == -1) && (errno != EEXIST)) {
		            perror("Error creating named pipe\n");
		            exit (-1);
		        }
		        rc = pthread_create(&threads[t], NULL, writeFIFO2, (void *) taskids[t]);//criar threads
		        if (rc) {
		            printf("ERROR; pthread_create() error code : %d\n", rc);
		            exit(-1);
	        	}
	    	}
	    sleep(3);
	    sem_post(&full);
    	}else{
    		printf("ERRO! O número que introduziu é diferente de 1 e 2!\n");
    		return (0);
    	}
    }
    pthread_exit(NULL);
}
