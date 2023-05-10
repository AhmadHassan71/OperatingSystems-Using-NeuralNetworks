#include <iostream>
#include "neuralNetwork.h"
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

// Global variables
pthread_mutex_t mutex;
bool finished = false;
int remainingLayers;

// Layer process function
void* layerProcess(void* args) {
    // Unpack arguments
    int layerIndex = *(int*) args;
    const std::vector<int>& readPipes = *(const vector<int>*) (args + sizeof(int));
    const std::vector<int>& writePipes = *(const vector<int>*) (args + sizeof(int) + sizeof(std::vector<int>));
    NeuralNetwork& neuralNetwork = *(NeuralNetwork*) (args + sizeof(int) + sizeof(std::vector<int>) + sizeof(std::vector<int>));

    // Read inputs from the pipe
    std::vector<double> inputs;
    double input;
    while (read(readPipes[layerIndex], &input, sizeof(double)) > 0) {
        inputs.push_back(input);
    }

    // Perform computations
    std::vector<double> outputs;
    neuralNetwork.layers[layerIndex].forwardPropagate(inputs, outputs);

    // Write outputs to the next layer through the pipe
    for (double output : outputs) {
        write(writePipes[layerIndex], &output, sizeof(double));
    }

    // Decrement the remaining layers
    pthread_mutex_lock(&mutex);
    remainingLayers--;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    NeuralNetwork neuralNetwork;

    // Define the neural network layers and initialize the weights

    const int numLayers = neuralNetwork.layers.size();
    vector<std::string> pipeNames(numLayers - 1);
    vector<int> readPipes(numLayers - 1);
    vector<int> writePipes(numLayers - 1);
    vector<pthread_t> threads(numLayers - 1);

    // Create the named pipes
    for (int i = 0; i < numLayers - 1; i++) {
        pipeNames[i] = "/tmp/layer" + std::to_string(i) + "-" + std::to_string(i + 1) + ".pipe";
        mkfifo(pipeNames[i].c_str(), 0666);
    }

    // Open the pipes for reading and writing
    for (int i = 0; i < numLayers - 1; i++) {
        readPipes[i] = open(pipeNames[i].c_str(), O_RDONLY | O_NONBLOCK);
        writePipes[i] = open(pipeNames[i].c_str(), O_WRONLY);
    }

    // Start the layer processes
    remainingLayers = numLayers - 1;
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < numLayers - 1; i++) {
        void* args = malloc(sizeof(int) + 2 * sizeof(std::vector<int>) + sizeof(NeuralNetwork));
        *((int*) args) = i;
        //std::copy(readPipes.begin(), readPipes.end(), (std::vector<int>*)(args + sizeof(int)));
        //std::copy(writePipes.begin(), writePipes.end(), (std::vector<int>*)(args + sizeof(int) + sizeof(std::vector<int>)));
        *(NeuralNetwork*) (args + sizeof(int) + 2 * sizeof(std::vector<int>)) = neuralNetwork;
        pthread_create(&threads[i], NULL, layerProcess, args);
    }

    // Load inputs
    std::vector<double> inputs;
    // Load inputs into the inputs vector

    // Write inputs to the first layer's pipe
    for (double input : inputs) {
        write(writePipes[0], &input, sizeof(double));
    }

    // Wait for the layer processes to finish their computations
    while (remainingLayers > 0) {
        // Waiting for the layers to finish
    }
// Read the final outputs from the last layer's pipe
    std::vector<double> outputs;
    double output;
    while (read(readPipes[numLayers - 2], &output, sizeof(double)) > 0) {
        outputs.push_back(output);
    }

    // Clean up the pipes
    for (int i = 0; i < numLayers - 1; i++) {
        close(readPipes[i]);
        close(writePipes[i]);
        remove(pipeNames[i].c_str());
    }

    // Destroy the mutex
    pthread_mutex_destroy(&mutex);

    // Use the final outputs as needed
    // ...

    return 0;
}



