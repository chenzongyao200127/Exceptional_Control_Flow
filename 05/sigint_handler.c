#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int sig) {
    write(1, "Caught SIGINT\n", 14);
}

int main() {
    // 设置 SIGINT 的处理函数
    signal(SIGINT, sigint_handler);

    // 阻塞 SIGINT 信号
    sigset_t sigset, oldset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigprocmask(SIG_BLOCK, &sigset, &oldset);

    // 执行关键操作
    printf("Critical operation started\n");
    sleep(5);  // 模拟长时间运行的操作
    printf("Critical operation finished\n");

    // 检查待处理的 SIGINT 信号并解除阻塞
    sigset_t pending;
    sigpending(&pending);
    if (sigismember(&pending, SIGINT)) {
        printf("SIGINT was pending\n");
    }
    sigprocmask(SIG_SETMASK, &oldset, NULL);

    return 0;
}
