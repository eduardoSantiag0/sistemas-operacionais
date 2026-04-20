#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_BABOONS 10
#define SLEEP_TIME 3
#define MAX_ON_ROPE 5


sem_t crossingLock, turnstile, multiplex;

pthread_mutex_t lmutex = PTHREAD_MUTEX_INITIALIZER;
int leftCount = 0;

pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
int rightCount = 0;

char *drawRight;
char *drawLeft;

void *leftBaboon(void *arg)
{
    while (1)
    {
        sem_wait(&turnstile);

        pthread_mutex_lock(&lmutex);
        leftCount++;
        if (leftCount == 1)
            sem_wait(&crossingLock);
        pthread_mutex_unlock(&lmutex);

        sem_post(&turnstile);

        sem_wait(&multiplex);

        printf("LEFT -> RIGHT\n");
        printf("%s", drawLeft);
        sleep(3);

        sem_post(&multiplex);

        pthread_mutex_lock(&lmutex);
        leftCount--;
        if (leftCount == 0)
            sem_post(&crossingLock);
        pthread_mutex_unlock(&lmutex);

        printf("\tEsquerda terminou\n");

        sleep(1);
    }
}

void *rightBaboon(void *arg)
{
    while (1)
    {
        sem_wait(&turnstile);

        pthread_mutex_lock(&rmutex);
        rightCount++;
        if (rightCount == 1)
            sem_wait(&crossingLock);
        pthread_mutex_unlock(&rmutex);

        sem_post(&turnstile);

        sem_wait(&multiplex);

        printf("RIGHT -> LEFT\n");
        sleep(3);
        printf("%s", drawRight);

        sem_post(&multiplex);

        pthread_mutex_lock(&rmutex);
        rightCount--;
        if (rightCount == 0)
            sem_post(&crossingLock);
        pthread_mutex_unlock(&rmutex);
        printf("\tDireita terminou\n");

        sleep(1);
    }
}
int main()
{
    pthread_t left[NUM_BABOONS], right[NUM_BABOONS];

    sem_init(&crossingLock, 0, 1);
    sem_init(&turnstile, 0, 1);
    sem_init(&multiplex, 0, MAX_ON_ROPE);

    for (int i = 0; i < NUM_BABOONS; i++)
    {
        pthread_create(&left[i], NULL, leftBaboon, NULL);
        pthread_create(&right[i], NULL, rightBaboon, NULL);
    }

    for (int i = 0; i < NUM_BABOONS; i++)
    {
        pthread_join(left[i], NULL);
        pthread_join(right[i], NULL);
    }

    sem_destroy(&crossingLock);
    sem_destroy(&multiplex);
    sem_destroy(&turnstile);

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