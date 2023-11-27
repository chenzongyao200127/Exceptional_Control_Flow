#include <stdio.h>
#include <setjmp.h>

jmp_buf jump_buffer;

void my_function() {
    // 遇到错误情况，跳转
    longjmp(jump_buffer, 1);
}

int main() {
    if (setjmp(jump_buffer) == 0) {
        // 正常执行
        printf("Calling my_function\n");
        my_function();
        printf("my_function returned\n");
    } else {
        // 从 my_function 的 longjmp 跳转回来
        printf("Jumped back from my_function\n");
    }
    return 0;
}