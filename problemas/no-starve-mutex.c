#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_t 500

long int ticketCounter = 0;
long int nowServing = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t use;
void *commmonRoutine(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        long int myTicket = ticketCounter++;
        pthread_mutex_unlock(&mutex);

        while (1)
        {
            pthread_mutex_lock(&mutex);
            if (nowServing == myTicket)
            {
                pthread_mutex_unlock(&mutex);
                break;
            }
            pthread_mutex_unlock(&mutex);
            usleep(1000);
        }

        sem_wait(&use);

        printf("Ticket %ld turn\n", myTicket);
        sleep(3);

        sem_post(&use);

        pthread_mutex_lock(&mutex);
        nowServing++;
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }

    return NULL;
}

int main()
{
    pthread_t t[NUM_t];

    pthread_mutex_init(&mutex, 0);
    sem_init(&use, 0, 1);
    int i;


    for (int i = 0; i < NUM_t; i++)
    {
        pthread_create(&t[i], NULL, commmonRoutine, NULL);
    }

    for (i = 0; i < NUM_t; i++)
    {
        pthread_join(t[i], NULL);
    }

    pthread_mutex_destroy(&mutex);    
    sem_destroy(&use);

    return 0;
}