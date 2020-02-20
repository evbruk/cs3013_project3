#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define SQUEEZE 0
#define SOAK 1
#define SHOCK 2
#define SCORCH 3

int grad1[4];
int grad2[4];
int grad3[4];
int grad4[4];

int grad1Steps = 0;
int grad2Steps = 0;
int grad3Steps = 0;
int grad4Steps = 0;

sem_t squeezeLock;
sem_t soakLock;
sem_t shockLock;
sem_t scorchLock;

sem_t moveLock;


struct gradArguments
{
	int * moveArray;			//list of moves that are 1-4
	int numMoves;				//the moves listed in array above
	int id;					// to reference grad students but not really used
	char * object;       			// one of the values in the objectValue
	char * studentName;			//the grad student's name
};

//bucket of 15 random items
char * objectBucket[] = {"sponge", "ball", "gum", "toothbrush", "2x4", "flip flop", "book", "jacket", "cup", "toothpaste", "shampoo", "pencil", "glass", "bottle", "jar"};

void initStudent(int * gradArray, int numSteps)
{
	int index = 0;
	//init array values to -1
	for(int i = 0; i<numSteps; i++)
	{
		gradArray[i] = -1;
	}
	
	//comb through array generating moves and making sure there are no repeats
	while(index < numSteps)
	{
		int value = (rand() % 4);
		int unique = 1;
		for(int i = 0; i < index; i++)
		{
			//printf("i: %d\n", i);
			if(gradArray[i] == value)
			{
				unique = 0;
			}		
		}
		if(unique)
		{
			gradArray[index] = value;
			index++;
		}
	}
/*
	printf("Value of steps:\n");
	for(int i = 0; i<numSteps; i++)
	{
		switch(gradArray[i])
		{
			case SQUEEZE:
				printf("Squeeze\n");
			break;
			case SOAK:
				printf("Soak\n");
			break;
			case SHOCK:
				printf("Shock\n");
			break;
			case SCORCH:
				printf("Scorch\n");
			break;
			default:
				printf("DEFAULT...\n");
			break;
		}
	
	}*/
}

void printMoves(int * moveArray, int numMoves)
{
	for(int i = 0; i < numMoves; i++)
	{
		switch(moveArray[i])
		{
			case SQUEEZE:
				printf("Squeeze");
			break;
			case SOAK:
				printf("Soak");
			break;
			case SHOCK:
				printf("Shock");
			break;
			case SCORCH:
				printf("Scorch");
			break;
			default:
				printf("DEFAULT...");
			break;
		}
		if(i < numMoves-1)
			printf(", ");	
	}
}

void initStudents() 
{
	grad1Steps = (rand() % 4) + 1;
	grad2Steps = (rand() % 4) + 1;
	grad3Steps = (rand() % 4) + 1;
	grad4Steps = (rand() % 4) + 1;
	printf("init grad1... %d steps\n", grad1Steps);
	initStudent(grad1, grad1Steps);
	printf("init grad2... %d steps\n", grad2Steps);	
	initStudent(grad2, grad2Steps);
	printf("init grad3... %d steps\n", grad3Steps);	
	initStudent(grad3, grad3Steps);
	printf("init grad4... %d steps\n", grad4Steps);
	initStudent(grad4, grad4Steps);
}

