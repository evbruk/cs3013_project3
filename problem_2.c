#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

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

struct gradArguments
{
	int * moveArray;
	int numMoves;
	int id;
};

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
		int value = (rand() % 3) + 1;
		int unique = 1;		
		for(int i = 0; i < index; i++)
		{
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
	
	}
}

void initStudents() 
{
	grad1Steps = (rand() % 3) + 1;
	grad2Steps = (rand() % 3) + 1;
	grad3Steps = (rand() % 3) + 1;
	grad4Steps = (rand() % 3) + 1;
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
	struct gradArguments *moveSet = (struct gradArguments *)vargp;
	printf("Grad student created! numMoves: %d \n", moveSet->numMoves);

	for(int i = 0; i < moveSet->numMoves; i++)
	{
		switch(moveSet->moveArray[i])
		{
			case SQUEEZE:
				printf("[%d] Attempting to squeeze... \n", moveSet->id);
			break;
			case SOAK:
				printf("[%d] Attempting to soak... \n", moveSet->id);
			break;
			case SHOCK:
				printf("[%d] Attempting to shock... \n", moveSet->id);
			break;
			case SCORCH:
				printf("[%d] Attempting to scorch... \n", moveSet->id);
			break;
			default:
			printf("[%d] unrecognized move! value: %d \n", moveSet->id, moveSet->moveArray[i]);
			break;		
		}	
	}
}

int main(int argc, char * argv[])
{
	srand(time(0));
	initStudents();
	

	pthread_t gradStudent1;
	pthread_t gradStudent2;
	pthread_t gradStudent3;
	pthread_t gradStudent4;
	
	struct gradArguments grad1Args;
	grad1Args.moveArray = grad1;
	grad1Args.numMoves = grad1Steps;
	grad1Args.id = 1;

	struct gradArguments grad2Args;
	grad2Args.moveArray = grad2;
	grad2Args.numMoves = grad2Steps;
	grad2Args.id = 2;
	
	struct gradArguments grad3Args;
	grad3Args.moveArray = grad3;
	grad3Args.numMoves = grad3Steps;
	grad3Args.id = 3;

	struct gradArguments grad4Args;
	grad4Args.moveArray = grad4;
	grad4Args.numMoves = grad4Steps;
	grad4Args.id = 4;	

	//pthread_create(&archie_dog, NULL, dog, "Archie");
	pthread_create(&gradStudent1, NULL, gradStudent, &grad1Args);
	pthread_create(&gradStudent2, NULL, gradStudent, &grad2Args);
	pthread_create(&gradStudent3, NULL, gradStudent, &grad3Args);
	pthread_create(&gradStudent4, NULL, gradStudent, &grad4Args);


	while(1){};
}



