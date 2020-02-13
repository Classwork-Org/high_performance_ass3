#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string.h>

using std::string;

#define PRINT_ERROR(description) printf("ERROR: %s\n", description)
#define PRINT_PHILOSOPER_INFO(philosopherIndex, description) printf("PHILOSOPHER[%d]: is %s\n", philosopherIndex, description)
#define PRINT_SIM_INFO(description) printf("SIM: %s\n", description)
#define MAX_PHILOSOPHER_COUNT 24
#define msec 1000
#define SUCCESS true
#define MAX_SLEEP_PERIOD 25
// #define PROLETARIAT
#define GREEDY
#define GET_FORKS_IN_ORDER
#define SIM_TERMINATION_TIME 5000

enum PhilosopherState
{
	THINKING,
	PICKING_UP_FORK,
	EATING,
	PUTTING_DOWN_FORKS
};

bool runFlag = true;
omp_lock_t runFlagLock;
unsigned int philosopherCount;
omp_lock_t forkLocks[MAX_PHILOSOPHER_COUNT];
unsigned int eatCounter[MAX_PHILOSOPHER_COUNT];

void simulation_terminator(unsigned int runtimeInmsec)
{
	printf("Terminator starting...\n");
	usleep(runtimeInmsec * msec);
	omp_set_lock(&runFlagLock);
	runFlag = false;
	omp_unset_lock(&runFlagLock);
}

unsigned int get_sleep_period()
{
#ifdef PROLETARIAT
	return MAX_SLEEP_PERIOD;
#else
	return rand() % MAX_SLEEP_PERIOD;
#endif
}

void seed_random_generator()
{
	static bool hasSeeded = false;
	if (!hasSeeded)
	{
		srand(time(NULL));
		hasSeeded = true;
	}
}

void sort(unsigned int *first, unsigned int *second)
{
	unsigned int temp;
	if (*first > *second)
	{
		temp = *first;
		*first = *second;
		*second = temp;
	}
}

const char* convert_enum(PhilosopherState state)
{
    switch(state)
    {
        case THINKING:
            return "THINKING";
        case EATING:
            return "EATING";
        case PUTTING_DOWN_FORKS:
            return "PUTTING_DOWN_FORKS";
        case PICKING_UP_FORK:
            return "PICKING_UP_FORKS";
        default:
            return "INVALID STATE!";
    }
}


bool pickup_fork(unsigned int forkNum)
{
#ifdef GREEDY
	omp_set_lock(&forkLocks[forkNum]);
	return SUCCESS;
#else
	return omp_test_lock(&forkLocks[forkNum]);
#endif
}

void putdown_fork(unsigned int forkNum)
{
	omp_unset_lock(&forkLocks[forkNum]);
}


void sit_on_table(unsigned int philosopherIndex)
{
	omp_set_lock(&runFlagLock);
	PhilosopherState state = THINKING;
	unsigned int forksCount = philosopherCount;
	unsigned int firstFork = philosopherIndex;
	unsigned int secondFork = (philosopherIndex + 1) % forksCount;
	bool tryToPickUpSecondForks = false;
	bool tryToPickUpFirstFork = false;

#ifdef GET_FORKS_IN_ORDER
	sort(&firstFork, &secondFork);
#endif

	printf("Philosopher [%d] will pickup first fork %d and second fork %d\n", philosopherIndex, firstFork, secondFork);

	while (runFlag)
	{
		omp_unset_lock(&runFlagLock);

		switch (state)
		{
		case THINKING:
		{
			PRINT_PHILOSOPER_INFO(philosopherIndex, convert_enum(state));
			unsigned int thinkingPeriod = get_sleep_period();
			usleep(msec * thinkingPeriod);
			state = PICKING_UP_FORK;
			break;
		}

		case PICKING_UP_FORK:
		{
			PRINT_PHILOSOPER_INFO(philosopherIndex, convert_enum(state));
			tryToPickUpFirstFork = pickup_fork(firstFork);
			tryToPickUpSecondForks = pickup_fork(secondFork);
			if (tryToPickUpFirstFork == SUCCESS && tryToPickUpSecondForks == SUCCESS)
			{
				state = EATING;
			}
			else
			{
				if (tryToPickUpFirstFork)
				{
					putdown_fork(firstFork);
				}
				if (tryToPickUpSecondForks)
				{
					putdown_fork(secondFork);
				}
				state = THINKING;
			}
			break;
		}

		case EATING:
		{
			PRINT_PHILOSOPER_INFO(philosopherIndex, convert_enum(state));
			unsigned int eatingPeriod = get_sleep_period();
			usleep(msec * eatingPeriod);
			eatCounter[philosopherIndex]++;
			state = PUTTING_DOWN_FORKS;
			break;
		}

		case PUTTING_DOWN_FORKS:
		{			
			PRINT_PHILOSOPER_INFO(philosopherIndex, convert_enum(state));
			putdown_fork(secondFork);
			putdown_fork(firstFork);
			state = THINKING;
			break;
		}

		default:
			break;
		}
		omp_set_lock(&runFlagLock);
	}
	omp_unset_lock(&runFlagLock);

	if (tryToPickUpFirstFork)
	{
		PRINT_PHILOSOPER_INFO(philosopherIndex, "WAS HOLDING FIRST FORK... LETTING GO...");
		putdown_fork(firstFork);
	}
	if (tryToPickUpSecondForks)
	{
		PRINT_PHILOSOPER_INFO(philosopherIndex, "WAS HOLDING SECOND FORK... LETTING GO...");
		putdown_fork(secondFork);
	}
	PRINT_PHILOSOPER_INFO(philosopherIndex, "LEAVING TABLE....");
}

int main(int argc, char *argv[])
{

	if(argc != 2)
	{
		PRINT_ERROR("Invalid number of arguments specified");
		return -1;
	}
	philosopherCount = atoi(argv[1]);
	if(philosopherCount > MAX_PHILOSOPHER_COUNT)
	{
		PRINT_ERROR("Invalid number of philosophers specified");
		return -1;
	}

	omp_set_num_threads(philosopherCount + 1);
	omp_init_lock(&runFlagLock);
	for(int i = 0; i<philosopherCount; i++)
	{
		omp_init_lock(&forkLocks[i]);
	}
	
	seed_random_generator();

	PRINT_SIM_INFO("Starting simulation....");

#pragma omp parallel
	{

		int tid = omp_get_thread_num();
		if (tid == philosopherCount)
		{
			simulation_terminator(SIM_TERMINATION_TIME);
		}
		else
		{
			sit_on_table(tid);
		}
	}

	omp_destroy_lock(&runFlagLock);
	
	PRINT_SIM_INFO("Performance results....");
	
	unsigned int totalConsumption = 0;
	for(int i = 0; i<philosopherCount; i++)
	{
		totalConsumption += eatCounter[i];
		printf("PHILOSOPHER[%d]: has eaten %d times\n", i, eatCounter[i]);
	}
	printf("Total Consumption : %d\n", totalConsumption);

	return 0;
}
