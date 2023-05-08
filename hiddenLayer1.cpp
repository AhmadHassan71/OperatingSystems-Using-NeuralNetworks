#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>
#include <sstream>

using namespace std;

#define BUFF_SIZE 100

//int hiddenLayerCOUNT = 0;

// pipe1 for input, pipe2 for hidden layer, pipe3 for calculation layer, pipe4 for output layer
//string _pipe1 = "pipe1";
string _pipe2 = "pipe2"; 
//string _pipe3 = "pipe3";
//string _pipe4 = "pipe4";

sem_t s2;        // s1 for input, s2 for hidden layer, s3 for calculation layer, s4 for output layer
pthread_mutex_t m1, m2;

void* hiddenLayer(void* args){
    //hiddenLayerCOUNT++;

    cout << "Hidden Layer Task doing" << endl;
    // Reading data from pipe 2
    float Hidden_Weights[8][8];
    int fd;
    fd = open(_pipe2.c_str(),O_RDONLY);
    read(fd, Hidden_Weights, sizeof(Hidden_Weights));
    close(fd);

    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
    cout << "Entered hidden layer" << ++*(argv[1]) << endl;

    // create semaphores

    // create pipes
    //mkfifo(_pipe1.c_str(), 0666);
    //mkfifo(_pipe2.c_str(), 0666);

    pthread_t h;

    // set thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // set thread scheduling policy
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // create thread
    pthread_create(&h, &attr, hiddenLayer, NULL);

    // wait for threads to finish
    sem_post(&s2);

    cout << "Program reached at the end of hidden Layer" << endl;
    //unlink(_pipe1.c_str());
    //unlink(_pipe2.c_str());
    return *argv[1];
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
