#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {


    pid_t ret = fork();

    if (ret==-1)  { 
        printf("Erro na execucao do fork, processo filho nao foi criado.\n"); 
        exit(0); 
    } 

    // se código chegou aqui é porque o fork funcionou. Há 2 processos executando a partir daqui.

    if (ret==0) {  
    printf("Processo filho: %d\n",(int)getpid()); 

    } else { // esse é o processo pai. Valor de retorno do fork é o pid do filho criado
    printf("Processo %d, pai de %d\n", (int)getpid(), (int)ret); 
    }

    // Os 2 processos prosseguem e vão executar a linha seguinte!
    printf("%d terminando...\n", (int)getpid());

}
