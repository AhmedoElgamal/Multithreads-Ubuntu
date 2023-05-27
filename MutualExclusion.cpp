	#include <bits/stdc++.h>
	#include <semaphore.h>
	using namespace std;
	
	#define N 7 //Number of Threads for Counter
	
	int count = 0; //Initializing integer for count number
	int bufferCounter = 0; //Initializing integer to know the current value in buffer
	const int sizeOfBuffer = 10; //Initializing Max Size of Buffer
	
	sem_t c, s, n, e; //Declaring Semaphores
	
	int buffer[sizeOfBuffer]; //Declaring Buffer Array
	
	
	void* counter(void* args) //Counter Function
	{
		while(true)
		{
			int cValue; //Declaring variable to know value of semaphore c
			sem_getvalue(&c,&cValue); //Putting value of the semaphore in that variable
			
			printf("Counter Thread %ld: Received a Message\n",pthread_self());
			
			if(cValue <= 0) //if semaphore c of counter is less than or equal zero we wait
        			printf("Counter Thread %ld: Waiting to Write\n",pthread_self());
			
			int random; //Declaring random number
			random = rand() % 10;
			
			sem_wait(&c); //SemWait on Semaphore c
			::count++;
			
			printf("Counter Thread %ld: Adding to Counter, Count = %d\n",pthread_self(), ::count);
			
			sem_post(&c); //SemSignal on Semaphore c
		
			sleep(random); //Sleep with the random number we got
		}
	}
	
	void* monitor(void* args) //Monitor Function
	{
		while(true)
		{	
			int random; //Declaring random number
			random = rand() % 10;
			
			int tempCount; //Declaring Temp Variable for the value of count
			
			int cValue; //Declaring variable to know value of semaphore c
			sem_getvalue(&c,&cValue); //Putting value of the semaphore in that variable
			
			if(cValue <= 0) //if semaphore c of counter is less than or equal zero we wait
        			printf("Monitor Thread: Waiting to Read Counter\n");
        			
			sem_wait(&c); //SemWait on Semaphore c
			tempCount = ::count; //putting the count value in the tempCount variable
			::count = 0; //then resetting count to zero
			
			printf("Monitor Thread: Reading Count = %d\n", tempCount);
			
			sem_post(&c); //SemSignal on Semaphore c
			
			if (::bufferCounter == sizeOfBuffer) //If Buffer is Full
			{
			    printf("Monitor Thread: Buffer is Full\n");
			}
			
			sem_wait(&e); //SemWait on Semaphore e to wait if there is an empty spot in buffer
			sem_wait(&s); //SemWait on Semaphore s to Enter critical section
			
			buffer[::bufferCounter] = tempCount; //Putting tempCount value in array
			::bufferCounter++; //Then incrementing the value position in array
			
			printf("Monitor Thread: Writing to Buffer\n");
			
			sem_post(&s); //SemSignal on Semaphore s to Exit critical section
			sem_post(&n); //SemSignal on semaphore n to signal that a value was added to the buffer
		
			sleep(random); //Sleep with the random number we got
		}
	}
	
	void* collector(void* args) //Collector Function
	{
		while(true)
		{
			if (bufferCounter == 0) //If Buffer is Empty
			{
			    printf("Collector Thread: Nothing is in the Buffer\n");
			}
			
			int random; //Declaring random number
			random = rand() % 10;
			
			sem_wait(&n); //SemWait on semaphore n to check if buffer has items to consume
			sem_wait(&s); //SemWait on Semaphore s to Enter critical section
			
			buffer[::bufferCounter-1] = 0; //Taking Last Element in Buffer similar to LIFO Stack
			::bufferCounter--;
			
			printf("Collector Thread: Reading from Buffer\n");
			
			sem_post(&s); //SemSignal on Semaphore s to Exit critical section
			sem_post(&e); //SemSignal on Semaphore e to signal that there is an empty spot in buffer
			
			sleep(random); //Sleep with the random number we got
		}
	}

	
	int main()
	{	
		srand(time(NULL)); //Used to Initialize Random Numbers
		int i;
		pthread_t th1; //Declaring Threads
		pthread_t th2;
		pthread_t th3[N]; //Declaring Thread Array of size N
		
		sem_init(&c, 0, 1); //Initializing Semaphore c for Counter
		sem_init(&s, 0, 1); //Initializing Semaphore s for Mutual Exclusion
		sem_init(&n, 0, 0); //Initializing Semaphore n to check if Buffer is Empty or Not
		sem_init(&e, 0, sizeOfBuffer); //Initializing Semaphore e to check Number of Empty Spaces in Buffer
		
		pthread_create(&th1, NULL, collector, NULL); //Create a thread for Collector Function
		pthread_create(&th2, NULL, monitor, NULL); //Create a thread for Monitor Function
		
		for (i=0; i<N; i++) //Loop to create threads for Counter Function
		{
			pthread_create(&th3[i], NULL, counter, NULL);
		}
		
		pthread_join(th1, NULL); //Join allows application to wait for thread to terminate
		pthread_join(th2, NULL);
		
		for (i=0; i<N; i++)
		{
			pthread_join(th3[i], NULL);
		}
	}
	
	
