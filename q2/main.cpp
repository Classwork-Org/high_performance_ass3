#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <cstring>
#include <semaphore.h>

#define THREAD_COUNT 8
#define NUM_MAX 5000
#define PRINT_COUNT 10

#define PRINT_ERROR(Description) printf("%s : %s\n", "ERROR", Description)

unsigned int divBy3Count = 0;
unsigned int divBy5Count = 0;
unsigned int divBy3And5Count = 0;
bool divBy3Array[NUM_MAX+1];
bool divBy5Array[NUM_MAX+1];

sem_t divby3mutex;
sem_t divby5mutex;
sem_t divby3And5mutex;

typedef struct 
{
	unsigned int threadIdx; 
	unsigned int lower; 
	unsigned int upper; 
} ThreadArgs;

void* divisibilityChecker(void* _args)
{
	ThreadArgs args = *((ThreadArgs*)_args);
	
	unsigned int threadIdx = args.threadIdx; 
	unsigned int lower = args.lower; 
	unsigned int upper = args.upper;

	printf("THREAD[%u], Recieved lower: %u and upper %u \n", \
		threadIdx, lower, upper);

	for(unsigned int i = args.lower; i<args.upper; i++)
	{
		if(!(i%3)) // Divisible by 3
		{
			// TODO: Protect access to this (exclude array)
			sem_wait(&divby3mutex);
			divBy3Count++;
			sem_post(&divby3mutex);

			divBy3Array[i] = true;
		}

		if(!(i%5)) // Divisible by 5
		{
			// TODO: Protect access to this (exclude array)
			sem_wait(&divby5mutex);
			divBy5Count++;
			sem_post(&divby5mutex);

			divBy5Array[i] = true;
		}

		if(!(i%3) && !(i%5)) // Divisible by 3 and 5
		{
			// TODO: Protect access to this (exclude array)
			sem_wait(&divby3And5mutex);
			divBy3And5Count++;
			sem_post(&divby3And5mutex);
		}
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_t threads[THREAD_COUNT];
	ThreadArgs threadArgs[THREAD_COUNT];

	unsigned int segment = NUM_MAX/THREAD_COUNT;
	unsigned int lower = 1;
	unsigned int upper = segment+1;

	sem_init(&divby3mutex, 0, 1);
	sem_init(&divby5mutex, 0, 1);
	sem_init(&divby3And5mutex, 0, 1);

	memset(divBy3Array, false, sizeof(bool) * (NUM_MAX+1));
	memset(divBy5Array, false, sizeof(bool) * (NUM_MAX+1));

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		threadArgs[i].threadIdx = i;
		threadArgs[i].lower = lower;
		threadArgs[i].upper = upper;
		
		if(pthread_create(&threads[i], NULL, &divisibilityChecker, (void*)(&threadArgs[i])) != 0)
		{
			PRINT_ERROR("Failed to instantiate a thread... exiting...");
			return -1;
		}

		lower = upper;
		upper += segment;
	}

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		pthread_join(threads[i], NULL);
	}	

	printf("Divisible by 3 ONLY: %u\n", divBy3Count - divBy3And5Count);
	printf("Divisible by 5 ONLY: %u\n", divBy5Count - divBy3And5Count);
	printf("Divisible by BOTH 3 and 5: %u\n", divBy3And5Count);


	unsigned int printCount = 0;
	printf("First %d numbers divisible by 3\n", PRINT_COUNT);
	for (int i = 0;i < sizeof(divBy3Array)/sizeof(bool); ++i)
	{
		if(divBy3Array[i])
		{
			printf("%d ", i);
			printCount++;
			if(printCount>=PRINT_COUNT)
			{
				break;
			}
		}		
	}
	printf("\n");


	printCount = 0;
	printf("First %d numbers divisible by 5\n", PRINT_COUNT);
	for (int i = 0; i < sizeof(divBy5Array)/sizeof(bool); ++i)
	{
		if(divBy5Array[i])
		{
			printf("%d ", i);
			printCount++;
			if(printCount>=PRINT_COUNT)
			{
				break;
			}
		}		
	}
	printf("\n");


	printCount = 0;
	printf("First %d numbers divisible by 3 and 5\n", PRINT_COUNT);
	for (int i = 0;i < sizeof(divBy3Array)/sizeof(bool); ++i)
	{
		if(divBy3Array[i] && divBy5Array[i])
		{
			printf("%d ", i);
			printCount++;
			if(printCount>=PRINT_COUNT)
			{
				break;
			}
		}		
	}
	printf("\n");

	sem_destroy(&divby3mutex);
	sem_destroy(&divby5mutex);
	sem_destroy(&divby3And5mutex);

	return 0;
}