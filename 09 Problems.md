# Questions
====================================================================================================
在这一章里，我们介绍了一些具有不寻常的调用和返回行为的函数：setjmp、longjmp、execve 和 fork。找到下列行为中和每个函数相匹配的一种：
A. 调用一次，返回两次。
B. 调用一次，从不返回。
C. 调用一次，返回一次或者多次。

Answer:
`setjmp`    C
`longjmp`   B
`execve`    B
`fork`      A

====================================================================================================

这个程序会输出多少个 “hello” 输出行？
~~~c
#include "csapp.h"

int main()
{
    int i;

    for (i = 0; i < 2; i++)
        Fork();
    printf("hello");
    exit(0);
}
~~~
A: 4

~~~shell
                        c
                    +-------+
                    |    "hello"
                    |    
                c   |   p
            +-------+-------+
            |     fork   "hello"
            |
            |           c
            |       +-------+
            |       |    "hello"
            |       |    
            |   p   |   p    
     +------+-------+-------+
    main  fork    fork   "hello"
~~~
====================================================================================================

这个程序会输出多少个 “hello” 输出行？
~~~c
#include "csapp.h"

void doit()
{
    Fork();
    Fork();
    printf("hello\n");
    return;
}

int main()
{
    doit ();
    printf("hello\n");
    exit(0);
}
~~~
A: 8

~~~shell
                        c
                    +-------+---------+
                    |    "hello"   "hello"
                    |    
                c   |   p
            +-------+-------+---------+
            |     fork   "hello"   "hello"
            |
            |           c
            |       +-------+---------+
            |       |    "hello"   "hello"
            |       |    
            |   p   |   p    
     +------+-------+-------+---------+
    main  fork    fork   "hello"   "hello"
~~~
====================================================================================================

下面程序的一种可能的输出是什么？
~~~c
#include "csapp.h"

int main() 
{
    int x = 3;

    if (Fork() != 0)
        printf("x=%d\n", ++x);

    printf("x=%d\n", --x);
    exit(0);
}
~~~

Answer:
432
243
423
====================================================================================================

这个程序会输出多少个 “hello” 输出行？
~~~c
#include "csapp.h"

void doit()
{
    if (Fork() == 0) {
        Fork();
        printf("hello\n");
        exit(0);
    }
    return;
}

int main()
{
    doit();
    printf("hello\n");
    exit(0);
}
~~~
A: 3
                        c
                    +-------+
                    |    "hello"
                    |    
                c   |   p
            +-------+-------+
            |     fork   "hello"
            |
            |   p   
     +------+-------+-------+
    main  fork   return  "hello"

====================================================================================================

这个程序会输出多少个 “hello” 输出行？
~~~c
#include "csapp.h"

void doit()
{
    if (Fork() == 0) {
        Fork();
        printf("hello\n");
        return;
    }
    return;
}

int main()
{
    doit();
    printf("hello\n");
    exit(0);
}
~~~

                        c
                    +-------+---------+
                    |    "hello"   "hello"
                    |    
                c   |   p
            +-------+-------+---------+
            |     fork   "hello"   "hello"
            |
            |
            |   p   
     +------+-------+-------+
    main  fork   return  "hello"

====================================================================================================

下面这个程序的输出是什么？
~~~c
#include "csapp.h"
int counter = 1;

int main()
{
    if (fork() == 0) {
        counter--;
        exit(0);
    }
    else {
        Wait(NULL);
        printf("counter = %d\n", ++counter);
    }
    exit(0);
}
~~~
A: 2

====================================================================================================

考虑下面的程序：
~~~c
#include "csapp.h"

void end(void)
{
    printf("2"); fflush(stdout);
}

int main()
{
    if (Fork() == 0)
        atexit(end);
    if (Fork() == 0) {
        printf("0"); fflush(stdout);
    }
    else {
        printf("1"); fflush(stdout);
    }
    exit(0);
}
~~~

判断下面哪个输出是可能的。
注意：atexit 函数以一个指向函数的指针为输入，并将它添加到函数列表中（初始为空），当 exit 函数被调用时，会调用该列表中的函数。
A. 112002
B. 211020
C. 102120
D. 122001
E. 100212 

Answer: B & D is impossible.
                        c
                    +-------+---------+
                    |      "0"     exit "2"
                    |    
                c   |   p
            +-------+-------+---------+
            |     fork     "1"     exit "2"
            |   (atexit)
            |           c
            |       +-------+---------+
            |       |      "0"      exit
            |       |    
            |   p   |   p    
     +------+-------+-------+---------+
    main  fork    fork     "1"      exit

====================================================================================================

下面的函数会打印多少行输出？用一个 n 的函数给出答案。假设 `n >= 1`。
~~~c
void foo(int n)
{
    int i;

    for (i = 0; i < n; i++)
        Fork();
    printf("hello\n");
    exit(0);
}
~~~
A: 2 ^ n
====================================================================================================

使用 execve 编写一个叫做 `myls` 的程序，该程序的行为和 /bin/ls 程序的一样。你的程序应该接受相同的命令行参数，解释同样的环境变量，并产生相同的输岀。
Is 程序从 COLUMNS 环境变量中获得屏幕的宽度。如果没有设置 COLUMNS，那么 ls 会假设屏幕宽 80 列。
因此，你可以通过把 COLUMNS 环境设置得小于 80，来检查你对环境变量的处理：

