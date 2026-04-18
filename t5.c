/*
 ** Programa :
 **   Uso de threads para soma das linhas de 2 matrizes
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#define LEN 128

#define NLIN 1000
#define NCOL 1000
#define NTHR 8

int *_A;
int *_B;
int *_C;

int _numeroLinhas = 0;
int _numeroColuna = 0;
int _numeroThreads = 0;

int _verbose = 0;

void *
soma(void *arg)
{
  // determina o número lógico desta thread
	long int ind = (long int)arg;
	int i,j;
	int nl, li, lf;

	// Falta tratar divisão não inteira...
	nl = _numeroLinhas/_numeroThreads;
	li = ind*nl;
	lf = li+nl;

  if (_verbose)
	  printf("Thread %ld manipula linhas %d a %d\n",ind,li,lf-1);

	for(i=li; i < lf; i++)

		for(j=0;j<_numeroColuna;j++)

			_C[i*_numeroColuna+j] = _A[i*_numeroColuna+j] + _B[i*_numeroColuna+j];

	pthread_exit(NULL);
}

int
main (int argc, char *argv[])
{
	pthread_t threads[NLIN];
	long int t;
  int status;
	char err_msg[LEN];
	int i,j,ind;

	ind=1;
	while (ind < argc) {
		if( !strcmp(argv[ind],"-h") || !strcmp(argv[ind],"/?")) {
			printf("Uso: %s [-nc num_col] [-nl num_lin] [-nt num_thr] ",argv[0]);
			exit(0);
		}
    if(!strcmp(argv[ind],"-v"))
		  _verbose=1;

		if(!strcmp(argv[ind],"-nc")) {
			if(argc>ind)
				_numeroColuna=atoi(argv[++ind]);
			else {
				printf("Erro nos parâmetros...\n");
				exit(0);
			}
		}
		if(!strcmp(argv[ind],"-nl")) {
			if(argc>ind)
				_numeroLinhas=atoi(argv[++ind]);
			else {
				printf("Erro nos parâmetros...\n");
				exit(0);
			}
		}
		if(!strcmp(argv[ind],"-nt")) {
			if(argc>ind)
				_numeroThreads=atoi(argv[++ind]);
			else {
				printf("Erro nos parâmetros...\n");
				exit(0);
			}
		}

		ind++;
	}

	if(_numeroLinhas==0) _numeroLinhas = NLIN;
	if(_numeroColuna==0) _numeroColuna = NCOL;
	if(_numeroThreads==0) _numeroThreads = NTHR;

	// alocar as matrizes
	_A=(int *)malloc(_numeroLinhas * _numeroColuna * sizeof(int));
	_B=(int *)malloc(_numeroLinhas * _numeroColuna * sizeof(int));
	_C=(int *)malloc(_numeroLinhas * _numeroColuna * sizeof(int));

  srand(getpid());

  // geração das matrizes com valores inteiros aleatórios (0 a 9). Senão, 0!
	for(i=0;i<_numeroLinhas;i++) {
		for(j=0;j<_numeroColuna;j++) {
			_A[i*_numeroColuna+j]=rand()%10;
			_B[i*_numeroColuna+j]=rand()%10;
		}
	}

	if(_verbose) {
		printf("\n");
		for(i=0;i<_numeroLinhas;i++) {
			printf("%d: ",i);
			for(j=0;j<_numeroColuna;j++)
				printf("%d ",_A[i*_numeroColuna+j]);
			printf("   ");
			for(j=0;j<_numeroColuna;j++)
				printf("%d ",_B[i*_numeroColuna+j]);
			printf("\n");
		}
    printf("\n");
	}

  // cria threads, passando o índice lógico como parâmetro para cada uma delas
	for (t=0; t<_numeroThreads; t++) {
		status = pthread_create(&threads[t], NULL, soma, (void *)t);
		if (status) {
			strerror_r(status,err_msg,LEN);
			printf("Falha da criacao da thread %ld (%d): %s\n",t,status,err_msg);
			exit(EXIT_FAILURE);
		}
	}

	// espera threads retornarem
	for (t=0; t<_numeroThreads; t++) {

		status = pthread_join(threads[t], NULL);
		if (status) {
			strerror_r(status,err_msg,LEN);
			printf("Erro em pthread_join: %s\n",err_msg);
			exit(EXIT_FAILURE);
		}
	}

	// impressão da matriz resultante
	if(_verbose) {
		printf("\n");
		for(i=0;i<_numeroLinhas;i++) {
			printf("%d: ",i);
			for(j=0;j<_numeroColuna;j++)
				printf("%2d ",_C[i*_numeroLinhas+j]);
			printf("\n");
		}
		printf("\n");
	}

	// desalocar as matrizes
	free(_A);
	free(_B);
	free(_C);

	return(0);
}