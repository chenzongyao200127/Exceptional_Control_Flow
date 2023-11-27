#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void alarm_handler(int sig) {
    printf("Alarm signal received\n");
    // 执行一些操作，比如超时处理
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(5);  // 设置 5 秒闹钟

    // 执行一些操作
    pause();  // 等待信号到达

    return 0;
}