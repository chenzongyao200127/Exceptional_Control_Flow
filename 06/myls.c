#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *ls_path = "/bin/ls";
    char *newargv[argc + 1]; // 创建新的参数数组，大小等于原始参数加上ls路径
    char *newenviron[] = { NULL }; // 初始化环境变量列表

    newargv[0] = ls_path; // 第一个参数是ls的路径
    for (int i = 1; i < argc; i++) {
        newargv[i] = argv[i]; // 复制剩余的参数
    }
    newargv[argc] = NULL; // 确保参数列表以NULL结尾

    // 检查COLUMNS环境变量
    char *columns_env = getenv("COLUMNS");
    if (columns_env != NULL) {
        char columns_str[50]; // 足够长的字符串来存储"COLUMNS=xxx"
        snprintf(columns_str, 50, "COLUMNS=%s", columns_env);
        newenviron[0] = columns_str; // 设置新的环境变量
    }

    // 调用 execve 来执行 /bin/ls 程序
    if (execve(ls_path, newargv, newenviron[0] != NULL ? newenviron : newenviron) == -1) {
        perror("execve"); // 如果execve失败，打印错误信息
        exit(EXIT_FAILURE);
    }

    return 0;
}
