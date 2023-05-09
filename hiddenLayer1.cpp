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

sem_t s1;

string _pipe2 = "pipe2"; 

void* hiddenLayer(void* args){
    cout << "Hidden Layer Task doing" << endl;
    cout << "reading pipe 2" << endl;
    // Reading data from pipe 2
    float Hidden_Weights[8][8];
    int fd;
    fd = open(_pipe2.c_str(),O_RDONLY);
    read(fd, Hidden_Weights, sizeof(Hidden_Weights));
    close(fd);
    sem_post(&s1);

    pthread_exit(NULL);
}


int main(int argc, char* argv[])
{
    sem_init(&s1, 0, 0);
    cout << "Entered hidden layer" << ++*(argv[1]) << endl;

    pthread_t h;

    // set thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // set thread scheduling policy
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // create thread
    pthread_create(&h, &attr, hiddenLayer, NULL);

    sem_wait(&s1);
    // TEMP

    // // TEMP Writing data from pipe 2
    // float Hidden_Weights[8][8];
    // int fd;
    // fd = open(_pipe2.c_str(),O_RDONLY);
    // write(fd, Hidden_Weights, sizeof(Hidden_Weights));
    // close(fd);

    cout << "Program reached at the end of hidden Layer" << endl;

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
