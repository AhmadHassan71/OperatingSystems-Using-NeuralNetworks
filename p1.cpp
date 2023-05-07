#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUFF_SIZE 100

// pipe1 for input, pipe2 for hidden layer, pipe3 for calculation layer, pipe4 for output layer
std::string _pipe1 = "pipe1";
std::string _pipe2 = "pipe2";
std::string _pipe3 = "pipe3";
std::string _pipe4 = "pipe4";

sem_t s1, s2, s3, s4; // s1 for input, s2 for hidden layer, s3 for calculation layer, s4 for output layer
pthread_mutex_t m1, m2;
const int neurons = 8;
int input_weights;

void *readInput(void *args)
{
    float *input_arr;
    std::cout << "Enter number of input weights : ";
    std::cin >> input_weights;
    input_arr = new float[input_weights];

    for (int i = 0; i < input_weights; i++)
    {
        // Read input from user
        std::cout << "Enter Weight " << i << " : ";
        std::cin >> input_arr[i];
    }

    // Write input to pipe1
    int fd = open(_pipe1.c_str(), O_WRONLY);
    write(fd, input_arr, sizeof(input_arr));
    close(fd);
    pthread_exit(NULL);
}

void *inputLayer(void *args)
{
    float* input_arr = new float[input_weights];
    //char input_array[BUFF_SIZE];
    int fd = open(_pipe1.c_str(), O_RDONLY);
    // Read input from pipe1
    read(fd, input_arr, sizeof(input_arr));
    close(fd);
    std::cout << "\nInput array : " << input_arr[0] << " " << input_arr[1] << std::endl;

    // Create matrix
    // Reading format is [a b c d e f g h] without the brackets and
    // values are separated by spaces
    std::string filename = "Input_Weights.txt";
    float Hidden_Weights[input_weights][neurons];
    FILE *fp = fopen(filename.c_str(), "r");
    for (int i = 0; i < input_weights; i++)
    {
        for (int j = 0; j < neurons; j++)
        {
            fscanf(fp, "%f", &Hidden_Weights[i][j]);
        }
    }
    fclose(fp);

    // Write matrix to pipe2
    int fd2;
    fd2 = open("pipe2", O_WRONLY);
    write(fd2, Hidden_Weights, sizeof(Hidden_Weights));
    close(fd2);
    sem_post(&s1);
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

/*
    input layer
*/

int main()
{

    // create semaphores
    sem_init(&s1, 0, 0);

    // create pipes
    mkfifo(_pipe1.c_str(), 0666);
    mkfifo(_pipe2.c_str(), 0666);

    pthread_t readThread, inputThread, t3, t4;

    // set thread attributes
    pthread_attr_t attr, attr2;
    pthread_attr_init(&attr);
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);

    // set thread scheduling policy
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);

    // create threads
    pthread_create(&readThread, NULL, readInput, NULL);
    pthread_create(&inputThread, NULL, inputLayer, NULL);

    // wait for threads to finish
    sem_wait(&s1);
    sem_wait(&s2);
    unlink(_pipe1.c_str());
    unlink(_pipe2.c_str());
    return 0;
}
