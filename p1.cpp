#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUFF_SIZE 100

sem_t s1,s2,s3;
pthread_mutex_t m1,m2;

/*
input layer
*/