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

// pipe1 for input, pipe2 for hidden layer, pipe3 for calculation layer, pipe4 for output layer
string _pipe1 = "pipe1";
string _pipe2 = "pipe2"; 
string _pipe3 = "pipe3";
string _pipe4 = "pipe4";

sem_t s1, s2, s3, s4;        // s1 for input, s2 for hidden layer, s3 for calculation layer, s4 for output layer
pthread_mutex_t m1, m2;

void *readInput(void *args)
{
    float input_arr[2];
    // Read input from user
    cout << "Enter Weight 1 : ";
    cin >> input_arr[0];
    cout << "Enter Weight 2 : ";
    cin >> input_arr[1];

    // Write input to pipe1
    int fd=open(_pipe1.c_str(),O_WRONLY);
    write(fd, input_arr, sizeof(input_arr));
    close(fd);
    pthread_exit(NULL);
}

void *forwardLayer(void *args)
{
    float input_arr[2];
    char input_array[BUFF_SIZE];
    int fd=open(_pipe1.c_str(),O_RDONLY);
    // Read input from pipe1
    read(fd, input_arr, sizeof(input_arr));
    close(fd);
    cout << "\nInput array : "<< input_arr[0] << " " << input_arr[1] << endl;

    // Storing data to the Matrix from file
    string filename = "Hidden_Weights.txt";
    float Hidden_Weights[8][8];
    ifstream myFile(filename);
    string line;
    
    int TOTALROWS = 2;                        // Row size depends upon rows in file.
    int cols = 0;
    for(int a = 0; a < TOTALROWS; a++){
        getline(myFile, line);
        stringstream ss(line);

        string substr;
        while(getline(ss, substr, ',')){
            Hidden_Weights[a][cols++] = stof(substr);
        }
        cols = 0;
    }

    cout << "The read data: " <<endl;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 8; j++){
            cout << Hidden_Weights[i][j] << " ";
        }
        cout << endl;
    }


    // Write matrix to pipe2                                        // PROBLEM EXIST HERE WHILE STORING DATA TO PIPE 2
    // int fd2;
    // fd2=open(_pipe2.c_str(),O_WRONLY);
    // write(fd2, Hidden_Weights, sizeof(Hidden_Weights));
    // close(fd2);

    myFile.close();
    cout << "file closed successfully" << endl;

    sem_post(&s1);
    pthread_exit(NULL);
}

void* hiddenLayer(void* args){

    pthread_exit(NULL);
}


int main()
{

    // create semaphores
    sem_init(&s1, 0, 0);

    // create pipes
    mkfifo(_pipe1.c_str(), 0666);
    mkfifo(_pipe2.c_str(), 0666);

    pthread_t readThread, forwardThread, t3, t4;

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
    pthread_create(&readThread, &attr, readInput, NULL);
    pthread_create(&forwardThread, &attr2, forwardLayer, NULL);

    // wait for threads to finish
    sem_wait(&s1);

    cout << "Program reached at the end" << endl;
    unlink(_pipe1.c_str());
    unlink(_pipe2.c_str());
    return 0;
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