~~~shell
linux> setenv COLUMNS 40
linux> ./myls
.
. // Output is 40 columns wide
.
linux> unsetenv COLUMNS
linux> ./myls
.
. // Output is now 80 columns wide
.
~~~

~~~c
/*
 * 8.20.c
 */
#include <stdio.h>
#include "csapp.h"

int main(int argc, char* argv[], char* env[]) {
  if (execve("/bin/ls", argv, env) == -1) {
    fprintf(stderr, "execve error: %s\n", strerror(errno));
    exit(1);
  }
}
~~~

====================================================================================================


下面的程序可能的输岀序列是什么？
~~~c
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
~~~

Answer: bac or abc

====================================================================================================

编写 Unix system 函数的你自己的版本
~~~c
int mysystem(char* command);
~~~
mysystem 函数通过调用 “/bin/sh-ccommand” 来执行 command，然后在 command 完成后返回。
如果 command（通过调用 exit 函数或者执行一条 return 语句）正常退出，那么 mysystem 返回 command 退出状态。例如，如果 command 通过调用 exit(8) 终止，那么 mysystem 返回值 8。否则，如果 command 是异常终止的，那么 mysystem 就返回 shell 返回的状态。

Answer:
~~~c
/*
 * mysystem.c
 */
#include <stdio.h>
#include "csapp.h"

int mysystem(char* command) {
  pid_t pid;
  int status;

  if ((pid = Fork()) == 0) {
    /* child process */
    char* argv[4] = { "", "-c", command, NULL };
    execve("/bin/sh", argv, environ);
  }

  /* print child pid so we can kill it */
  printf("child pid: %d\n", pid);

  if (Waitpid(pid, &status, 0) > 0) {
    /* exit normally */
    if (WIFEXITED(status))
      return WEXITSTATUS(status);

    /* exit by signal */
    if (WIFSIGNALED(status))
      return WTERMSIG(status);
  }
}

int main(int argc, char* argv[]) {
  int code;

  code = mysystem("./exit-code");
  printf("normally exit, code: %d\n", code); fflush(stdout);

  code = mysystem("./wait-sig");
  printf("exit caused by signal, code: %d\n", code); fflush(stdout);
  return 0;
}
~~~

====================================================================================================


你的一个同事想要使用信号来让一个父进程对发生在子进程中的事件计数。其想法是每次发生一个事件时，通过向父进程发送一个信号来通知它，并且让父进程的信号处理程序对一个全局变量 counter 加一，在子进程终止之后，父进程就可以检查这个变量。
然而，当他在系统上运行图 8-45 中的测试程序时，发现当父进程调用 printf 时，counter 的值总是 2，即使子进程向父进程发送了 5 个信号也是
如此。他很困惑，向你寻求帮助。你能解释这个程序有什么错误吗？

~~~c
#include "csapp.h"

int counter = 0;

void handler(int sig)
{
    counter++;
    sleep(1); /* Do some work in the handler */
    return;
}

int main()
{
    int i;

    Signal(SIGUSR2, handler);

    if (Fork() == 0) { /* Child */
        for (i = 0; i < 5; i++) {
            Kill(getppid(), SIGUSR2);
            printf("sent SIGUSR2 to parent\n");
        }
        exit(0);
    }
    
    Wait(NULL);
    printf("counter=%d\n", counter);
    exit(0);
}
~~~
~~~shell
   SIGUSR2      SIGUSR2  SIGUSR2  SIGUSR2  SIGUSR2
      |            |        |        |        |
being handled   Pending  Canceld  Canceld  Canceld
 need 1 sec
~~~
there’s only one pending signal all the time. other same type signals will be canceled.

if you modify code

remove sleep

`/* sleep(1); */`
or

send more signals

`for (i = 0; i < 500000; i++)`

you will get different counter output.

====================================================================================================

修改图 8-18 中的程序，以满足下面两个条件：
~~~c
#include "csapp.h"

void end(void)
{
    printf("2"); fflush(stdout);
}

int main()
{
    if (Fork() == 0)
        atexit(end);
    if (Fork() == 0) {
        printf("0"); fflush(stdout);
    }
    else {
        printf("1"); fflush(stdout);
    }
    exit(0);
}
~~~
1）每个子进程在试图写一个只读文本段中的位置时会异常终止。
2）父进程打印和下面所示相同（除了 PID）的输出：
~~~shell
child 12255 terminated by signal 11: Segmentation fault
child 12254 terminated by signal 11: Segmentation fault
~~~
提示：请参考 psignal(3) 的 man 页。

====================================================================================================

编写 fgets 函数的一个版本，叫做 tfgets，它 5 秒钟后会超时。tfgets 函数接收和 fgets 相同的输入。如果用户在 5 秒内不键入一个输入行，tfgets 返回 NULL。否则，它返回一个指向输入行的指针。

https://dreamanddead.github.io/CSAPP-3e-Solutions/chapter8/8.24/

====================================================================================================


Shell Lab
https://hansimov.gitbook.io/csapp/labs/shell-lab