//takes in moveArray, AND number of steps
void * gradStudent(void * vargp)
{
	struct gradArguments *moveSet = (struct gradArguments *)vargp;				//cast it
	printf("Grad student created! numMoves: %d \n", moveSet->numMoves);
	
	while(1)
	{
		printf("%s is grabbing a %s and will perform [", moveSet->studentName, moveSet->object);
		printMoves(moveSet->moveArray, moveSet->numMoves);
		printf("] on it! \n");
			
		for(int i = 0; i < moveSet->numMoves; i++)
		{

			sem_wait(&moveLock);				//only one grad student can move at a time

			int sleepValue = rand() % 250;

			switch(moveSet->moveArray[i])
			{
				case SQUEEZE:
					printf("[%s] waiting to squeeze %s... \n", moveSet->studentName, moveSet->object);					
					sem_wait(&squeezeLock);
					printf("[%s] Squeezing %s... \n", moveSet->studentName, moveSet->object);
					usleep(sleepValue*1000);
					sem_post(&squeezeLock);				
				break;
				case SOAK:
					printf("[%s] waiting to soak %s... \n", moveSet->studentName, moveSet->object);					
					sem_wait(&soakLock);
					printf("[%s] Soaking %s... \n", moveSet->studentName, moveSet->object);
					usleep(sleepValue*1000);					
					sem_post(&soakLock);				
				break;
				case SHOCK:
					printf("[%s] waiting to shock %s... \n", moveSet->studentName, moveSet->object);
					sem_wait(&shockLock);
					printf("[%s] Shocking %s... \n", moveSet->studentName, moveSet->object);
					usleep(sleepValue*1000);
					sem_post(&shockLock);				
				break;
				case SCORCH:
					printf("[%s] waiting to scorch %s... \n", moveSet->studentName, moveSet->object);					
					sem_wait(&scorchLock);
					printf("[%s] Scorching %s... \n", moveSet->studentName, moveSet->object);					
					usleep(sleepValue*1000);	
					sem_post(&scorchLock);				
				break;
				default:
				printf("[%s] Unrecognized move! value: %d \n", moveSet->studentName, moveSet->moveArray[i]);
				break;		
			}
			sem_post(&moveLock);
		}
		//get a new item and new moves;
		moveSet->object = objectBucket[(rand() % 14) + 1];
		moveSet->numMoves = ((rand() % 3) + 1);
		initStudent(moveSet->moveArray, moveSet->numMoves);
		printf("%s has new moves on new object %s! [", moveSet->studentName, moveSet->object);
		printMoves(moveSet->moveArray, moveSet->numMoves);
		printf("]!\n");
	}
	
}

int main(int argc, char * argv[])
{
	srand(time(0));
	initStudents();
	sem_init(&squeezeLock, 0, 1);
	sem_init(&soakLock, 0, 1);
	sem_init(&shockLock, 0, 1);
	sem_init(&scorchLock, 0, 1);
	sem_init(&moveLock, 0, 1);

	pthread_t gradStudent1;
	pthread_t gradStudent2;
	pthread_t gradStudent3;
	pthread_t gradStudent4;
	
	struct gradArguments grad1Args;
	grad1Args.moveArray = grad1;
	grad1Args.numMoves = grad1Steps;
	grad1Args.id = 1;
	grad1Args.object = objectBucket[(rand() % 14)];
	grad1Args.studentName = "Greg";

	struct gradArguments grad2Args;
	grad2Args.moveArray = grad2;
	grad2Args.numMoves = grad2Steps;
	grad2Args.id = 2;
	grad2Args.object = objectBucket[(rand() % 14)];
	grad2Args.studentName = "Samantha";	

	struct gradArguments grad3Args;
	grad3Args.moveArray = grad3;
	grad3Args.numMoves = grad3Steps;
	grad3Args.id = 3;
	grad3Args.object = objectBucket[(rand() % 14)];
	grad3Args.studentName = "Alice";

	struct gradArguments grad4Args;
	grad4Args.moveArray = grad4;
	grad4Args.numMoves = grad4Steps;
	grad4Args.id = 4;
	grad4Args.object = objectBucket[(rand() % 14)];	
	grad4Args.studentName = "Bob";

	//pthread_create(&archie_dog, NULL, dog, "Archie");
	pthread_create(&gradStudent1, NULL, gradStudent, &grad1Args);
	pthread_create(&gradStudent2, NULL, gradStudent, &grad2Args);
	pthread_create(&gradStudent3, NULL, gradStudent, &grad3Args);
	pthread_create(&gradStudent4, NULL, gradStudent, &grad4Args);


	while(1){};
}



