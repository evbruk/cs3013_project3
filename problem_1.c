#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define NUM_DOGS 9
#define NUM_CATS 13

void *dog(void * vargp)
{
	char * dogName = (char*) vargp;
	printf("Dog %s created! \n", dogName);

}

void *cat(void * vargp)
{
	char * catName = (char*) vargp;
	printf("Cat %s created! \n", catName);
	int sleepTime = ((rand() % 250) + 50);
	sleepTime = sleepTime * 1000;     				//converts from microseconds to milliseconds
	print("[%s] Waiting for %d milliseconds before asking to enter the kitchen \n", catName, sleepTime);
	usleep(sleepTime);
	printf("[%s] Asking to enter the kitchen... \n" catName);
	int permissionGranted = requestEntry(CAT_TYPE);
		if ( permissionGranted )
		{
			pthread_mutex_lock(&kitchen_lock);

			pets_in_kitchen[petsInKitchen] = catName;
			petsInKitchen++;

			pthread_mutex_unlock(&kitchen_lock);
			//enter the kitchen
			
		}

}

int main(int argc, char * argv[])
{
	printf("Creating %d dogs & %d cats... \n", NUM_DOGS, NUM_CATS);

	srand(time(0));

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
	


	while(1){}
}

