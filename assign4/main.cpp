/***************************************************************
* Assignemnt 4 - Reader-Writer Problem 
*
* Programmer: Tristen Anderson
* ZID: z1820036
*
* Use: simiulates the reader writer problem by popping the end off
* of a shared string for some number of iterations for each writer
* while the reader prints its contents  
***************************************************************/
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>

//function declarations
void *writer(void *param);
void *reader(void *param);

//shared string for readers and writers
std::string word = "All work no play makes Jack a dull boy.";
//shared readcount data, how many are currently readinga
int readCount = 0;
int numIterations = 43;
//create semaphores
sem_t rw_sem;
sem_t cs_sem;

int main(int argc, char *argv[]) {

	//set default values for the command line parameters
	int numReaders = 1;
	int numWriters = 1;
	//int numIterations = 43;
	
	//for each argument supplied, put to corresponding var (note this assumes the user knows the order as per assignment guidelines)
	if(argc > 1)
		numReaders = std::stoi(argv[1]);
	if(argc > 2)
		numWriters = std::stoi(argv[2]);
	if(argc > 3)
		numIterations = std::stoi(argv[3]);

	//initialize semaphores to 1
	sem_init(&rw_sem, 0, 1);
	sem_init(&cs_sem, 0, 1);	

	printf("*** Reader-Writer Problem Simulation ***\n");
	//create pthreads
	pthread_t r[numReaders],w[numWriters];
	//create a number of reader threads
	for(long i = 0; i < numReaders; i++) {
		int rc = pthread_create(&r[i], NULL, reader, (void *) i);
		if(rc) {
			printf("ERROR: return code from pthread_create: %d\n", rc);
			exit(-1);
		}
	}
	//create a number of writer threads
	for(long i = 0; i < numWriters; i++) {
		int rc = pthread_create(&w[i], NULL, writer, (void *) i);
		if(rc) {
			printf("ERROR: return code from pthread_create: %d\n", rc);
			exit(-1);
		}
	}

	//wait for threads to finish
	for(int i = 0; i < numReaders; i++) {
		int rc = pthread_join(r[i], NULL);
		if(rc) {
			printf("ERROR: return code from pthread_join: %d\n", rc);
			exit(-1);
		}
	}
	for(int i = 0; i < numWriters; i++) {
		int rc = pthread_join(w[i], NULL);
		if(rc) {
			printf("ERROR: return code from pthread_join: %d\n", rc);
			exit(-1);
		}
	}

	printf("All threads are done.\n");
	//cleanup by destroying semaphores
	if(sem_destroy(&cs_sem) < 0) {
		printf("ERROR: problem destroying semaphore\n");
		exit(-1);
	}
	if(sem_destroy(&rw_sem) < 0) {
		printf("ERROR: problem destroying semaphore\n");
		exit(-1);
	}
	printf("Resources cleaned up\n");

	return 0;
}

/***************************************************************
Function: writer

Use: writes to the shared memory for numIterations if the string 
	is not empty

Arguments: param, holds the id of the writer

Returns: nothing
***************************************************************/
void *writer(void *param) {

	//writing is performed
	for(int i = 0; i < numIterations; i++) {
		sem_wait(&rw_sem);
		printf("Writer %d is writing... \n", param);
		if(!word.empty()) 
			word.pop_back();
		sem_post(&rw_sem);
		sleep(1);
	}
	pthread_exit(NULL);
}

/***************************************************************
Function: reader

Use: Reads from shared memory for numIterations and prints its contents 

Arguments: param, holds the id of the reader

Returns: nothing
***************************************************************/
void *reader(void *param) {

	int sval;
	for(int i = 0; i < numIterations; i++) {
		sem_wait(&cs_sem);
		readCount++;
		printf("read_count increments to: %d\n", readCount);
		if(readCount == 1)
			sem_wait(&rw_sem);
		sem_post(&cs_sem);
		
		printf("Reader %d is reading ... content : %s\n", param, word.c_str());	

		sem_wait(&cs_sem);
		readCount--;
		printf("read_count decrements to: %d\n", readCount);
		if(readCount == 0) {
			sem_getvalue(&rw_sem, &sval);
			printf("Last reader. Semaphore value before signaling writer: %d\n", sval);
			sem_post(&rw_sem);
			sem_getvalue(&rw_sem, &sval);
			printf("Last reader. Semaphore value after signaling writer: %d\n", sval);
		}
		sem_post(&cs_sem);
		sleep(1);
	}
	pthread_exit(NULL);
}
