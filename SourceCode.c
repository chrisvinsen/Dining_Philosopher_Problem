#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

struct Philosopher {
	int number;
	int eatenTimes;	
	int leftChopstickIndex;
	int rightChopstickIndex;
	pthread_t threadID;
};

struct Chopstick {
	int index;
	sem_t mutex;
};

int is_finished();
void think(struct Philosopher *philosopher);
void eat(struct Philosopher *philosopher);
void put_chopsticks(struct Philosopher *philosopher, char chopstickPosition[]);
void take_chopsticks(struct Philosopher *philosopher);

void actionTime();

struct Chopstick* chopsticks;
sem_t globalMutex;
int currentFoods = 0;

void* philosopher_thread(void *argument) {
	struct Philosopher* philosopher = (struct Philosopher*)argument;

	while(1) {
        // Philosopher start thinking
        think(philosopher);
		
		// Philosopher wants to eat
		printf("Philosopher %d is hungry and wants to eat...\n", philosopher->number);

        // Check if Bakpao is exists
        if (currentFoods > 0) {
            // Philosopher take the chopsticks
            take_chopsticks(philosopher);
        } else {
            // Bakpao doesn't exists
            printf("Philosopher %d failed to Eat because the Bakpao ran out\n", philosopher->number);
        }
        
        // Checks whether all Philosophers have finished eating
        if (is_finished()) break;
	}
}

void main() {
	struct Philosopher* philosophers;
	int optionChosen, i, numberOfPhilosophers = 5, numberOfPhilosophersRequested, numberOfFoods = 10, numberOfFoodsRequested;
	srand((unsigned int)time(NULL));

    while(1) {
        printf("=====================================================================\n");
        printf("          Welcome to DINING PHILOSOPHERS Simulation Program          \n");
        printf("=====================================================================\n");
        printf("Current number of Philosopher = %d\n", numberOfPhilosophers);
        printf("Current number of Food = %d\n", numberOfFoods);
        printf("1. Change the number of Philosophers\n");
        printf("2. Change the number of Bakpao\n");
        printf("3. Start Simulation\n");
        printf("0. Exit\n");
        printf("Your choice: ");
        scanf("%d", &optionChosen);
        
        if (optionChosen == 1) {
            // Change the number of Philosophers
            printf("\nPlease enter the number of Philosophers (2 ~ 100) : ");
            scanf("%d", &numberOfPhilosophersRequested);

            // Check if the new number of Philosopher is valid
            if (numberOfPhilosophersRequested >= 2 && numberOfPhilosophersRequested <= 100) {
                numberOfPhilosophers = numberOfPhilosophersRequested;
                printf("\nNumber of Philosophers has been changed to %d\n\n", numberOfPhilosophers);
            } else {
                printf("\nFailed to change the number of Philosophers to %d\n", numberOfPhilosophersRequested);
                printf("The number of Philosophers allowed is between 2 - 100.\n\n");
            }
        } else if (optionChosen == 2) {
            // Change the number of Bakpao
            printf("\nPlease enter the number of Bakpao (1 ~ 100) : ");
            scanf("%d", &numberOfFoodsRequested);

            // Check if the new number of Bakpao is valid
            if (numberOfFoodsRequested >= 1 && numberOfFoodsRequested <= 100) {
                numberOfFoods = numberOfFoodsRequested;
                printf("\nNumber of Bakpao has been changed to %d\n\n", numberOfFoods);
            } else {
                printf("\nFailed to change the number of Bakpao to %d\n", numberOfFoodsRequested);
                printf("The number of Bakpao allowed is between 1 - 100.\n\n");
            }
        } else if (optionChosen == 3) {
            // Simulation Dining Philosopher Program
            printf("=====================================================================\n");
            printf("                          SIMULATION BEGIN                           \n");
            printf("=====================================================================\n");

            // Create array of struct for philosophers
            philosophers = (struct Philosopher*) malloc(sizeof(struct Philosopher) * numberOfPhilosophers);

            // Create array of struct for chopsticks
            chopsticks = (struct Chopstick*) malloc(sizeof(struct Chopstick) * numberOfPhilosophers);

            // Initialize global mutex in order to protect the critical region
            sem_init(&globalMutex, 0, 1);

            // Set number of foods to current foods, current foods value will always decrease every time the Philosopher eats
            currentFoods = numberOfFoods;
            printf("======================= AVAILABLE BAKPAO : %d =======================\n", currentFoods);

            for(i = 0; i < numberOfPhilosophers; i++) {
                // Initialize mutex of chopstick
                sem_init(&chopsticks[i].mutex, 0, 1);

                // Set number to Philosopher (Naming Philosopher)
                philosophers[i].number = i + 1;
                // Set number of Philosopher eating to zero
                philosophers[i].eatenTimes = 0;
                // Set index of left chopstick
                philosophers[i].leftChopstickIndex = i;
                // Set index of right chopstick
                if ((i + 1) == numberOfPhilosophers)
                    philosophers[i].rightChopstickIndex = 0;
                else
                    philosophers[i].rightChopstickIndex = i+1;
            }

            // Create thread for each Philosopher
            for(i = 0; i < numberOfPhilosophers; i++)
                pthread_create(&philosophers[i].threadID, NULL, philosopher_thread, &philosophers[i]);

            // Wait until all threads are finished
            for(i = 0; i < numberOfPhilosophers; i++)
                pthread_join(philosophers[i].threadID, NULL);

            // Prints Philosopher Statistics
            printf("\n=====================================================================\n");
            printf("                       PHILOSOPHERS STATISTICS                       \n");
            printf("=====================================================================\n");
            for(i = 0; i < numberOfPhilosophers; i++)
                printf("Philosopher %d eaten for %d times\n", philosophers[i].number, philosophers[i].eatenTimes);
                
            // Free all dynamically allocated memory
            free(chopsticks);
            free(philosophers);	

            printf("\n=====================================================================\n");
            printf("                           SIMULATION ENDS                           \n");
            printf("=====================================================================\n\n");
        } else if (optionChosen == 0) {
            printf("\nThank you for using this program\n");
            printf(" 1. Christianto Vinsen B. - 00000028917\n");
            printf(" 2. Delvin Chianardi      - 00000028583\n");
            break;
        } else {
            printf("The option you entered is wrong. Please try again\n\n");
        }
    }
}

