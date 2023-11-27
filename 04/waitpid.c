// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>

// int main() {
//     pid_t pid;

//     // 创建子进程
//     pid = fork();

//     if (pid == -1) {
//         // fork失败
//         perror("fork failed");
//         return -1;
//     } else if (pid == 0) {
//         // 子进程
//         printf("Child process: PID = %d\n", getpid());
//         // 子进程执行完毕并退出
//         exit(0);
//     } else {
//         // 父进程
//         printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);

//         // 父进程故意“忙碌”一段时间，不立即回收子进程
//         sleep(10);

//         // 父进程现在回收子进程
//         waitpid(pid, NULL, 0);
//         printf("Child process has been reaped\n");
//     }

//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>

// int main() {
//     pid_t pid;
//     int status;

//     pid = fork();

//     if (pid == -1) {
//         // fork失败
//         perror("fork failed");
//         return -1;
//     } else if (pid == 0) {
//         // 子进程
//         printf("Child process: PID = %d\n", getpid());
//         // 模拟子进程执行一些操作
//         sleep(5);
//         exit(0);  // 子进程退出
//     } else {
//         // 父进程
//         printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
//         while (1) {
//             // 使用WNOHANG | WUNTRACED选项调用waitpid
//             pid_t w = waitpid(pid, &status, WNOHANG | WUNTRACED);
//             if (w == 0) {
//                 // 子进程还在运行
//                 printf("No change in child process, parent does some work...\n");
//                 sleep(1); // 父进程执行其他工作
//             } else if (w == -1) {
//                 // 错误发生
//                 perror("waitpid");
//                 exit(EXIT_FAILURE);
//             } else {
//                 // 子进程已终止或被停止
//                 if (WIFEXITED(status)) {
//                     printf("Child exited, status=%d\n", WEXITSTATUS(status));
//                 } else if (WIFSTOPPED(status)) {
//                     printf("Child stopped by signal %d\n", WSTOPSIG(status));
//                 }
//                 break; // 退出循环
//             }
//         }
//     }

//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#define N 2


// int main() {
//     pid_t pid;
//     int status;

//     pid = fork();

//     if (pid == -1) {
//         // fork失败
//         perror("fork failed");
//         return -1;
//     } else if (pid == 0) {
//         // 子进程
//         printf("Child process: PID = %d\n", getpid());

//         // 根据需要修改这里以模拟不同的退出情况
//         // 正常退出
//         // exit(0);

//         // 通过信号终止
//         abort();  // 生成 SIGABRT 信号

//     } else {
//         // 父进程
//         printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
//         waitpid(pid, &status, 0); // 等待子进程状态改变

//         if (WIFEXITED(status)) {
//             printf("Child exited normally with exit status %d\n", WEXITSTATUS(status));
//         } else if (WIFSIGNALED(status)) {
//             printf("Child terminated by signal %d\n", WTERMSIG(status));
//         } else if (WIFSTOPPED(status)) {
//             printf("Child stopped by signal %d\n", WSTOPSIG(status));
//         } else if (WIFCONTINUED(status)) {
//             printf("Child continued\n");
//         }
//     }

//     return 0;
// }

void unix_error(char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int main()
{
    int status, i;
    pid_t pid[N], retpid;

    /* Parent creates N children */
    for (i = 0; i < N; i++)
        if ((pid[i] = fork()) == 0) /* Child */
            exit(100+i);

    /* Parent reaps N children in order */
    i = 0;
    while ((retpid = waitpid(pid[i++], &status, 0)) > 0) {
        if (WIFEXITED(status))
            printf("child %d terminated normally with exit status=%d\n",
                   retpid, WEXITSTATUS(status));
    else
        printf("child %d terminated abnormally\n", retpid);
    }

    /* The only normal termination is if there are no more children */
    if (errno != ECHILD)
        unix_error("waitpid error");
    
    exit(0);
}