#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define NUM_DOGS 9
#define NUM_CATS 13
#define	DOG_TYPE 0
#define CAT_TYPE 1

pthread_mutex_t kitchen_lock;
int petsInKitchen;
int kitchenOwner;
char * pets_in_kitchen[13];
int numVisited = 0;
pthread_cond_t kitchenSwitch = PTHREAD_COND_INITIALIZER;

int request_entry(int animalType);
int bowlsAvailable = 2;

//maybe use this to "prove" fairness?
struct animalArgs {
	char * animalName;
	int timesDrank;
};

struct animalArgs animalArgsArray[13+9];


int print_kitchen()
{
	for(int i = 0; i < petsInKitchen; i++)
	{
		printf("%s \n", pets_in_kitchen[i]);	
	}
}

void *dog(void * vargp)
{
	struct animalArgs * arguments = (struct animalArgs *) vargp;
	char * dogName = arguments->animalName;
	printf("Dog %s created! \n", dogName);
	int sleepTime = (rand() % 250) + 50;
	printf("[%s] Waiting for %d milliseconds before asking to enter the kitchen \n", dogName, sleepTime);
	sleepTime *= 1000;
	
	usleep(sleepTime);
	
	//this could potentially lock up the thread.
	
	while(1)
	{
		printf("[%s] Asking to enter the kitchen... \n", dogName);
		pthread_mutex_lock(&kitchen_lock);
		if( request_entry(DOG_TYPE) )
		{
			printf("[%s] Allowed to enter the kitchen! \n", dogName);
			numVisited++; 
			if (bowlsAvailable > 0)
			{
				printf("[%s] is drinking... \n", dogName);
				//do some drinking
				bowlsAvailable--;		
				pthread_mutex_unlock(&kitchen_lock);
		
				usleep(10000);

				pthread_mutex_lock(&kitchen_lock);
				arguments->timesDrank++;
				bowlsAvailable++;
				printf("[%s] %d dogs have drank \n", dogName, numVisited);
				//the logic for "leaving" should go here.
				if( numVisited >= 2)
				{
					//we're done and we should signal the condition variable.
					printf("Cats now rule the kitchen. \n");
					kitchenOwner = CAT_TYPE;
					numVisited = 0;
					pthread_cond_broadcast(&kitchenSwitch);	
				}
				pthread_mutex_unlock(&kitchen_lock);
			}
		}else
		{
		//cats are drinking, wait on a switching condition variable.
			printf("[%s] Waiting on kitchen switch... \n", dogName);
			while( !request_entry(DOG_TYPE) )
			{
				//printf("[%s] Waiting on kitchen switch... \n", catName);
				pthread_cond_wait(&kitchenSwitch, &kitchen_lock);		
			}
		
		}
		pthread_mutex_unlock(&kitchen_lock);
		int sleepTime = (rand() % 4) + 1;
		printf("[%s] sleeping for %d seconds...\n", dogName, sleepTime);
		sleep(sleepTime);
	}
}

void *cat(void * vargp)
{
	struct animalArgs * arguments = (struct animalArgs *) vargp;
	char * catName = arguments->animalName;
	printf("Cat %s created! \n", catName);
	int sleepTime = ((rand() % 250) + 50);
	printf("[%s] Waiting for %d milliseconds before asking to enter the kitchen \n", catName, sleepTime);	
	sleepTime = sleepTime * 1000;     				//converts from microseconds to milliseconds
	
	usleep(sleepTime);

	while(1)
	{
		printf("[%s] Asking to enter the kitchen... \n", catName);		
		pthread_mutex_lock(&kitchen_lock);
		if (request_entry(CAT_TYPE))
		{
			printf("[%s] is allowed to enter the kitchen ...\n", catName);
			numVisited++;
			if (bowlsAvailable > 0)
			{
				printf("[%s] is drinking ...\n", catName);
				//do some drinking
				bowlsAvailable--;
				pthread_mutex_unlock(&kitchen_lock);

				usleep(10000);

				pthread_mutex_lock(&kitchen_lock);
				arguments->timesDrank++;
				bowlsAvailable++;
				printf("[%s] %d cats have drank \n", catName, numVisited);
	
				if ( numVisited >= 2) 
				{
					//cats are gone and dogs can drink
					kitchenOwner = DOG_TYPE;
					numVisited = 0;
					printf("Dogs now rule the kitchen \n");
					pthread_cond_broadcast(&kitchenSwitch);
				}
				pthread_mutex_unlock(&kitchen_lock);				
			}
	
		}else
		{
			printf("[%s] Waiting on kitchen switch... \n", catName);
			while (!request_entry(CAT_TYPE))
			{
				pthread_cond_wait(&kitchenSwitch, &kitchen_lock);
			}
		}
 		pthread_mutex_unlock(&kitchen_lock);
		int sleepTime = (rand() % 4) + 1;
		printf("[%s] sleeping for %d seconds...\n", catName, sleepTime);
		sleep(sleepTime);
	}
}

int request_entry(int animalType)
{
	//also check for maximum number of pets allowed, if it is met or exceeded, signal condition variable on process_kitchen()
	if(kitchenOwner == -1)
	{
		kitchenOwner = animalType;	
	}
	if(animalType == kitchenOwner)
	{
		//grant access
		return 1;	
	}else
	{
		//deny access
		return 0;	
	}	
}


