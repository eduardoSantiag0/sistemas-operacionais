#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_LEFT_BABOONS 15
#define NUM_RIGHT_BABOONS 5

#define SLEEP_TIME 3
#define MAX_ON_ROPE 5
#define MAX_DELAY_US 500000

#define LEFT 0
#define RIGHT 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int leftWaiting = 0;
int leftCrossing = 0;

int rightWaiting = 0;
int rightCrossing = 0;

int direction = LEFT;

pthread_cond_t canLeft = PTHREAD_COND_INITIALIZER;
pthread_cond_t canRight = PTHREAD_COND_INITIALIZER;

char *drawRight;
char *drawLeft;

void *leftBaboon(void *arg)
{
    pthread_mutex_lock(&mutex);

    leftWaiting++;

    while (direction != LEFT 
        || rightCrossing > 0 
        || leftCrossing == MAX_ON_ROPE
        || (rightWaiting > 0 && leftCrossing > 0)
    ) 
    {

        pthread_cond_wait(&canLeft, &mutex);
    }

    leftWaiting--;
    leftCrossing++;

    pthread_mutex_unlock(&mutex);

    printf("%s", drawLeft);
    sleep(SLEEP_TIME);

    pthread_mutex_lock(&mutex);

    leftCrossing--;


    if (leftCrossing == 0 && rightWaiting > 0)
    {
        direction = RIGHT;

        printf("Rodada da esquerda terminou. Direita assume agora.\n");

        pthread_cond_broadcast(&canRight);
    }
    else
    {
        pthread_cond_broadcast(&canLeft);
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *rightBaboon(void *arg)
{
    pthread_mutex_lock(&mutex);

    rightWaiting++;

    while (direction != RIGHT 
        || leftCrossing > 0 
        || rightCrossing == MAX_ON_ROPE
        || (leftWaiting > 0 && rightCrossing > 0)
    )
    {
        pthread_cond_wait(&canRight, &mutex);
    }

    rightWaiting--;
    rightCrossing++;


    pthread_mutex_unlock(&mutex);

    printf("%s", drawRight);
    sleep(SLEEP_TIME);

    pthread_mutex_lock(&mutex);

    rightCrossing--;

    if (rightCrossing == 0 && leftWaiting > 0)
    {
        direction = LEFT;

        printf("Rodada da direita terminou. Esquerda assume agora.\n");

        pthread_cond_broadcast(&canLeft);
    }
    else
    {
        pthread_cond_broadcast(&canRight);
    }

    pthread_mutex_unlock(&mutex);

    return NULL;    
}

int main()
{
    pthread_t left[NUM_LEFT_BABOONS];
    pthread_t right[NUM_RIGHT_BABOONS];

    int createdLeft = 0;
    int createdRight = 0;

    srand(time(NULL));

    struct timespec start, end;
    double timeDiff;

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

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&canLeft);
    pthread_cond_destroy(&canRight);

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