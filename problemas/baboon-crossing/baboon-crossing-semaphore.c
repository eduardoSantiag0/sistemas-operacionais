#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_BABOONS 10
#define SLEEP_TIME 3
#define MAX_ON_ROPE 5

#define MAX_DELAY_US 500000 

#define NUM_LEFT_BABOONS 15
#define NUM_RIGHT_BABOONS 5

// crossingLock -> exclusão entre direções
// catraca -> impede entrada de novos babuínos quando lado oposto chega
// multiplex -> limite máximo na corda
sem_t crossingLock, catraca, multiplex;
pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t lmutex = PTHREAD_MUTEX_INITIALIZER;
int leftCount = 0;

pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
int rightCount = 0;

char *drawRight;
char *drawLeft;

void *leftBaboon(void *arg)
{
        sem_wait(&catraca);
        sem_post(&catraca);

        pthread_mutex_lock(&lmutex);
        leftCount++;

        if (leftCount == 1)
            sem_wait(&crossingLock);

        pthread_mutex_unlock(&lmutex);

        sem_wait(&multiplex);

        pthread_mutex_lock(&printMutex);
        printf("LEFT -> RIGHT\n");
        printf("%s", drawLeft);
        pthread_mutex_unlock(&printMutex);

        sleep(SLEEP_TIME);

        sem_post(&multiplex);

        pthread_mutex_lock(&lmutex);
        leftCount--;

        if (leftCount == 0) {
            printf("\tEsquerda terminou\n");
            sem_post(&crossingLock);
        }

        pthread_mutex_unlock(&lmutex);

        pthread_mutex_lock(&printMutex);
        pthread_mutex_unlock(&printMutex);

    return NULL;
}

void *rightBaboon(void *arg)
{
        sem_wait(&catraca);
        sem_post(&catraca);

        pthread_mutex_lock(&rmutex);
        rightCount++;

        if (rightCount == 1)
            sem_wait(&crossingLock);

        pthread_mutex_unlock(&rmutex);


        sem_wait(&multiplex);

        pthread_mutex_lock(&printMutex);
        printf("RIGHT -> LEFT\n");
        printf("%s", drawRight);
        pthread_mutex_unlock(&printMutex);

        sleep(SLEEP_TIME);

        sem_post(&multiplex);

        pthread_mutex_lock(&rmutex);
        rightCount--;

        if (rightCount == 0) {
            printf("\tDireita terminou\n");
            sem_post(&crossingLock);
        }

        pthread_mutex_unlock(&rmutex);

        pthread_mutex_lock(&printMutex);
        pthread_mutex_unlock(&printMutex);

    return NULL;
}

