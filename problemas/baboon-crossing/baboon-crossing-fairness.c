#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_LEFT_BABOONS 10
#define NUM_RIGHT_BABOONS 10

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
        printf("[ESPERANDO] Esquerda | fila E=%d D=%d | corda E=%d D=%d | direção atual=%s\n",
            leftWaiting,
            rightWaiting,
            leftCrossing,
            rightCrossing,
            direction == LEFT ? "ESQUERDA" : "DIREITA"
        );

        pthread_cond_wait(&canLeft, &mutex);
    }

    leftWaiting--;
    leftCrossing++;

    printf("[ENTROU]    Esquerda | fila E=%d D=%d | corda E=%d D=%d\n",
        leftWaiting,
        rightWaiting,
        leftCrossing,
        rightCrossing
    );

    pthread_mutex_unlock(&mutex);

    printf("[TRAVESSIA] Esquerda atravessando -> direita\n");
    printf("%s", drawLeft);
    sleep(SLEEP_TIME);

    pthread_mutex_lock(&mutex);

    leftCrossing--;

    printf("[SAIU]      Esquerda | fila E=%d D=%d | corda E=%d D=%d\n",
        leftWaiting,
        rightWaiting,
        leftCrossing,
        rightCrossing
    );

    if (leftCrossing == 0 && rightWaiting > 0)
    {
        direction = RIGHT;

        printf("[TROCA]     Rodada da esquerda terminou. Direita assume agora.\n");

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
        printf("[ESPERANDO] Direita  | fila E=%d D=%d | corda E=%d D=%d | direção atual=%s\n",
            leftWaiting,
            rightWaiting,
            leftCrossing,
            rightCrossing,
            direction == LEFT ? "ESQUERDA" : "DIREITA"
        );

        pthread_cond_wait(&canRight, &mutex);
    }

    rightWaiting--;
    rightCrossing++;

    printf("[ENTROU]    Direita  | fila E=%d D=%d | corda E=%d D=%d\n",
        leftWaiting,
        rightWaiting,
        leftCrossing,
        rightCrossing
    );

    pthread_mutex_unlock(&mutex);

    printf("[TRAVESSIA] Direita atravessando -> esquerda\n");
    printf("%s", drawRight);
    sleep(SLEEP_TIME);

    pthread_mutex_lock(&mutex);

    rightCrossing--;

    printf("[SAIU]      Direita  | fila E=%d D=%d | corda E=%d D=%d\n",
        leftWaiting,
        rightWaiting,
        leftCrossing,
        rightCrossing
    );

    if (rightCrossing == 0 && leftWaiting > 0)
    {
        direction = LEFT;

        printf("[TROCA]     Rodada da direita terminou. Esquerda assume agora.\n");

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