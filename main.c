#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_PHILOSOPHER 5

enum State
{
	THINKING,
	HUNGRY,
	EATING
};

enum State state_et[NUM_PHILOSOPHER];
int philosophers[NUM_PHILOSOPHER];
bool philosopherHasEaten[NUM_PHILOSOPHER];

sem_t mutex;
sem_t semaphore[NUM_PHILOSOPHER];

void check_forks(int i)
{
	int left = (i + 4) % NUM_PHILOSOPHER;
	int right = (i + 1) % NUM_PHILOSOPHER;

	if(state_et[i] == HUNGRY && state_et[left] != EATING && state_et[right] != EATING)
	{
		state_et[i] = EATING;
		
		printf("Philosopher %d picks up forks %d and %d.\n",i+1, left + 1, right + 1);
		printf("Philosopher %d is EATING.\n",i+1);

		sem_post(&semaphore[i]);
	}
}

void pickup_forks(int i)
{
	sem_wait(&mutex);

	//change state
	state_et[i] = HUNGRY;

	printf("Philosopher %d is HUNGRY.\n",i+1);

	check_forks(i);

	sem_post(&mutex);

	sem_wait(&semaphore[i]);
}

void return_forks(int i)
{
	sem_wait(&mutex);

	//change state
	state_et[i] = THINKING;

	printf("Philosopher %d returned the forks %d and %d back.\n",i+1, ((i + 4) % NUM_PHILOSOPHER) + 1, ((i + 1) % NUM_PHILOSOPHER) + 1);
	printf("Philosopher %d is THINKING.\n", i+1);

	check_forks(((i + 4) % NUM_PHILOSOPHER));
	check_forks(((i + 1) % NUM_PHILOSOPHER));

	sem_post(&mutex);
}

void* philosopher(void* num)
{
	while(1)
	{
		int *i = num;
		int delay = (rand() % 3) + 1;

		sleep(delay);

		pickup_forks(*i);
		
		delay = (rand() % 3) + 1;
		//eat
		sleep(delay);

		return_forks(*i);

		pthread_exit(0);
	}
}


int main()
{
	int i = 0;
	pthread_t philosopherThreads[NUM_PHILOSOPHER];
	srand(time(NULL));

	for(; i < NUM_PHILOSOPHER; i++)
		philosophers[i] = i;

	sem_init(&mutex, 0, 1);

	for(i = 0; i < NUM_PHILOSOPHER; i++)
		sem_init(&semaphore[i], 0, 0);
	
	for(i = 0; i < NUM_PHILOSOPHER; i++)
	{
		pthread_create(&philosopherThreads[i], NULL, philosopher,(void*) &philosophers[i]);
		printf("Philosopher %d is thinking\n",i+1);
	}

	for(i = 0; i< NUM_PHILOSOPHER; i++)
		pthread_join(philosopherThreads[i], NULL);
	
	return 0;
}
	
