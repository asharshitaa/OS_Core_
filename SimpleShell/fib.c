#include <stdio.h>
#include <stdlib.h>

int fib(int n) {
    if (n < 2) return n;
    else return fib(n - 1) + fib(n - 2);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("error");
        return 1; 
    }

    int n = atoi(argv[1]);

    int val = fib(n);
    printf("Fibonacci of %d is %d\n", n, val);
    return 0;
}