void *process_kitchen()
{
	pthread_mutex_lock(&kitchen_lock);
	if(petsInKitchen > 6)
	{
		printf("************ Animals in kitchen: ***********\n");
		print_kitchen();

		for(int i = 0; i < petsInKitchen; i++)
		{
			pets_in_kitchen[i] = "";	
		}
		petsInKitchen = 0;
	
		if (kitchenOwner == CAT_TYPE)
		{
			kitchenOwner = DOG_TYPE;	
		}else if(kitchenOwner == DOG_TYPE)
		{
			kitchenOwner = CAT_TYPE;	
		}
		pthread_cond_signal(&kitchenSwitch);
	
	}
	pthread_mutex_unlock(&kitchen_lock);		
}

void initAnimalArgs(char * name, int index)
{
	animalArgsArray[index].animalName = name;
	animalArgsArray[index].timesDrank = 0;
}

void sigint_handler(int sig)
{
	printf("Cats Drinking Stats: \n");
	for(int i = 0; i < 13; i++)
	{
		printf("%s: %d \n", animalArgsArray[i].animalName, animalArgsArray[i].timesDrank);	
	}
	printf("-------------------------\nDogs Drinking Stats: \n");
	for(int i = 13; i < (13+9); i++)
	{
		printf("%s: %d \n", animalArgsArray[i].animalName, animalArgsArray[i].timesDrank);
	}
	exit(0);
}

int main(int argc, char * argv[])
{
	int result = pthread_mutex_init(&kitchen_lock, NULL);
	petsInKitchen = 0;
	kitchenOwner = -1;
	signal(SIGINT, sigint_handler);

	if(result != 0)
	{
		printf("problem creating the mutex! \n");	
		return -1;	
	}	
	printf("Creating %d dogs & %d cats... \n", NUM_DOGS, NUM_CATS);

	srand(time(0));

	pthread_t waterbowl_thread;

	//cats	
	pthread_t chico_cat;
	pthread_t mittens_cat;
	pthread_t pickles_cat;
	pthread_t cake_cat;
	pthread_t cheeks_cat;
	pthread_t mozz_cat;
	pthread_t soda_cat;
	pthread_t kitten_cat;
	pthread_t luna_cat;
	pthread_t violet_cat;
	pthread_t honeymust_cat;
	pthread_t buster_cat;
	pthread_t munchkin_cat;

	//create cats
	initAnimalArgs("Chico", 0);
	initAnimalArgs("Mittens", 1);
	initAnimalArgs("Pickles", 2);
	initAnimalArgs("Cake", 3);
	initAnimalArgs("Cheeks", 4);
	initAnimalArgs("Mozz", 5);
	initAnimalArgs("Soda", 6);
	initAnimalArgs("Kitten", 7);
	initAnimalArgs("Luna", 8);
	initAnimalArgs("Violet", 9);
	initAnimalArgs("Honeymust", 10);
	initAnimalArgs("Buster", 11);
	initAnimalArgs("Munchkin", 12);
	
	pthread_create(&chico_cat, NULL, cat, &animalArgsArray[0]);
	pthread_create(&mittens_cat, NULL, cat, &animalArgsArray[1]);
	pthread_create(&pickles_cat, NULL, cat, &animalArgsArray[2]);
	pthread_create(&cake_cat, NULL, cat, &animalArgsArray[3]);
	pthread_create(&cheeks_cat, NULL, cat, &animalArgsArray[4]);
	pthread_create(&mozz_cat, NULL, cat, &animalArgsArray[5]);
	pthread_create(&soda_cat, NULL, cat, &animalArgsArray[6]);
	pthread_create(&kitten_cat, NULL, cat, &animalArgsArray[7]);
	pthread_create(&luna_cat, NULL, cat, &animalArgsArray[8]);
	pthread_create(&violet_cat, NULL, cat, &animalArgsArray[9]);
	pthread_create(&honeymust_cat, NULL, cat, &animalArgsArray[10]);
	pthread_create(&buster_cat, NULL, cat, &animalArgsArray[11]);
	pthread_create(&munchkin_cat, NULL, cat, &animalArgsArray[12]);
	
	//dogs
	pthread_t archie_dog;
	pthread_t toby_dog;
	pthread_t brady_dog;
	pthread_t tom_dog;
	pthread_t buddy_dog;
	pthread_t princess_dog;
	pthread_t rosie_dog;
	pthread_t yeller_dog;
	pthread_t brandy_dog;	

	initAnimalArgs("Archie", 13);
	initAnimalArgs("Toby", 14);
	initAnimalArgs("Brady", 15);
	initAnimalArgs("Tom", 16);
	initAnimalArgs("Buddy", 17);
	initAnimalArgs("Princess", 18);
	initAnimalArgs("Rosie", 19);
	initAnimalArgs("Yeller", 20);
	initAnimalArgs("Brandy", 21);
	//create dogs
	pthread_create(&archie_dog, NULL, dog, &animalArgsArray[13]);
	pthread_create(&toby_dog, NULL, dog, &animalArgsArray[14]);
	pthread_create(&brady_dog, NULL, dog, &animalArgsArray[15]);
	pthread_create(&tom_dog, NULL, dog, &animalArgsArray[16]);
	pthread_create(&buddy_dog, NULL, dog, &animalArgsArray[17]);
	pthread_create(&princess_dog, NULL, dog, &animalArgsArray[18]);
	pthread_create(&rosie_dog, NULL, dog, &animalArgsArray[19]);
	pthread_create(&yeller_dog, NULL, dog, &animalArgsArray[20]);
	pthread_create(&brandy_dog, NULL, dog, &animalArgsArray[21]);
	
	pthread_create(&waterbowl_thread, NULL, process_kitchen, NULL);
	


	while(1){}
}

