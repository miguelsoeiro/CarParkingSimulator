//Authors: André Cerveira nº21402048 - João Batista nº 21404080 - Miguel Soeiro nº21502788

#include <stdio.h>
#include <string.h>
FILE *file2;
FILE *file3;
int main(int argc,char **argv){
	char *c1;
	char text[1000000];

	c1="fifo4";
	int tam=sizeof(c1);

	char d;
	int f=0;
	file2=fopen(c1,"r");
	if(file2==NULL){
		perror("Ficheiro nao encontrado\n");
		return(-1);
	}
	while ((d = fgetc (file2)) != EOF){
	 	putchar(d);
	 	text[f]=d;
	 	f++;
	}
	fclose(file2);

	
	file3=fopen("log.txt","w");
	if(file3==NULL){
		perror("Ficheiro nao encontrado\n");
		return(-1);
	}
	fwrite(text,f+1,1,file3);
	printf("criado ficheiro log.txt\n");
	fclose(file3);
}
