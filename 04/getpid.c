#include <unistd.h>
#include <stdio.h>

int main() {
    int pid;
    pid = getpid();
    printf("The Process ID is %d\n", pid);
    return 0;
}
