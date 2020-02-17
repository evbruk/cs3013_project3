#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define NUM_DOGS 9
#define NUM_CATS 13

int main(int argc, char * argv[])
{
	printf("Creating %d dogs & %d cats... \n", NUM_DOGS, NUM_CATS);
	srand(time(0));
	//cats	
	pthread_t chico_cat;
	pthread_t mittens_cat;
	pthread_t pickles_cat;
	pthread_t cake_cate;
	
	//dogs
	


}

void dog(char * dogName)
{
	printf("Dog %s created! \n", dogName);

}

void cat(char * catName)
{
	printf("Cat %s created! \n", catName);

}
