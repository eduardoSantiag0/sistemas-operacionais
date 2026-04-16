#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//     Multiple readers can access data together if no writer is writing.
//     Writers have exclusive access no other reader or writer can enter during writing.

// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define TAM 10
char buffer[TAM];
int count = 0;

// Controle de leitores
int readCount = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t writeLock;
sem_t catraca;

void* readers(void *arg) 
{
    while(1) 
    {
        sem_wait(&catraca); // Tenta passar pela catraca
        sem_post(&catraca); // Libera a catraca

        pthread_mutex_lock(&mutex); // Protege o readCount
        readCount++; // Incrementa o cont
        if (readCount == 1) // Primeiro fecha a porta para escritores
            sem_wait(&writeLock); 
        pthread_mutex_unlock(&mutex);  

        // Região Crítica
        printf("Leitor lendo: ");
        for (int i = 0; i < count; i++)
            printf("%c ", buffer[i]);
        printf("\n");
        sleep(2);

        pthread_mutex_lock(&mutex); // Protege o eradCount
        readCount--;
        if (readCount == 0)
            sem_post(&writeLock); // Libera o lock para o escritor
        pthread_mutex_unlock(&mutex);
    }
}

void* writers(void *arg)
{
    while(1) 
    {
        // Fecha a catraca: impede que NOVOS leitores entrem no sistema
        // (os leitores atuais continuam até terminar)
        sem_wait(&catraca); 

        sem_wait(&writeLock); // Verifica se ele pode editar
        // Região crítica
        if (count < TAM) 
            {
                char c = 'A' + (rand() % 26);
                buffer[count++] = c;
                printf("Escritor escreveu: %c\n", c);
            }

        sem_post(&writeLock); // Devolve o lock

        // Reabre a catraca: permite que leitores (e outros escritores) voltem a entrar
        sem_post(&catraca); // Devolve a catraca

        sleep(1);
    }

    return NULL;

}

int main() {


    sem_init(&writeLock, 0, 1);
    sem_init(&catraca, 0, 1);

    pthread_t reader1, reader2, reader3;
    pthread_t writer1, writer2;


    int *lines = calloc(TAM, sizeof(char));

    pthread_create(&reader1, NULL, readers, lines);
    pthread_create(&reader2, NULL, readers, lines);
    pthread_create(&reader3, NULL, readers, lines);
    pthread_create(&writer1, NULL, writers, lines);
    pthread_create(&writer2, NULL, writers, lines);


    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);
    pthread_join(reader3, NULL);
    pthread_join(writer1, NULL);
    pthread_join(writer2, NULL);

    free(lines);

    sem_destroy(&writeLock);


    return 0;
}