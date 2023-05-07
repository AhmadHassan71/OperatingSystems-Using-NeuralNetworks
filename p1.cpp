#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUFF_SIZE 100

sem_t s1,s2,s3;
pthread_mutex_t m1,m2;

<<<<<<< Updated upstream
=======
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
    fd2 = open(_pipe2.c_str(), O_WRONLY);
    write(fd2, Hidden_Weights, sizeof(Hidden_Weights));
    close(fd2);
    sem_post(&s2);
}
>>>>>>> Stashed changes
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