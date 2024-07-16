#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_INPUTS 2
#define LEARNING_RATE 0.1
#define NUM_EPOCHS 100

float random_float() {
    return (float)rand() / RAND_MAX;
}

int step_function(float x) {
    return x >= 0 ? 1 : 0;
}

float calculate_output(float inputs[], float weights[], float bias) {
    float sum = bias;
    for (int i = 0; i < NUM_INPUTS; i++) {
        sum += inputs[i] * weights[i];
    }
    return sum;
}

int predict(float inputs[], float weights[], float bias) {
    float output = calculate_output(inputs, weights, bias);
    return step_function(output);
}

void train_perceptron(float training_data[][NUM_INPUTS], int labels[], int num_samples) {
    float weights[NUM_INPUTS];
    float bias = random_float();

    for (int i = 0; i < NUM_INPUTS; i++) {
        weights[i] = random_float();
    }

    for (int epoch = 0; epoch < NUM_EPOCHS; epoch++) {
        for (int i = 0; i < num_samples; i++) {
            float output = calculate_output(training_data[i], weights, bias);
            int prediction = step_function(output);
            int error = labels[i] - prediction;

            for (int j = 0; j < NUM_INPUTS; j++) {
                weights[j] += LEARNING_RATE * error * training_data[i][j];
            }
            bias += LEARNING_RATE * error;
        }
    }

    printf("Final weights: [%.2f, %.2f]\n", weights[0], weights[1]);
    printf("Final bias: %.2f\n", bias);

		float test_inputs[][NUM_INPUTS] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int num_tests = sizeof(test_inputs) / sizeof(test_inputs[0]);

    printf("\nTesting the perceptron:\n");
    for (int i = 0; i < num_tests; i++) {
        int result = predict(test_inputs[i], weights, bias);
        printf("Input: [%.0f, %.0f] Output: %d\n", 
               test_inputs[i][0], test_inputs[i][1], result);
    }

}

int main() {
    srand(time(NULL));

    float training_data[][NUM_INPUTS] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    int labels[] = {0, 0, 0, 1};
    int num_samples = sizeof(labels) / sizeof(labels[0]);

    train_perceptron(training_data, labels, num_samples);

    return 0;
}