/*
1- Neuron Structure:

struct Neuron {
    double input;
    double output;
    std::vector<double> weights;
}; 
2- Layer Structure:

struct Layer {
    std::vector<Neuron> neurons;

    void initialize(int numNeurons, int numInputs) {
        neurons.resize(numNeurons);
        for (auto& neuron : neurons) {
            neuron.weights.resize(numInputs);
        }
    }

    void forwardPropagate(const std::vector<double>& inputs, std::vector<double>& outputs) {
        outputs.resize(neurons.size());
        for (size_t i = 0; i < neurons.size(); ++i) {
            double sum = 0.0;
            for (size_t j = 0; j < inputs.size(); ++j) {
                sum += neurons[i].weights[j] * inputs[j];
            }
            outputs[i] = sum;
        }
    }
}; 
3 - Neural Network Initialization:


struct NeuralNetwork {
    std::vector<Layer> layers;

    void addLayer(int numNeurons, int numInputs) {
        Layer layer;
        layer.initialize(numNeurons, numInputs);
        layers.push_back(layer);
    }

    void initialize() {
        for (size_t i = 1; i < layers.size(); ++i) {
            int numInputs = layers[i - 1].neurons.size();
            layers[i].initialize(layers[i].neurons.size(), numInputs);
        }
    }

    void propagateInputs(const std::vector<double>& inputs, std::vector<double>& outputs) {
        for (size_t i = 0; i < layers.size(); ++i) {
            if (i == 0) {
                layers[i].forwardPropagate(inputs, outputs);
            } else {
                layers[i].forwardPropagate(outputs, outputs);
            }
        }
    }
}; 
4 - Multi-threading and Communication:


// Example code for two layers running in separate processes
// Layer 1 Process
void layer1Process(int readPipe, int writePipe) {
    // Read inputs from the pipe
    std::vector<double> inputs;
    // Read inputs from the readPipe

    // Perform computations
    std::vector<double> outputs;
    neuralNetwork.layers[0].forwardPropagate(inputs, outputs);

    // Send outputs to the next layer through the writePipe
    // Write outputs to the writePipe
}

// Layer 2 Process
void layer2Process(int readPipe, int writePipe) {
    // Read inputs from the pipe
    std::vector<double> inputs;
    // Read inputs from the readPipe

    // Perform computations
    std::vector<double> outputs;
    neuralNetwork.layers[1].forwardPropagate(inputs, outputs);

    // Send outputs to the next layer through the writePipe
    // Write outputs to the writePipe
} 
 5 - Forward Propagation:


std::vector<double> inputs = {1.0, 2.0, 3.0}; // Example inputs
std::vector<double> outputs;

// Propagate inputs through the layers
neuralNetwork.propagateInputs(inputs, outputs); 
6 - Backward Propagation:


// Example backward propagation from the output layer to the first layer
for (size_t i = neuralNetwork.layers.size() - 1; i > 0; --i) {
    // Read inputs from the pipe
    std::vector<double> inputs;
    // Read inputs from the readPipe

    // Send outputs to the previous layer through the
#include <unistd.h> // For pipe, fork, close
#include <sys/wait.h> // For wait

void layer1Process(int readPipe, int writePipe) {
    // Close unused pipe ends
    close(readPipe);

    // Read inputs from the pipe
    std::vector<double> inputs;
    // Read inputs from the writePipe

    // Perform computations
    std::vector<double> outputs;
    neuralNetwork.layers[0].forwardPropagate(inputs, outputs);

    // Send outputs to the next layer through the writePipe
    // Write outputs to the readPipe

    // Close the pipe after writing
    close(writePipe);
}
void layer2Process(int readPipe, int writePipe) {
    // Close unused pipe ends
    close(writePipe);

    // Read inputs from the pipe
    std::vector<double> inputs;
    // Read inputs from the readPipe

    // Perform computations
    std::vector<double> outputs;
    neuralNetwork.layers[1].forwardPropagate(inputs, outputs);

    // Send outputs to the next layer through the writePipe
    // Write outputs to the writePipe

    // Close the pipe after writing
    close(readPipe);
}

int main() {
    int pipe1[2]; // Pipe between layer 1 and layer 2
    int pipe2[2]; // Pipe between layer 2 and layer 1

    // Create the pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid_t child1 = fork();
    if (child1 == -1) {
        perror("Fork failed");
        exit(1);
    } else if (child1 == 0) {
        // Child process (layer 1)
        layer1Process(pipe1[0], pipe2[1]);
        exit(0);
    }

    pid_t child2 = fork();
    if (child2 == -1) {
        perror("Fork failed");
        exit(1);
    } else if (child2 == 0) {
        // Child process (layer 2)
        layer2Process(pipe2[0], pipe1[1]);
        exit(0);
    }

    // Parent process

    // Close unused pipe ends
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    // Wait for child processes to finish
    waitpid(child1, nullptr, 0);
    waitpid(child2, nullptr, 0);

    return 0;
}
*/