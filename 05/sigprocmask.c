#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>

void sigint_handler(int sig) {
    printf("Caught SIGINT\n");
}

int main() {
    sigset_t newset, oldset;

    // 设置 SIGINT 的处理函数
    signal(SIGINT, sigint_handler);

    // 初始化信号集 newset，并添加 SIGINT 信号
    sigemptyset(&newset);
    sigaddset(&newset, SIGINT);

    // 阻塞 SIGINT 信号，并保存当前信号掩码
    sigprocmask(SIG_BLOCK, &newset, &oldset);

    // 执行关键代码段
    printf("Critical section start\n");
    sleep(5); // 模拟长时间执行
    printf("Critical section end\n");

    // 解除阻塞 SIGINT 信号
    sigprocmask(SIG_SETMASK, &oldset, NULL);

    // 程序继续执行，可以响应 SIGINT 信号
    printf("SIGINT is now unblocked\n");
    sleep(5);

    return 0;
}
