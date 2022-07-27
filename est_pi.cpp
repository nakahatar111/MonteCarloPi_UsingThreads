#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <math.h> 
#include <ctype.h>
#include <cassert>
using namespace std;
using namespace std::chrono;
int sum = 0;
pthread_mutex_t mutexsum;

void *est_pi(void* num){
  int count = 0;
  double randX;
  double randY;
  unsigned int* seed = (unsigned int*)pthread_self();
  
  for(long i = 0; i < (long)num; i++){
    randX = (double)rand_r(seed)/(RAND_MAX - 1);
    randY = (double)rand_r(seed)/(RAND_MAX - 1);
    if(sqrt(pow((randX - 0.5),2) + pow((randY - 0.5),2)) < 0.5)
      count++;
  }

  assert(pthread_mutex_lock (&mutexsum)==0);
  sum += count;
  assert(pthread_mutex_unlock (&mutexsum)==0);
  pthread_exit(NULL);
}

int main (int argc, char *argv[]){
  if(argc == 2 && isdigit(*argv[1])){
    assert(pthread_mutex_init(&mutexsum, NULL)==0);

    int numPoints = 100000;
    int NUMTHREADS = stoi(argv[1]);


    int pointsPerThread = numPoints / NUMTHREADS;
    int total = pointsPerThread * NUMTHREADS;
    auto start = high_resolution_clock::now();

    pthread_t arr_thread[NUMTHREADS];
  

    for(int i = 0; i < NUMTHREADS; i++){
      int err = pthread_create(&arr_thread[i], NULL, est_pi, (void*)(intptr_t)pointsPerThread);
      if(err){
        cout << "Error code "<< err << endl; 
        exit(-1);
      }
    }
    int retval;
    for (int t = 0; t < NUMTHREADS; t++){
      retval = pthread_join(arr_thread[t], NULL); // waits for all threads to finish
      if(retval != 0){
        switch (retval) {
            case EDEADLK:
                printf("Deadlock detected\n");
                break;
            case EINVAL:
                printf("The thread is not joinable\n");
                break;
            case ESRCH:
                printf("No thread with given ID is found\n");
                break;
            default:
                printf("Error occurred when joining the thread\n");
        }
      }

    }

    cout << "pi = " << ((double) sum / (long) total * 4) << endl;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time: " << duration.count() << " microseconds" << endl;

    assert(pthread_mutex_destroy(&mutexsum)==0);
    pthread_exit(NULL); //make sure no threads are still running before terminating
  }
  else{
    cout << "Argument is not an int" << endl;
  }
}