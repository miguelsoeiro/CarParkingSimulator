//Authors: André Cerveira nº21402048 - João Batista nº 21404080 - Miguel Soeiro nº21502788

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <uuid/uuid.h>

#define SHMSZ     27

struct matricula{
    char *matricula;
    struct timespec start, end;
};

int main()
{    int shmid;
    key_t key;
    struct matricula *shm, *s;
    int *i;
    key = 5678;

    //ver onde o segmento se encontra em memória
    
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        return 1;
    }

    //anexa o segmento para a zona de memória
     
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        return 1;
    }

    //Ler o que o programa servidor mete na memória partilhada
     

    int h,l,j; 
    size_t k;

    struct matricula estacionamento[80];
    for(j=0;j<80;j++){
        for(k=0;k<sizeof (estacionamento[j].matricula);k++){
            printf("%02x",estacionamento[j].matricula[k]);
        }
        printf("\n");
    } 
    return 0;
}