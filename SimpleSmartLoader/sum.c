#include <stdio.h>
#include <stdlib.h>

#define SIZE 1024

int __attribute__((aligned(4096))) A[SIZE] = { 0 };
int sum = 0;

void _start() {
    for (int i = 0; i < SIZE; i++) {
        A[i] = 2;
    }
    for (int i = 0; i < SIZE; i++) {
        sum += A[i];
    }
    printf("Sum = %d\n", sum);
    _exit(0);
}