int main()
{
    pthread_t left[NUM_LEFT_BABOONS];
    pthread_t right[NUM_RIGHT_BABOONS];

    int createdLeft = 0;
    int createdRight = 0;

    sem_init(&crossingLock, 0, 1);
    sem_init(&catraca, 0, 1);
    sem_init(&multiplex, 0, MAX_ON_ROPE);

    struct timespec start, end;
    double timeDiff;

    srand(time(NULL));

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Criação aleatória das threads para a chegada dos babuínos
    while (createdLeft < NUM_LEFT_BABOONS || createdRight < NUM_RIGHT_BABOONS)
    {
        int chooseLeft = rand() % 2;

        if (chooseLeft && createdLeft < NUM_LEFT_BABOONS)
        {
            pthread_create(&left[createdLeft], NULL, leftBaboon, NULL);
            createdLeft++;
        }
        else if (createdRight < NUM_RIGHT_BABOONS)
        {
            pthread_create(&right[createdRight], NULL, rightBaboon, NULL);
            createdRight++;
        }
        else if (createdLeft < NUM_LEFT_BABOONS)
        {
            pthread_create(&left[createdLeft], NULL, leftBaboon, NULL);
            createdLeft++;
        }

        usleep(rand() % MAX_DELAY_US);
    }

    for (int i = 0; i < NUM_LEFT_BABOONS; i++)
    {
        pthread_join(left[i], NULL);
    }

    for (int i = 0; i < NUM_RIGHT_BABOONS; i++)
    {
        pthread_join(right[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    timeDiff =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    sem_destroy(&crossingLock);
    sem_destroy(&multiplex);
    sem_destroy(&catraca);

    printf(
        "Tempo total: %.10f segundos para \n%d babuínos na esquerda e \n%d babuínos na direita\n",
        timeDiff,
        NUM_LEFT_BABOONS,
        NUM_RIGHT_BABOONS
    );

    printf(
        "Tempo médio por babuíno: %.10f segundos\n\n",
        timeDiff / (NUM_LEFT_BABOONS + NUM_RIGHT_BABOONS)
    );

    return 0;
}


char *drawRight =
"                                                                                \n"
"                                                                                \n"
"                                                                                \n"
"                                                          ▓▓▓▓▓▓▓▓              \n"
"                                                      ▓▓▓▓▓▓      ▓▓            \n"
"                                                    ▓▓▓▓          ▓▓            \n"
"                                                    ▓▓            ▓▓            \n"
"                                                    ▓▓            ▓▓            \n"
"                                                    ▓▓      ▓▓▓▓▓▓              \n"
"                                                    ▓▓▓▓                        \n"
"                                                      ██▓▓▓▓▓▓                  \n"
"                                                          ██▓▓▓▓                \n"
"                                                            ██▓▓▓▓              \n"
"                                                              ██▓▓▓▓            \n"
"                                                                ██▓▓▓▓          \n"
"                                                                  ██▓▓          \n"
"                                                                  ██▓▓          \n"
"                    ▓▓▓▓▓▓▓▓                                      ▓▓▓▓          \n"
"                  ▓▓▓▓▓▓▓▓  ▓▓▓▓                                ▓▓▓▓██          \n"
"                      ▓▓▓▓    ▓▓        ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓      ▓▓▓▓██            \n"
"                  ██  ▓▓▓▓    ██▓▓████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██            \n"
"              ▒▒▒▒▒▒  ▓▓▓▓██████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████              \n"
"              ▒▒▒▒▒▒▒▒▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓              \n"
"                ▒▒▒▒▓▓▓▓██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██              \n"
"                    ░░░░██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██              \n"
"                          ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██              \n"
"                          ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██░░              \n"
"                        ▒▒▓▓▓▓▓▓▓▓██████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██                \n"
"                        ▓▓▓▓▓▓▓▓████████░░░░████████▓▓██▓▓▓▓▓▓██                \n"
"                      ▒▒▓▓▓▓▓▓██████████  ▓▓████████░░██▓▓▓▓▓▓▓▓                \n"
"                      ▓▓▓▓▓▓██░░████████  ████████░░  ░░██▓▓▓▓▓▓▓▓              \n"
"                    ▓▓▓▓████    ████████  ████████          ██▓▓▓▓▓▓            \n"
"                    ▓▓▓▓██      ██████      ██████            ▓▓▓▓██            \n"
"                  ▓▓▓▓██        ██████        ████            ▓▓██              \n"
"              ▓▓▓▓▓▓████  ████████████  ██████████      ████▓▓▓▓██              \n"
"            ██▓▓██▓▓██    ██████████    ████████      ██████▓▓██                \n"
"            ░░    ░░░░    ░░░░  ░░░░    ░░░░░░        ░░░░░░░░░░                \n"
"                                                                                \n"
"                                                                                \n"
"                                                                                \n";


char *drawLeft =
"                                                                                \n"
"                                                                                \n"
"                                                                                \n"
"              ▓▓▓▓▓▓▓▓                                                          \n"
"            ▓▓      ▓▓▓▓▓▓                                                      \n"
"            ▓▓          ▓▓▓▓                                                    \n"
"            ▓▓            ▓▓                                                    \n"
"            ▓▓            ▓▓                                                    \n"
"              ▓▓▓▓▓▓      ▓▓                                                    \n"
"                        ▓▓▓▓▓▓                                                  \n"
"                  ▓▓▓▓▓▓██                                                      \n"
"                ▓▓▓▓██                                                          \n"
"              ▓▓▓▓██                                                            \n"
"            ▓▓▓▓██                                                              \n"
"          ▓▓▓▓██                                                                \n"
"          ▓▓██                                                                  \n"
"          ▓▓██                                                                  \n"
"          ▓▓▓▓                                      ▓▓▓▓▓▓▓▓                    \n"
"          ██▓▓▓▓                                ▓▓▓▓  ▓▓▓▓▓▓▓▓                  \n"
"            ██▓▓▓▓      ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓        ▓▓    ▓▓▓▓                      \n"
"            ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓████▓▓██    ▓▓▓▓  ██                  \n"
"              ██████▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████▓▓▓▓  ▒▒▒▒▒▒              \n"
"              ▓▓██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒              \n"
"              ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██▓▓▒▒▒▒              \n"
"              ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██░░░░                  \n"
"              ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                      \n"
"              ░░██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓                      \n"
"                ██▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓██████▓▓▓▓▓▓▓▓▓▓▓▓▒▒                      \n"
"                ██▓▓▓▓▓▓▓▓██▓▓████████░░░░████████▓▓▓▓▓▓                      \n"
"                ▓▓▓▓▓▓▓▓██░░████████▓▓  ██████████▓▓▓▓▓▓▒▒                    \n"
"              ▓▓▓▓▓▓▓▓██░░  ████████  ████████░░██▓▓▓▓▓▓▓▓                    \n"
"            ▓▓▓▓▓▓██          ████████  ████████    ████▓▓▓▓                  \n"
"            ██▓▓▓▓            ██████      ██████      ██▓▓▓▓                  \n"
"              ██▓▓            ████        ██████        ██▓▓▓▓                \n"
"              ██▓▓▓▓████      ██████████  ████████████  ████▓▓▓▓            \n"
"                ██▓▓██████      ████████    ██████████    ██▓▓██            \n"
"                ░░░░░░░░░░        ░░░░░░    ░░░░  ░░░░    ░░░░    ░░        \n"
"                                                                                \n"
"                                                                                \n"
"                                                                                \n";