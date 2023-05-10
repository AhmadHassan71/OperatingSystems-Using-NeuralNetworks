#include "neuralNetwork.h"
#include <semaphore.h>

using namespace std;

NeuralNetwork n;
int threadCount = 0;
int allPassed = 0;

sem_t s;

pthread_mutex_t lock;

void * processThread(void * args)
{

    pthread_mutex_lock(&lock);
    cout << "thread called with id: " << ++threadCount << endl;


    NeuralNetwork neural = *(NeuralNetwork*) args;
    //cout << "thread received currentLayer: " << neural.currentLayer << endl;

    int numOfneurons=0;
    int currentLayer=neural.currentLayer;


    int outputNeurons=0;
    float* input;
    float* output;

    //defining an input size to read
    if(currentLayer==0)
    {   numOfneurons=neural.neurons_initial;
        outputNeurons=neural.neurons_hidden;
        output= new float[outputNeurons];
        input= new float [numOfneurons];
    }

    else if(currentLayer>0 && currentLayer != n.numOflayers-1)
    {
        numOfneurons= neural.neurons_hidden;
        input= new float[numOfneurons];
        output= new float[numOfneurons];
    }
    else if(currentLayer == n.numOflayers-1){
        numOfneurons= neural.neurons_hidden;
        input= new float[numOfneurons];
        output= new float;
    }

    cout << "\nreading inputPipe["<<currentLayer<<"][0]\n" << endl;
    read(neural.inputPipe[currentLayer][0],input,sizeof(float)*neural.layers[currentLayer].numOfNeurons);

    //cout << "input = " << input[0] <<","<< input[1] << input[2] << endl;

    cout<<"input recieved through pipe = ";
    for(int i=0;i<numOfneurons;i++)
    {
        cout << input[i]<< ",";
    }

    cout <<endl;


    //input layer calculations

    if(currentLayer==0)
    {

        // Calculate sum for each neuron in the input layer
        for (int i = 0; i <neural.layers[1].numOfNeurons; i++) 
        {
            float sum = 0.0;

            // Iterate over the inputs from the previous layer
            for (int j = 0; j < neural.layers[0].numOfNeurons; j++) 
            {
                // Multiply the input value with the corresponding weight from the input layer
                    sum += input[j] * neural.layers[0].weights[j][i];
                   //cout << "sum + = " << input[j] << " * " << neural.layers[0].weights[j][i] << "= " << sum <<endl;
            }
            //cout << sum << endl;
            output[i]= sum;
            //cout <<output[i] <<",";

        // Append the resulting sum to the outputs vector
       // cout << "sum = " << sum << endl;
        }

        cout <<endl;

        neural.layers[currentLayer].pass=true;
    }

    else if(currentLayer>0 &&  currentLayer != n.numOflayers-1)
    {

        // Calculate sum for each neuron in the input layer
        for (int i = 0; i <neural.layers[currentLayer].numOfNeurons; i++) 
        {
            float sum = 0.0;

            // Iterate over the inputs from the previous layer
            for (int j = 0; j < neural.layers[currentLayer].numOfNeurons; j++) 
            {
                // Multiply the input value with the corresponding weight from the input layer
                    sum += input[j] * neural.layers[currentLayer].weights[j][i];
                    //cout << "sum + = " << input[j] << " * " << neural.layers[currentLayer].weights[j][i] << "= " << sum <<endl;
            }
            output[i]= sum;
            //cout <<output <<",";

        // Append the resulting sum to the outputs vector
       // cout << "sum = " << sum << endl;
        }

        neural.layers[currentLayer].pass=true;
    }
    else if(currentLayer == n.numOflayers-1){
        
        float sum = 0.0;

        // Iterate over the inputs from the previous layer
        for (int j = 0; j < neural.layers[currentLayer].numOfNeurons; j++) 
        {
            // Multiply the input value with the corresponding weight from the input layer
                sum += input[j] * neural.layers[currentLayer].weights[0][j];
                //cout << "sum + = " << input[j] << " * " << neural.layers[currentLayer].weights[0][j] << "= " << sum <<endl;
        }
        output[0]= sum;
        //cout <<output <<",";

       

        neural.layers[currentLayer].pass=true;
    }

    cout<<"output to send through pipe = ";
    if(currentLayer > 0 && currentLayer != n.numOflayers-1){
        for(int i=0;i<neural.neurons_hidden;i++)
        {
            cout << output[i]<< ",";
        }
        cout <<endl;
    }
    else if(currentLayer == n.numOflayers-1){
        cout << output[0];
    }
    cout << endl;
    

    if (currentLayer + 1 < neural.numOflayers) 
    {
        n.currentLayer++;
        cout << "currentLayer updated Val: " << n.currentLayer << endl;
        cout << "\nwriting inputPipe["<<n.currentLayer<<"][1]\n" << endl;
        write(neural.inputPipe[n.currentLayer][1], output, sizeof(float)*neural.neurons_hidden);
    }
    

    //hidden layer calculation
    cout << "Thread with id: " << threadCount << " Exiting" << endl;
    allPassed++;
    sem_post(&s);
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);

}


int main(){

    int layers;
    cout << "enter how many layers: ";
    cin >> layers;

    sem_init(&s, 0, layers);

    float input[2]= {0.1,0.2};

    n.initialize(layers,2,8,8);
    n.readInputs();
    //n.displayfilesData(layers);


    //initializing
    int layerIndex=0;

    cout << "\nwriting inputPipe[0][1]\n" << endl;
    write(n.inputPipe[0][1],&input,sizeof(input));

    pthread_mutex_init(&lock,NULL);

    pthread_t pid[layers];

    for(int i=0;i<layers;i++)
        pthread_create(&pid[i],NULL,processThread,(void*)&n);
    

    for(int i=0;i<layers;i++)
        pthread_join(pid[i],NULL);
    

    for(int i = 0; i < n.numOflayers; i++)
        sem_wait(&s);
    
    pthread_mutex_destroy(&lock);

    return 0;
}