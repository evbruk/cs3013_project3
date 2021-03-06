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

sem_t gregLock;
sem_t bobLock;
sem_t aliceLock;
sem_t samanthaLock;

sem_t moveLock;

int gregState = -1;
int bobState = -1;
int aliceState = -1;
int samanthaState = -1;


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
}

int * getMyState(int id)
{
	switch(id)
	{
		case 1:
			return &gregState;
		break;
		case 2:
			return &samanthaState;
		break;
		case 3:
			return &aliceState;
		break;
		case 4:
			return &bobState;
		break;	
	}
}

sem_t * getMySemaphore(int id)
{
	switch(id)
	{
		case 1:
			return &gregLock;
		break;
		case 2:
			return &samanthaLock;
		break;
		case 3:
			return &aliceLock;
		break;
		case 4:
			return &bobLock;
		break;	
	}
}

//check each grad student's state and check if it is equal to the supplied stationId
//if it is, return that students ID. if it is not, return -1;
int isTaken(int stationId)
{
	if(bobState == stationId)
	{
		return 4;	
	}else if(aliceState == stationId)
	{
		return 3;	
	}else if(samanthaState == stationId)
	{
		return 2;	
	}else if(gregState == stationId)
	{
		return 1;	
	}

	return -1;
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

void printAction(char * studentName, char * objectName, int moveId)
{
	switch(moveId)
	{
		case SQUEEZE:
			printf("[%s] Squeezing %s... \n", studentName, objectName);
		break;
		case SOAK:
			printf("[%s] Soaking %s... \n", studentName, objectName);
		break;
		case SHOCK:
			printf("[%s] Shocking %s... \n", studentName, objectName);
		break;
		case SCORCH:
			printf("[%s] Scorching %s... \n", studentName, objectName);					
		break;
	}
			
}

//takes in moveArray, AND number of steps
//Each thread runs their own copy of this function!
void * gradStudent(void * vargp)
{
	struct gradArguments *moveSet = (struct gradArguments *)vargp;				//cast it
	printf("Grad student created! numMoves: %d \n", moveSet->numMoves);
	
	while(1)
	{
		printf("%s is grabbing a %s and will perform [", moveSet->studentName, moveSet->object);
		printMoves(moveSet->moveArray, moveSet->numMoves);
		printf("] on it! \n");
		
		//condition to keep looping
		int experimenting = 1;
		int moveIndex = 0;
		int * myState = 0;	
		while(experimenting)
		{
				printf("[%s] is waiting to move... \n", moveSet->studentName);
				sem_wait(&moveLock);
				myState = getMyState(moveSet->id);
	
				sem_t * mySemaphore = getMySemaphore(moveSet->id);				
				sem_wait(mySemaphore);
				//printf("it is %s's turn to move. \n", moveSet->studentName);
				if(*myState == -1)
				{
					//it is my first move;		
									
				}
				int desiredStation = moveSet->moveArray[moveIndex];
				int taken = 0;
				int isTakenId = isTaken(desiredStation);
				
				if(isTakenId > 0 && isTakenId != moveSet->id)
				{
					taken = 1;
					sem_t * waitOnThis = getMySemaphore(isTakenId);
					//printf("[%s] is waiting on student with id: %d\n", moveSet->studentName, isTakenId);
					sem_wait(waitOnThis);
					//update my state because the other guy is done.
					*myState = desiredStation;
					printAction(moveSet->studentName, moveSet->object, desiredStation);
					usleep(((rand() % 250) + 1) * 1000);
					
					moveIndex++;
					sem_post(waitOnThis);				
				}else if(isTakenId == moveSet->id)
				{
					//we already hold our desired station.
					printAction(moveSet->studentName, moveSet->object, desiredStation);
					usleep(((rand() % 250) + 1) * 1000);
					moveIndex++;
					//printf("[%s] holds station %d... \n", moveSet->studentName, desiredStation);				
				}else{
					//it is not taken so we should take it.
					
					*myState = desiredStation;
					printAction(moveSet->studentName, moveSet->object, desiredStation);
					usleep(((rand() % 250) + 1) * 1000);
					moveIndex++;
					//sleep?				
				}

				sem_post(mySemaphore);
				sem_post(&moveLock);

				if( moveIndex > moveSet->numMoves)
				{
					moveSet->object = objectBucket[(rand() % 14) + 1];
					moveSet->numMoves = ((rand() % 4) + 1);
					initStudent(moveSet->moveArray, moveSet->numMoves);
					printf("%s has new moves on new object %s! [", moveSet->studentName, moveSet->object);
					printMoves(moveSet->moveArray, moveSet->numMoves);
					printf("]!\n");
					moveIndex = 0;
				}
				//sleep(5);
				//see what the other students are doing and make a decision?
		}
	}
}



int main(int argc, char * argv[])
{
	srand(time(0));
	initStudents();
	sem_init(&bobLock, 0, 1);
	sem_init(&samanthaLock, 0, 1);
	sem_init(&aliceLock, 0, 1);
	sem_init(&gregLock, 0, 1);
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



