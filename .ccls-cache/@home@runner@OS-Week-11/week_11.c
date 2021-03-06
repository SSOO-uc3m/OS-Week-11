#include "week_11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>



/*******************************************************
Exercise 01

Given the following code, it is requested to implement the odd and even functions so that one thread prints the even numbers from 0 to 199 and the other, the odd numbers from 0 to 199 consecutively. 

An example of the output would be the following:

We want the execution to provide the following output on screen:

********************************************************/

int shared_data = 0;
bool even = false;

pthread_mutex_t mutex;
pthread_cond_t waitEven;
pthread_cond_t waitOdd;

void exercise01(){
  
  pthread_t th1, th2;
    
  pthread_mutex_init (&mutex, NULL);
  pthread_cond_init (&waitEven, NULL);
  pthread_cond_init (&waitOdd, NULL);
  
  pthread_create(&th1,NULL,oddThread,NULL);
  pthread_create(&th2,NULL,evenThread,NULL);
	
  // I wait for the completion of the thread
    
  pthread_join(th1,NULL);
  pthread_join(th2,NULL);
   
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&waitEven);
  pthread_cond_destroy(&waitOdd);

  
}
   

void *evenThread(void *arg) {   

}

void *oddThread(void *arg) {

}

/*******************************************************
Exercise 02

A student has provided us with the following code, in which different threads are created from the main process that execute the print () function:

********************************************************/

#define N 3

pthread_t thread1, thread2; 
pthread_attr_t attr; /* threads atributes*/
  
pthread_mutex_t printer=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t printHello;
pthread_cond_t printWorld;
  
bool hello = true;

void exercise02 () {
    pthread_cond_init(&printHello, NULL);
    pthread_cond_init(&printWorld, NULL);
    char stringHello[]="Hello ";
    char StringWorld[]="world \n";
  
    pthread_attr_init (&attr);
  
    for (int i=1; i<=N; i++) {
        pthread_create(&thread2, &attr, print, (void *)StringWorld);
        pthread_create(&thread1, &attr, print, (void *)stringHello);
    }
    pthread_exit (NULL);
}

void *print (void *arg) {
  
        char stringHello[] = "Hello ";
        char a[12];
        pthread_mutex_lock (&printer);
        strcpy(a, (char*)arg);
        if (strncmp(a,stringHello,5)==0) {
          while (!hello) {                          
                    pthread_cond_wait(&printHello,&printer);
          }
          printf("%s", a);
          hello = false;
          pthread_cond_signal(&printWorld);
        } 
        else {
                while (hello){
                    pthread_cond_wait(&printWorld,&printer);
                }
                printf("%s", a);
                hello = true;
                pthread_cond_signal(&printHello);
        }
        pthread_mutex_unlock (&printer);
        pthread_exit (NULL);
}

/*******************************************************

Exercise 03


********************************************************/
int isWriting = 0;
int isReading = 0;
int data;

pthread_mutex_t myMutex;
pthread_cond_t reading;
pthread_cond_t writing;

void * reader(void *p) {
    long i=(long)p;
    while(1) {
        pthread_mutex_lock(&mutex);
        if(isWriting)
            pthread_cond_wait(&reading,&myMutex);
        isReading++;
        pthread_mutex_unlock(&mutex);

        printf("Reader %ld: Data = %d\n", i, data);
        sleep(1); //takes 1 sec to read

        pthread_mutex_lock(&mutex);
        isReading--;
        if(!isReading)
            pthread_cond_signal(&reading);
        pthread_mutex_unlock(&myMutex);
    }
}


void * writer(void * p) {
   long i=(long)p;
   while(1)   {
        pthread_mutex_lock(&mutex);
        while(isReading||isWriting)
            pthread_cond_wait(&writing,&mutex);
        isWriting++;
        pthread_mutex_unlock(&mutex);
        sleep(1); //tardo 1 sg en escribir
        data+=2;
        printf("writer %ld: Dato = %d\n", i, data);
        pthread_mutex_lock(&mutex);
        isWriting--;
        pthread_cond_broadcast(&writing);
        pthread_cond_broadcast(&reading);
        pthread_mutex_unlock(&mutex);
   }
}
/*******************************************************

Exercise 04


********************************************************/
#define N 10
#define SIZE 1024

int array[SIZE];

struct b_s {
  int n;
  pthread_mutex_t m;
  pthread_cond_t ll;
} b;


void exercise04(void) {
  pthread_t thread[N];
  int * number; 
  	
  b.n = 0;
  pthread_mutex_init(&b.m, NULL);
  pthread_cond_init(&b.ll, NULL);

  
  for(int i=0; i<N; i++){
    number = malloc (sizeof(int));
    *number = i;
    pthread_create(&thread[i],  NULL, worker,  (void *)number);
    }
  
  for(int i=0; i<N; i++)
    pthread_join(thread[i], NULL);

  pthread_cond_destroy(&b.ll);
  pthread_mutex_destroy(&b.m);
  
for(int i=0; i< SIZE; i++){
 

    printf("array[%d] = %d\n", i,array[i]);  


}

  
}

void *worker(void *arg) {

  int start=0, end=0;
  int * id;
  id  = (int *)arg; 
  start =(*id)*(SIZE/N);
  
  end = (*id+1)*(SIZE/N);
  
  pthread_mutex_lock(&b.m); 

   b.n++; 

  if (N<=b.n) {	
    pthread_cond_broadcast(&b.ll);
  } 
  else {
  	pthread_cond_wait(&b.ll, &b.m); 
  }
  
  for(int i=start; i<end; i++) {
  	array[i] = *id;
  }
  
   pthread_mutex_unlock(&b.m); 
 

  free(id);
}