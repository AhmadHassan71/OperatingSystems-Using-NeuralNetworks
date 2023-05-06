#include <iostream>
#include <string>
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
    // Read input from user
    std::cout<<"Enter Weight 1 : ";
    std::cin>>input_arr[0];
    std::cout<<"Enter Weight 2 : ";
    std::cin>>input_arr[1];

    // Write input to pipe1
    write(pipe1[1],input_arr,sizeof(input_arr));
    close(pipe1[1]);
    pthread_exit(NULL);
}

void* forwardLayer(void* args){
    float input_arr[2];
    // Read input from pipe1
    read(pipe1[0],input_arr,sizeof(input_arr));
    close(pipe1[0]);

    // Create matrix
    // Reading format is [a b c d e f g h] without the brackets and 
    // values are separated by spaces
    std::string filename="Hidden_Weights.txt";
    float Hidden_Weights[8][8];
    FILE* fp = fopen(filename.c_str(),"r");
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            fscanf(fp,"%f",&Hidden_Weights[i][j]);
        }
    }
    fclose(fp);

    // Write matrix to pipe2
    write(pipe2[1],Hidden_Weights,sizeof(Hidden_Weights));
    close(pipe2[1]);
    pthread_exit(NULL);
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

    CalculationLayer:
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

    //create semaphores
    sem_init(&s1,0,0);
    sem_init(&s2,0,0);

    //create pipes
    pipe(pipe1);
    pipe(pipe2);


    pthread_t readThread,forwardThread,t3,t4;

    //set thread attributes
    pthread_attr_t attr,attr2;
    pthread_attr_init(&attr);
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_DETACHED);

    //set thread priority
    const struct sched_param param = {1};
    pthread_setschedparam(readThread,SCHED_FIFO,&param);
    pthread_setschedparam(forwardThread,SCHED_FIFO,&param);

    //create threads
    pthread_create(&readThread,NULL,readInput,NULL);
    pthread_create(&forwardThread,NULL,forwardLayer,NULL);

    //wait for threads to finish
    sem_wait(&s1);
    sem_wait(&s2);
    return 0;
}