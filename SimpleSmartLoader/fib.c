#include <stdio.h>
#include <stdlib.h>

long long int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

int main() {
    int val = fib(40);
    printf("Fibonacci(40) = %d\n", val);
    exit(0);
}
