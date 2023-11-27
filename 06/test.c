#include <stdio.h>
#include <stdlib.h>

int counter = 1;

int main()
{
    if (fork() == 0) {
        printf("a"); fflush(stdout);
        exit(0);
    }
    else {
        printf("b"); fflush(stdout);
        waitpid(-1, NULL, 0);
    }
    printf("c"); fflush(stdout);
    exit(0);
}