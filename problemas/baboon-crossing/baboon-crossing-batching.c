//A direção atual tem direito a continuar por um pouco, 
// mesmo que o outro lado já esteja esperando.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>


#define MAX_BATCH 7
#define MIN_BATCH 5
#define SLEEP_TIME 4

#define MAX_ON_ROPE 5

#define LEFT 0
#define RIGHT 1

#define NUM_LEFT_BABOONS 10
#define NUM_RIGHT_BABOONS 10
#define MAX_DELAY_US 500000
#define TIMEOUT_US 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t canLeft = PTHREAD_COND_INITIALIZER;
pthread_cond_t canRight = PTHREAD_COND_INITIALIZER;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int leftWaiting = 0;
int leftCrossing = 0;

int rightWaiting = 0;
int rightCrossing = 0;

int direction = LEFT;
int batchCount = 0;

char *drawRight;
char *drawLeft;


void unlock_mutex(void *arg)
{
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}

void *leftBaboon(void *arg)
{
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex);

    // Para liberar o mutex caso a thread seja cancelada enquanto espera
    pthread_cleanup_push(unlock_mutex, &mutex);

    leftWaiting++;

    while (
        direction != LEFT ||
        rightCrossing > 0 ||
        leftCrossing == MAX_ON_ROPE ||
        (rightWaiting > 0 && batchCount >= MAX_BATCH)
    )
    {
        pthread_cond_wait(&canLeft, &mutex);
    }

    leftWaiting--;
    leftCrossing++;
    batchCount++;
    pthread_cleanup_pop(1); // libera mutex

    // pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&printMutex);
    printf("LEFT id %d -> RIGHT | batch: %d\n", id, batchCount);
    printf("%s", drawLeft);
    pthread_mutex_unlock(&printMutex);

    sleep(SLEEP_TIME);

    pthread_mutex_lock(&mutex);

    leftCrossing--;

    if (leftCrossing == 0)
    {
        if (rightWaiting > 0 && (batchCount >= MIN_BATCH || leftWaiting == 0))
        {
            direction = RIGHT;
            batchCount = 0;

            pthread_mutex_lock(&printMutex);
            printf("\tEsquerda terminou a rodada. Direita assume.\n");
            pthread_mutex_unlock(&printMutex);

            pthread_cond_broadcast(&canRight);
        }
        else
        {
            pthread_cond_broadcast(&canLeft);
        }
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
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex);
    // Para liberar o mutex caso a thread seja cancelada enquanto espera
    pthread_cleanup_push(unlock_mutex, &mutex);

    rightWaiting++;

    while (
        direction != RIGHT ||
        leftCrossing > 0 ||
        rightCrossing == MAX_ON_ROPE ||
        (leftWaiting > 0 && batchCount >= MAX_BATCH)
    )
    {
        pthread_cond_wait(&canRight, &mutex);
    }

    rightWaiting--;
    rightCrossing++;
    batchCount++;

    pthread_cleanup_pop(1); 

    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&printMutex);
    printf("RIGHT id %d -> LEFT | batch: %d\n", id, batchCount);
    printf("%s", drawRight);
    pthread_mutex_unlock(&printMutex);

    sleep(SLEEP_TIME);

    pthread_mutex_lock(&mutex);

    rightCrossing--;

    if (rightCrossing == 0)
    {
        if (leftWaiting > 0 && (batchCount >= MIN_BATCH || rightWaiting == 0))
        {
            direction = LEFT;
            batchCount = 0;

            pthread_mutex_lock(&printMutex);
            printf("\tDireita terminou a rodada. Esquerda assume.\n");
            pthread_mutex_unlock(&printMutex);

            pthread_cond_broadcast(&canLeft);
        }
        else
        {
            pthread_cond_broadcast(&canRight);
        }
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

    pthread_cond_init(&cond, NULL);

    int leftIds[NUM_LEFT_BABOONS];
    int rightIds[NUM_RIGHT_BABOONS];

    int createdLeft = 0;
    int createdRight = 0;

    srand(time(NULL));

    for (int i = 0; i < NUM_LEFT_BABOONS; i++)
    {
        leftIds[i] = i + 1;
    }

    for (int i = 0; i < NUM_RIGHT_BABOONS; i++)
    {
        rightIds[i] = i + 1;
    }

    while (createdLeft < NUM_LEFT_BABOONS || createdRight < NUM_RIGHT_BABOONS)
    {
        int chooseLeft = rand() % 2;

        if (chooseLeft && createdLeft < NUM_LEFT_BABOONS)
        {
            pthread_create(&left[createdLeft], NULL, leftBaboon, &leftIds[createdLeft]);
            createdLeft++;
        }
        else if (createdRight < NUM_RIGHT_BABOONS)
        {
            pthread_create(&right[createdRight], NULL, rightBaboon, &rightIds[createdRight]);
            createdRight++;
        }
        else if (createdLeft < NUM_LEFT_BABOONS)
        {
            pthread_create(&left[createdLeft], NULL, leftBaboon, &leftIds[createdLeft]);
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

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&printMutex);

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