int is_finished() {
    // Use global mutex to enter the critical region
	sem_wait(&globalMutex);
	int temp = currentFoods;
	sem_post(&globalMutex);	

    // return 1 (true) if current foods is empty or zero
	return temp == 0;
}

void think(struct Philosopher *philosopher) {
    // Philosopher start thinking
    printf("Philosopher %d is Thinking\n", philosopher->number);

    // Do something for delay thinking
    actionTime();
}

void eat(struct Philosopher *philosopher) {
    // Philosopher start eating
    printf("Philosopher %d started Eating\n", philosopher->number);

    // Use global mutex to enter the critical region
    sem_wait(&globalMutex);
    // Check if Bakpao is exists
    if (currentFoods > 0) {
        // Reduce the number of Bakpao by one
        currentFoods--;
        printf("======================= AVAILABLE BAKPAO : %d =======================\n", currentFoods);
        // Increase the number of eaten times of Philosopher by one
        philosopher->eatenTimes++;
    } else {
        // Bakpao doesn't exists
        printf("Philosopher %d failed to Eat because the Bakpao ran out\n", philosopher->number);
    }
    sem_post(&globalMutex);
    
    // Delay time for the Philosopher to eat. Random Delay from 1.00 to 5.00 second
    usleep(1000000 + (100000 * (rand() % 41)));
}

void put_chopsticks(struct Philosopher *philosopher, char chopstickPosition[]) {
    // Check which chopsticks to put back on the table
    if (strcmp(chopstickPosition, "left") == 0) 
        // Put the left chopstick back on the table
        sem_post(&chopsticks[philosopher->leftChopstickIndex].mutex);
    else if (strcmp(chopstickPosition, "right") == 0)
        // Put the right chopstick back on the table
        sem_post(&chopsticks[philosopher->rightChopstickIndex].mutex);
}

void take_chopsticks(struct Philosopher *philosopher) {
    // Try to get left chopstick, here use sem_trywait instead of sem_wait because it makes possible to resolve deadlock
    if (sem_trywait(&chopsticks[philosopher->leftChopstickIndex].mutex) == 0) {
        // Generate random waiting time for take the right chopstick. Random waiting time from 10 to 59 interval
        int waitingTimes = 10 + rand() % 50; 

        // Check whether the waiting time to take the right chopstick is still available
        while(waitingTimes > 0) {
            // Try to get right chopstick
            if (sem_trywait(&chopsticks[philosopher->rightChopstickIndex].mutex)==0) {	
                // Philosopher has both chopsticks and is ready to eat
                eat(philosopher);	

                // After eating, the two chopsticks were put back on the table
                put_chopsticks(philosopher, "right");
                put_chopsticks(philosopher, "left");

                // Philosopher finished eating and also finished returning the chopsticks to the table
                printf("Philosopher %d finished Eating\n", philosopher->number);

                break;
            } else {
                // Philosopher have not been able to take the right chopstick because chopstick are still used by other Philosophers
                // Reduce timer for waiting right chopstick by one
                waitingTimes--;
                // Delay for 0.1 second
                usleep(100000);					
            }
        }

        // If the waiting time is 0, it means Philosopher cannot get the right chopstick and the waiting time is up
        // So the Philosopher will return the left chopstick to the table even though he is hungry
        if (waitingTimes == 0) {
            printf("Philosopher %d cannot take second chopstick...\n", philosopher->number);
            // Put the left chopstick on the table
            put_chopsticks(philosopher, "left");
        }
    } else {
        // Philosopher have not been able to take the left chopstick because chopstick are still used by other Philosophers
        printf("Philosopher %d cannot eat at this moment...\n", philosopher->number);
    }
}

void actionTime() { 
    int arraySize; 
    // Generate random array size from 30.000 to 50.000 with multiples of 2500
    arraySize = 30000 + (2500 * (rand() % 9));

    int arr[arraySize];
    int tempArraySize = arraySize;

    // Initialize array before, ex: random size = 3, then the array value = 3, 2, 1
    for (int i = 0; i < arraySize; i++) {
        arr[i] = tempArraySize--;
    }

    // Doing Bubble Sort, ex: array size = 3, Before = 3, 2, 1 --> After = 1, 2, 3
    int n = sizeof(arr)/sizeof(arr[0]); 
    for (int i = 0; i < n-1; i++)       
        for (int j = 0; j < n-i-1; j++)  
            if (arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
} 