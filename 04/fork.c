// #include <stdio.h>

// int main()
// {
//     int pid;
//     int x = 1;

//     pid = fork();
//     if (pid == 0) { /* Child */
//         printf("child : x=%d\n", ++x);
//         exit(0);
//     }
    
//     /* Parent */
//     printf("parent: x=%d\n", --x);
//     exit(0);
// }

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

// int main() {
//     pid_t pid;

//     pid = fork();

//     if (pid == -1) {
//         // Fork failed
//         perror("fork failed");
//         return -1;
//     } else if (pid == 0) {
//         // This is the child process
//         printf("Child process with PID %d\n", getpid());
//     } else {
//         // This is the parent process
//         printf("Parent process with PID %d, Child PID %d\n", getpid(), pid);
//     }

//     return 0;
// }

int main()
{
    int x = 1;
    
    if (fork() == 0)
        printf("p1: x=%d\n", ++x);
    printf("p2: x=%d\n", --x);
    exit(0);
}
