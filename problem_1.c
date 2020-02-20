#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

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


int print_kitchen()
{
	for(int i = 0; i < petsInKitchen; i++)
	{
		printf("%s \n", pets_in_kitchen[i]);	
	}
}

void *dog(void * vargp)
{
	char * dogName = (char*) vargp;
	printf("Dog %s created! \n", dogName);
	int sleepTime = (rand() % 250) + 50;
	sleepTime *= 1000;
	printf("[%s] Waiting for %d milliseconds before asking to enter the kitchen \n", dogName, sleepTime);
	usleep(sleepTime);
	printf("[%s] Asking to enter the kitchen... \n", dogName);
	//this could potentially lock up the thread.
	
	while(1)
	{
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
				bowlsAvailable++;
				printf("[%s] %d dogs have drank \n", dogName, numVisited);
				//the logic for "leaving" should go here.
				if( numVisited >= 2)
				{
					//we're done and we should signal the condition variable.
					printf("Cats now rule the kitchen. \n");
					kitchenOwner = CAT_TYPE;
					numVisited = 0;
					pthread_cond_signal(&kitchenSwitch);	
				}
				pthread_mutex_unlock(&kitchen_lock);
			}
		}else
		{
		//cats are drinking, wait on a switching condition variable.
			printf("[%s] Waiting on kitchen switch... \n", dogName);
			while( !request_entry(DOG_TYPE) )
				//printf("[%s] Waiting on kitchen switch... \n", catName);
				pthread_cond_wait(&kitchenSwitch, &kitchen_lock);		
		
		}
		pthread_mutex_unlock(&kitchen_lock);
		sleep(rand() % 5);
	}
}

void *cat(void * vargp)
{
	char * catName = (char*) vargp;
	printf("Cat %s created! \n", catName);
	int sleepTime = ((rand() % 250) + 50);
	sleepTime = sleepTime * 1000;     				//converts from microseconds to milliseconds
	printf("[%s] Waiting for %d milliseconds before asking to enter the kitchen \n", catName, sleepTime);
	usleep(sleepTime);
	printf("[%s] Asking to enter the kitchen... \n", catName);

	

	while(1)
	{
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
				bowlsAvailable++;
				printf("[%s] %d cats have drank \n", catName, numVisited);
	
				if ( numVisited >= 2) 
				{
					//cats are gone and dogs can drink
					kitchenOwner = DOG_TYPE;
					numVisited = 0;
					printf("Dogs now rule the kitchen \n");
					pthread_cond_signal(&kitchenSwitch);
				}
				pthread_mutex_unlock(&kitchen_lock);				
			}
	
		}else
		{
			while (!request_entry(CAT_TYPE))
			{
				pthread_cond_wait(&kitchenSwitch, &kitchen_lock);
			}
		}
 		pthread_mutex_unlock(&kitchen_lock);
		sleep(rand() % 5);
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

int main(int argc, char * argv[])
{
	int result = pthread_mutex_init(&kitchen_lock, NULL);
	petsInKitchen = 0;
	kitchenOwner = -1;
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
	pthread_create(&chico_cat, NULL, cat, "Chico");
	pthread_create(&mittens_cat, NULL, cat, "Mittens");
	pthread_create(&pickles_cat, NULL, cat, "Pickles");
	pthread_create(&cake_cat, NULL, cat, "Cake");
	pthread_create(&cheeks_cat, NULL, cat, "Cheeks");
	pthread_create(&mozz_cat, NULL, cat, "Mozz");
	pthread_create(&soda_cat, NULL, cat, "Soda");
	pthread_create(&kitten_cat, NULL, cat, "Kitten");
	pthread_create(&luna_cat, NULL, cat, "Luna");
	pthread_create(&violet_cat, NULL, cat, "Violet");
	pthread_create(&honeymust_cat, NULL, cat, "Honeymust");
	pthread_create(&buster_cat, NULL, cat, "Buster");
	pthread_create(&munchkin_cat, NULL, cat, "Munchkin");
	
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

	//create dogs
	pthread_create(&archie_dog, NULL, dog, "Archie");
	pthread_create(&toby_dog, NULL, dog, "Toby");
	pthread_create(&brady_dog, NULL, dog, "Brady");
	pthread_create(&tom_dog, NULL, dog, "Tom");
	pthread_create(&buddy_dog, NULL, dog, "Buddy");
	pthread_create(&princess_dog, NULL, dog, "Princess");
	pthread_create(&rosie_dog, NULL, dog, "Rosie");
	pthread_create(&yeller_dog, NULL, dog, "Yeller");
	pthread_create(&brandy_dog, NULL, dog, "Brandy");
	
	pthread_create(&waterbowl_thread, NULL, process_kitchen, NULL);
	


	while(1){}
}

