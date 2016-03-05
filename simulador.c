//Authors: André Cerveira nº21402048 - João Batista nº 21404080 - Miguel Soeiro nº21502788

#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#define MAXNUM_THREADS 4
#define MAX_MSGBYTES 2048
#define BUFSIZE 1000
#define SHMSZ     27


struct matricula{
	char *matricula;
	struct timespec start, end;
};

pthread_t matriculas[80], limpar;
int *idtarefas[80], *idtarefa,nump=3;

char fifo[80];
char teste[600][600],b[36],teste2[36][1000000], buffer[80][BUFSIZE];
int as=0,d=0,j=0,q=0,is=0,numl=0,numbuffer=0,w=0;

struct matricula estacionamento[80];

char c;
int shmid;
key_t key;
struct matricula *shm, *mempart;

sem_t sem,sem2;  




void *limparBuffer(void *threadid){
	FILE *saida;
	sem_wait(&sem);
		int i=0;
		int wrfd;
		char item[38];
		for(i=0;i<((nump)/37);i++){
			snprintf(fifo, sizeof(fifo), "fifo5");
    		wrfd = open(fifo, O_WRONLY);
    		saida=fopen(fifo,"r");
    		w++;
    		sleep(1);
    		printf("posicao %d do buffer saiu\n",i);
    		buffer[i][0]=='\0';
    		write(wrfd,item,sizeof(item));
		}
	}

void *estacionar(void *threadid){
	sem_wait(&sem2);
	key = 5678;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    mempart = shm;
	srand(time(NULL));
	pthread_t lugar=pthread_self();
	int i=0,a=0,j,l,k,rd;
	for (k = 0; k < 80; k++){
		if(pthread_equal(lugar,matriculas[k])){
			if(estacionamento[k].matricula=='\0'){
				int h=0;
					estacionamento[k].matricula=teste2[k];
					*mempart++ = estacionamento[k];
				clock_gettime(CLOCK_MONOTONIC,&estacionamento[k].start);
				printf(" posicao=%d, matricula=%s. ",k,estacionamento[k].matricula);
				int n=0;
				n=(rand() %5)+1;
				printf("tempo rnd=%d.\n",n);
				sleep(n);
				n=0;
				clock_gettime(CLOCK_MONOTONIC,&estacionamento[k].end);
				printf(" posicao=%d, matricula=%s saiu. \n",k,estacionamento[k].matricula);
				sem_post(&sem);
				int tambuffer=0;
				numbuffer=((nump)/37)-k;
				for(tambuffer=0;tambuffer<BUFSIZE;tambuffer++){
					if(buffer[tambuffer]=='\0'){
						strcpy(buffer[tambuffer],estacionamento[k].matricula);
						unsigned short int idx;
						for(idx=0;idx<BUFSIZE;idx++){
							estacionamento[k].matricula[idx]=(int)(void *)NULL;
						}
						estacionamento[k].start.tv_sec=0;
						estacionamento[k].end.tv_nsec=0;
						break;
					}
				}
				rd = pthread_create(&limpar, NULL, limparBuffer, (void *) idtarefa);
			}
		}
	}
}

void *readFIFO(void *threadid)
{
	pthread_t matriculas[80];
	int *idtarefas[80],rc;
	FILE *stream;
	int *id_ptr, taskid, rdfd;
	char c;
	id_ptr = (int *) threadid;
	taskid = *id_ptr;
	snprintf(fifo, sizeof(fifo), "fifo%d", taskid);
	rdfd = open(fifo, O_RDONLY);
	stream = fdopen (rdfd, "r");
	while ((c = fgetc (stream)) != EOF){
		if(isalpha(c)||c=='-'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9'||c=='0'||c=='\n'){
			b[as]=c;
			as++;
			numl++;
			if(as=36){
				for(is=0;is<36;is++){
					teste2[is][d]=b[is];
					if(is==35){
						nump++;
					}
				}

				for(q=0;q<36;q++){
					printf("%c",teste2[q][d]);
				}
				as=0;
				d++;
				sem_post(&sem2);
			}
		}
	}
	
	fclose (stream);   	
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	sem_init(&sem,0,0);
	sem_init(&sem2,0,0);
	if (argc != 2 ){
		printf("Usage: %s <number_of_threads>\n",argv[0]);
		exit(-1);
	}
	sem_t *full;
	full = sem_open("/sem1", O_CREAT, 00700, 0);
	int nthreads, rc, t, ret_val, i,e;
	int rd;
	nthreads=atoi(argv[1]);
	pthread_t threads[nthreads];
	int *taskids[nthreads];
	sem_wait(&full);
	for(t=0;t<nthreads;t++) {
		taskids[t] = (int *) malloc(sizeof(int));
		*taskids[t] = t;
		printf("Create thread %d\n", t);
		rc = pthread_create(&threads[t], NULL, readFIFO, (void *) taskids[t]);
		if (rc) {
			printf("ERROR; pthread_create() error code : %d\n", rc);
			exit(-1);
		}
		if (t==nthreads-1){
			break;
		}
	}
	sleep(5);
	int k=1;
	for(;;){
		while(k<=(nump)/37){
			rd = pthread_create(&matriculas[(nump)/37], NULL, estacionar, (void *) idtarefas[(nump)/37]);
			k++;
		}
	}
	pthread_exit(NULL);
	return 0;
	
}
