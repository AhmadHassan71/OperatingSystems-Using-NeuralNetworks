#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUFF_SIZE 100

int pipe1[2],pipe2[2],pipe3[2],pipe4[2];
sem_t s1,s2,s3;
pthread_mutex_t m1,m2;

void* readInput(void* args){
    float input_arr[2];
    std::cout<<"Enter Weight 1 : ";
    std::cin>>input_arr[0];
    std::cout<<"Enter Weight 2 : ";
    std::cin>>input_arr[1];

}

/*

Creation of a neural network with 3 layers:


Input layer:
input  -> 2 weights
arr[2] -> weight1, weight2
pipe1  -> arr[2]
call hidden layer

Hidden layer:
    ForwardLayer
            -->Forward propogation
    read from pipe1
    create Matrix
    pipe2( use between ForwardLayer and CalcculationLayer)

    CalcculationLayer:
            -->Backpropagation:
    {0,1,2,3,4}-> exec -> process -> mat[0]->thread mat[1]->thread...
    (communication between files through pipes)

    pipe3(use between CalcculationLayer and ReturnResultLayer)
    ReturnResultLayer:
        calculated gradients -> f(x) -> send to output layer

    pipe4(use between ReturnResultLayer and OutputLayer)

Output layer:
        weights -> output

*/

int main()
{
    pipe(pipe1);
    pthread_t readThread,t2,t3,t4;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    const struct sched_param param = {1};
    pthread_setschedparam(readThread,SCHED_FIFO,&param);
    pthread_create(&readThread,NULL,readInput,NULL);
    return 0;
}