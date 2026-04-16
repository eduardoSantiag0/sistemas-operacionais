#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


// 1 ou mais threads produtoras, responsáveis por gerar itens.
// 1 ou mais threads consumidoras, responsáveis por consumir itens.
// Um buffer compartilhado de tamanho limitado (fila circular, por exemplo).

// Regras
// O produtor:
    // gera um item (pode ser um número inteiro aleatório ou sequencial);
    // insere o item no buffer;
    // se o buffer estiver cheio, deve esperar.
// O consumidor:
    // remove um item do buffer;
    // processa/imprime o item consumido;
    // se o buffer estiver vazio, deve esperar.
// O acesso ao buffer:
    // deve ser sincronizado corretamente;
    // não pode haver condição de corrida;
    // não pode haver corrupção de dados.


#define BUFFER_SIZE 10
#define MAX_THREADS 5

// mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int in = 0;        // posição de escrita
int out = 0;       // posição de leitura
int item_counter = 0;



void *producer(void *arg)
{
    int *buffer = (int *)arg;
    int item = 1;

    printf("Producer iniciado\n");

    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (item_counter < BUFFER_SIZE)
        {
            buffer[in] = item;

            printf("Produziu %d na posição %d\n", item, in);

            in = (in + 1) % BUFFER_SIZE;
            item_counter++;
            item++;
        }
        else
        {
            printf("Buffer cheio. Producer esperando...\n");
        }

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }

    return NULL;
}

void *consumer(void *arg)
{
    int *buffer = (int *)arg;

    printf("Consumer iniciado\n");

    while (1)
    {
        pthread_mutex_lock(&mutex);

        if (item_counter > 0)
        {
            int item = buffer[out];

            printf("Consumiu %d da posição %d\n", item, out);

            buffer[out] = 0;

            out = (out + 1) % BUFFER_SIZE;
            item_counter--;
        }
        else
        {
            printf("Buffer vazio. Consumer esperando...\n");
        }

        pthread_mutex_unlock(&mutex);

        sleep(3);
    }

    return NULL;
}

int main()
{
    int *buffer = calloc(BUFFER_SIZE, sizeof(int));

    if (buffer == NULL)
    {
        perror("Erro ao alocar memória");
        exit(1);
    }

    pthread_t producerThread;
    pthread_t consumerThread;

    pthread_create(&producerThread, NULL, producer, buffer);
    pthread_create(&consumerThread, NULL, consumer, buffer);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    free(buffer);

    return 0;
}

