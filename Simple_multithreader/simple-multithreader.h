
#ifndef SIMPLE_MULTITHREADER_H
#define SIMPLE_MULTITHREADER_H

#include <pthread.h>      
#include <functional>     
#include <iostream>             
// Existing contents of the file remain unchanged
int user_main(int argc, char **argv);

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> &&lambda) {
    lambda();
}

int main(int argc, char **argv) {
    /*
     * Declaration of a sample C++ lambda function
     * that captures variable 'x' by value and 'y'
     * by reference. Global variables are by default
     * captured by reference and are not to be supplied
     * in the capture list. Only local variables must be 
     * explicitly captured if they are used inside lambda.
     */

    int x = 5, y = 1;
    // Declaring a lambda expression that accepts void type parameter
    auto lambda1 = [x, &y](void) {
        /* Any changes to 'x' will throw a compilation error as x is captured by value */
        y = 5;
        std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
        /* you can have any number of statements inside this lambda body */
    };
    // Executing the lambda function
    demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

    int rc = user_main(argc, argv);

    auto lambda2 = []() {
        std::cout << "====== Hope you enjoyed CSE231(A) ======\n";
        /* you can have any number of statements inside this lambda body */
    };
    demonstration(lambda2);
    return rc;
}

#define main user_main

struct ThreadData {
    std::function<void()> task;
};


void* thread_func(void* arg) {
    auto* data = static_cast<ThreadData*>(arg);
    data->task(); 
    delete data;       
    return nullptr;
}



void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
    if (low >= high) {
        std::cerr << "invalid - low is higher than high\n";
        return;
    }

    if (numThreads <= 0) {
        std::cerr << "thread less than 0\n";
        return;
    }

    int size = (high - low) / numThreads;
    int extra = (high - low) % numThreads;
    if (size <= 0) {
        std::cerr << "Number of threads exceeds available work\n";
        return;
    }

    pthread_t threads[numThreads];
    clock_t start = clock();

    for (int i = 0; i < numThreads; i++) {
        int s_index = low + i * size + std::min(i, extra);
        int e_index = s_index + size + (i < extra ? 1 : 0);

        auto work_l = [lambda, s_index, e_index]() {
            for (int j = s_index; j < e_index; j++) {
                lambda(j);
            }
        };


        auto* data = new ThreadData{work_l};

        int result = pthread_create(&threads[i], nullptr, thread_func, (void*)data);
        if (result != 0) {
            std::cerr << "Error Failed to create thread " << i << ".\n";
            delete data;
            return;
        }
    }

    for (int i = 0; i < numThreads; i++) {
        int result = pthread_join(threads[i], nullptr);
        if (result != 0) {
            std::cerr << "Error Failed to join thread " << i << ".\n";
        }
    }
    clock_t end = clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Execution time: " << duration * 1000 << " ms\n";
}


void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads) {
    if (low1 >= high1 || low2 >= high2) {
        std::cerr << "Invalid loop ranges low>high.\n";
        return;
    }

    if (numThreads <= 0) {
        std::cerr << "Number of threads must be greater than 0.\n";
        return;
    }

    int size = (high1 - low1) / numThreads;
    if (size == 0) {
        std::cerr << "Number of threads exceeds available rows.\n";
        return;
    }

    pthread_t threads[numThreads];
    clock_t start = clock();
    
    for (int i = 0; i < numThreads; i++) {
        int s_row = low1 + i * size;
        int e_row = (i == numThreads - 1) ? high1 : s_row + size;
    

        auto work_l = [lambda, s_row, e_row, low2, high2]() {
            for (int row = s_row; row < e_row; row++) {
                for (int col = low2; col < high2; col++) {
                    lambda(row, col);
                }
            }
        };

        auto* data = new ThreadData{work_l};
        int result = pthread_create(&threads[i], nullptr, thread_func, (void*)data);
        if (result != 0) {
            std::cerr << "Error Failed to create thread " << i << ".\n";
            delete data;
            return;
        }
    }

    for (int i = 0; i < numThreads; i++) {
        int result = pthread_join(threads[i], nullptr);
        if (result != 0) {
            std::cerr << "Error Failed to join thread " << i << ".\n";
        }
    }

    clock_t end = clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Execution time: " << duration * 1000 << " ms\n";
}

#endif 
