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
#include <vector>

using namespace std;

struct Layer {
    float **weights;
    int numOfNeurons;
    bool pass=false;

    void initialize(int numNeurons) 
    {
        numOfNeurons=numNeurons;

        weights = new float * [numOfNeurons];

        for(int i=0;i<numOfNeurons;i++)
            weights[i]= new float [numOfNeurons];

        for (int i=0;i<numOfNeurons;i++)
            for(int j=0;j<numOfNeurons;j++)
                weights[i][j]=0;

    }

    
    ~Layer()
    {
        for(int i=0;i<numOfNeurons;i++)
            delete[] weights[i];

            delete[] weights;
            weights=NULL;
    }
}; 

struct NeuralNetwork {
    int numOflayers;
    int neurons_initial;
    int neurons_hidden;
    int neurons_outer;
    int currentLayer;
    int **inputPipe;
    int **outputPipe;
    Layer * layers;


    void initialize(int numOfLayers, int numOfNeurons_initialLayer, int numOfNeurons_hiddenLayers, int numOfNeurons_outerLayer) 
    {
        this->numOflayers = numOfLayers;

        this->neurons_initial = numOfNeurons_initialLayer;
        this->neurons_hidden = numOfNeurons_hiddenLayers;
        this->neurons_outer = numOfNeurons_outerLayer;
        
        layers = new Layer[numOfLayers];

        // Initializing the initial layer
        this->layers[0].numOfNeurons = 2;
        this->layers[0].weights = new float*[2]; 
        for(int i = 0; i < 2; i++)
            this->layers[0].weights[i] = new float[8];

        // Initializing hidden layers
        int nurons = 8;
        for(int i = 1; i < this->numOflayers - 1; i++){
            this->layers[i].initialize(nurons);
        }

        // Initializing outer layer
        this->layers[numOflayers-1].numOfNeurons = 1;
        this->layers[numOflayers-1].weights = new float*[8];
        for(int i = 0; i < 8; i++)
            this->layers[numOflayers-1].weights[i] = new float;

        inputPipe= new int * [numOfLayers];
        outputPipe= new int * [numOfLayers];

        for(int i=0;i<numOfLayers;i++)
        {
            inputPipe[i]=new int[numOfLayers];
            outputPipe[i]= new int [numOfLayers];
        }

        for(int i=0;i<numOfLayers;i++)
        {
            if(pipe(inputPipe[i])==-1|| pipe(outputPipe[i])==-1)
            {
                cout << "error creating pipes " <<endl;
            }
        }
        

        //cout << "All initializations done!" << endl;

    }

    void readInputs()
    {   
        string filename = "initLayer.txt";
        ifstream myFile;
        myFile.open(filename);
        string line;


        // Storing the initial_layer
        int TOTALROWS = 2;                        // Row size depends upon rows in file.
        int cols = 0;
        for(int a = 0; a < TOTALROWS; a++){
            getline(myFile, line);
            stringstream ss(line);

            string substr;
            while(getline(ss, substr, ',')){
                this->layers[0].weights[a][cols++] = stof(substr);
            }
            cols = 0;
        }

        //cout << "storing to initial Layer done!" << endl;
        myFile.close();

        // Storing to hidden layers
        int TOTAL_HIDDEN_FILES = numOflayers - 2;    // Total size of hidden layers

        for(int i = 1; i <= TOTAL_HIDDEN_FILES; i++)
        {
            filename = "hiddenLayer" + to_string(i) + ".txt";
            myFile.open(filename);
            for(int a = 0; a < this->layers[i].numOfNeurons; a++){
                getline(myFile, line);
                stringstream ss(line);

                string substr;
                while(getline(ss, substr, ',')){
                    this->layers[i].weights[a][cols++] = stof(substr);
                }
                cols = 0;
            }
            myFile.close();
        }
        //cout << "storing to hidden Layer done!" << endl;
        
        // Storing to final outer layer
        filename = "outerLayer.txt";

        myFile.open(filename);

        int TOTALROWS2 = 8;                        // Row size depends upon rows in file.
        for(int a = 0; a < TOTALROWS2; a++){
            getline(myFile, line);
            this->layers[this->numOflayers-1].weights[a][0] = stof(line);
        }
        //cout << "storing to outer Layer done!" << endl;
        myFile.close();
    }

    void displayfilesData(int numFiles){
        for(int a = 0; a < numFiles; a++){
            if(a == 0){   // initial layer contents
                cout << "initial layer: " << endl;
                for(int i = 0; i < 2; i++){
                    for(int j = 0; j < 8; j++){
                        cout << this->layers[a].weights[i][j] << " ";
                    }
                    cout << endl;
                }
            }
            else if(a == numFiles-1){  // outer layer contents
                cout << "outer layer: " << endl;
                for(int i = 0; i < 8; i++){
                    cout << this->layers[a].weights[i][0] << endl;
                }
            }
            else{  // hidden layers contents
                cout << "hidden layers " << a << ":"<< endl;
                for(int i = 0; i < this->layers[a].numOfNeurons; i++){
                    for(int j = 0; j < this->layers[a].numOfNeurons; j++){
                        cout << this->layers[a].weights[i][j] << " ";
                    }
                    cout << endl;
                }
            }
        }
    }
    
}; 