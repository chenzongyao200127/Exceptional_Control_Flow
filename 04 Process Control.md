# Process Control
# 进程控制

====================================================================================================
Unix 提供了大量从 C 程序中操作进程的系统调用。这一节将描述这些重要的函数，并举例说明如何使用它们。

Unix提供了一系列强大的系统调用，允许C程序创建和管理进程。这些系统调用是Unix风格操作系统进程管理的基础。以下是一些重要的进程操作系统调用及其用法示例：

### 1. fork()

- **用途**：创建一个新的子进程，它是当前进程的副本。
- **行为**：`fork()`调用一次，返回两次。在父进程中返回子进程的PID，在子进程中返回0。
- **示例**：
  ```c
  pid_t pid = fork();
  if (pid == -1) {
      // 错误处理
  } else if (pid == 0) {
      // 子进程代码
  } else {
      // 父进程代码
  }
  ```

### 2. exec()

- **用途**：在当前进程中加载和运行一个新程序。`exec()`系列函数有多种（如`execl()`, `execp()`, `execv()`等），区别在于如何指定程序和参数。
- **行为**：替换当前进程的映像，包括代码和数据。成功调用后不返回。
- **示例**：
  ```c
  if (fork() == 0) {
      execl("/bin/ls", "ls", "-l", (char *)NULL);
      // 如果exec()失败
      perror("exec failed");
      exit(EXIT_FAILURE);
  }
  ```

### 3. wait() / waitpid()

- **用途**：等待子进程结束。
- **行为**：`wait()`挂起当前进程，直到一个子进程结束。`waitpid()`可以等待特定的子进程，并提供更多的控制。
- **示例**：
  ```c
  pid_t pid = fork();
  if (pid == -1) {
      // 错误处理
  } else if (pid == 0) {
      // 子进程代码
  } else {
      int status;
      waitpid(pid, &status, 0); // 等待特定的子进程
      // 检查子进程的退出状态
  }
  ```

### 4. exit()

- **用途**：结束进程执行。
- **行为**：终止当前进程，返回一个状态码给父进程。
- **示例**：
  ```c
  if (fork() == 0) {
      // 子进程代码
      exit(0); // 成功退出
  }
  ```

### 5. getpid() / getppid()

- **用途**：`getpid()`用于获取当前进程的PID，`getppid()`用于获取父进程的PID。
- **示例**：
  ```c
  pid_t my_pid = getpid();
  pid_t parent_pid = getppid();
  ```

这些系统调用是Unix/Linux进程管理的基石，它们为操作系统提供了丰富的进程控制功能，从而使得Unix/Linux成为强大的多任务操作系统。
在使用这些调用时，理解它们的行为和可能的陷阱（比如正确处理`fork()`的返回值，以及`exec()`成功时不返回）是非常重要的。

====================================================================================================

## 获取进程ID
每个进程都有一个唯一的正数（非零）进程 ID（PID）。
`getpid` 函数返回调用进程的 PID。getppid 函数返回它的父进程的 PID（创建调用进程的进程）。

~~~c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
pid_t getppid(void);

// 返回：调用者或其父进程的 PID。
~~~

`getpid` 和 `getppid` 函数返回一个类型为 `pid_t` 的整数值，在 Linux 系统上它在 `types.h` 中被定义为 `int`。

The function `getpid(void)` is a standard function in many Unix-like operating systems. 
Its purpose is straightforward: it returns the process ID (PID) of the calling process. 
The function is defined in the POSIX standard, which is followed by Unix, Linux, and other Unix-like operating systems.

Here's a bit more detail about it:

- **Return Type:** The function returns a value of type `pid_t`. This is typically defined as an integer or a long integer. The `pid_t` data type is used for representing process IDs.

- **Function Prototype:** The function is declared as `pid_t getpid(void)`. It takes no arguments, as indicated by the `void` in the parentheses.

- **Usage:** When a process calls `getpid()`, it receives its own process ID in return. This is useful in various programming scenarios, especially where processes need to identify themselves, like in logging, process management, generating unique identifiers based on process IDs, etc.

- **Uniqueness of PID:** The process ID is unique for each process. No two running processes can have the same PID at the same time. However, PIDs can be reused by the operating system once a process terminates.

- **Header File:** To use `getpid()`, you typically need to include the header file `<unistd.h>` in your program.

An example of its use in a C program might look like this:

```c
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid;
    pid = getpid();
    printf("The Process ID is %d\n", pid);
    return 0;
}
```

In this example, `getpid()` retrieves the process ID of the current process, and it's then printed to the standard output.


## 创建和终止进程
从*程序员*的角度，我们可以认为进程总是处于下面三种状态之一：
 - **运行**进程要么在 CPU 上 执行，要么在等待被执行且最终会被内核调度。

 - **停止**进程的执行被挂起（suspended），且不会被调度。当收到 `SIGSTOP`、`SIGTSTP`、`SIGTTIN` 或者 `SIGTTOU` 信号时，进程就停止，并且保持停止直到它收到一个 `SIGCONT` 信号，在这个时刻，进程再次开始运行。（信号是一种软件中断的形式，将在 8.5 节中详细描述。）
 
 - **终止**进程永远地停止了。进程会因为三种原因终止：
    1）收到一个信号，该信号的默认行为是终止进程；
    2）从主程序返回；
    3）调用 `exit` 函数。
  
~~~c
#include <stdlib.h>

void exit(int status);

// 该函数不返回。
~~~

`exit` 函数以 `status` 退出状态来终止进程（另一种设置退出状态的方法是从主程序中返回一个整数值）。

The `exit(int status)` function is a standard C library function used to terminate a program execution. 
When this function is called, it performs several clean-up operations before ending the program. 
Here's a detailed overview:

- **Function Prototype:** The function is declared as `void exit(int status)`. This means it does not return a value (`void`) and takes a single integer argument `status`.

- **Argument - `status`:** 
This integer is used by the program to indicate the exit status. 
By convention, a status of `0` or `EXIT_SUCCESS` (a macro defined in `stdlib.h`) indicates success, while any non-zero value (often `EXIT_FAILURE`, also defined in `stdlib.h`) indicates a failure or an error. The exact meaning of non-zero status codes can be defined by the program.

- **Behavior:** When `exit` is called, it performs the following operations:
  - It terminates the process.
  - It closes all open file descriptors.
  - It flushes all open output streams (like standard output and standard error).
  - It executes any functions registered with `atexit()` and `_Exit()` functions.
  - It returns the status value to the operating system, which can be used by other processes, especially the one that invoked the exiting program (often a shell or a parent process).

- **Usage:** The `exit` function is used when a program needs to terminate immediately, often due to an error condition or after the completion of the program's main tasks. 
It ensures a clean exit, with proper closure of resources.

- **Header File:** To use `exit`, you need to include the header file `<stdlib.h>` in your program.

Here's a simple example of its usage in a C program:

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Program is starting...\n");
    
    // Some code here

    printf("Program is exiting...\n");
    exit(EXIT_SUCCESS); // Successful exit
    // exit(EXIT_FAILURE); // Use this for an unsuccessful exit
}
```

In this example, when `exit(EXIT_SUCCESS)` is called, the program will perform the aforementioned cleanup operations and then terminate, returning the status `EXIT_SUCCESS` to the operating system.

父进程通过调用 fork 函数创建一个新的运行的子进程。

~~~c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);

// 返回：子进程返回 0，父进程返回子进程的 PID，如果出错，则为 -1。
~~~

新创建的子进程几乎但不完全与父进程相同。
子进程得到与父进程用户级虚拟地址空间相同的（但是独立的）一份副本，包括代码和数据段、堆、共享库以及用户栈。
子进程还获得与父进程任何打开文件描述符相同的副本，这就意味着当父进程调用 `fork` 时，子进程可以读写父进程中打开的任何文件。
父进程和新创建的子进程之间最大的区别在于它们有不同的 `PID`。

`fork` 函数是有趣的（也常常令人迷惑），因为它只被调用一次，却会返回两次：一次是在调用进程（父进程）中，一次是在新创建的子进程中。
在父进程中，fork 返回子进程的 PID。在子进程中，fork 返回 0。因为子进程的 PID 总是为非零，返回值就提供一个明确的方法来分辨程序是在父进程还是在子进程中执行。

一个使用 fork 创建子进程的父进程的示例。
当 fork 调用在第 6 行返回时，在父进程和子进程中 x 的值都为 1。子进程在第 8 行加一并输出它的 x 的副本。相似地，父进程在第 13 行减一并输出它的 x 的副本。

~~~c
int main()
{
    pid_t pid;
    int x = 1;

    pid = Fork();
    if (pid == 0) { /* Child */
        printf("child : x=%d\n", ++x);
        exit(0);
    }
    
    /* Parent */
    printf("parent: x=%d\n", --x);
    exit(0);
}
~~~
当在 Unix 系统上运行这个程序时，我们得到下面的结果：
~~~shell
linux> ./fork
parent：x=0
child ：x=2
~~~
这个简单的例子有一些微妙的方面。

 - *调用一次*，返回两次。fork 函数被父进程调用一次，但是却返回两次。
 一次是返回到父进程，一次是返回到新创建的子进程。对于只创建一个子进程的程序来说，这还是相当简单直接的。
 但是具有多个 fork 实例的程序可能就会令人迷惑，需要仔细地推敲了。

 - *并发执行*。父进程和子进程是并发运行的独立进程。
 内核能够以任意方式交替执行它们的逻辑控制流中的指令。
 在我们的系统上运行这个程序时，父进程先完成它的 printf 语句，然后是子进程。然而，在另一个系统上可能正好相反。
 一般而言，作为程序员，我们决不能对不同进程中指令的交替执行做任何假设。

 - *相同但是独立的地址空间*。
 如果能够在 fork 函数在父进程和子进程中返回后立即暂停这两个进程，我们会看到两个进程的地址空间都是相同的。
 每个进程有相同的用户栈、相同的本地变量值、相同的堆、相同的全局变量值，以及相同的代码。
 因此，在我们的示例程序中，当 fork 函数在第 6 行返回时，本地变量 x 在父进程和子进程中都为 1。
 然而，因为父进程和子进程是独立的进程，它们都有自己的私有地址空间(COW 写时复制)。
 后面，父进程和子进程对 x 所做的任何改变都是独立的，不会反映在另一个进程的内存中。
 这就是为什么当父进程和子进程调用它们各自的 `printf` 语句时，它们中的变量 x 会有不同的值。

- *共享文件*。当运行这个示例程序时，我们注意到父进程和子进程都把它们的输出显示在屏幕上。
原因是子进程继承了父进程所有的打开文件。当父进程调用 fork 时，stdout 文件是打开的，并指向屏幕。
子进程继承了这个文件，因此它的输出也是指向屏幕的。


```
+-------------------+
| Parent Process    |
| PID = X           |
|                   |
|   fork()----------+-----------------+
|                   |                 |
|                   |                 |
|                   | +---------------v-------------+
|                   | | Child Process                |
|                   | | PID = Y (returned by fork()) |
|                   | |                              |
|                   | +------------------------------+
+-------------------+
```

在这个ASCII图中：
- "Parent Process" 表示父进程，它执行了 `fork()` 函数。
- 在 `fork()` 被调用之后，创建了一个 "Child Process"。
- 父进程中的 "PID = X" 表示父进程的PID。
- 子进程中的 "PID = Y (returned by fork())" 表示子进程的PID，这是由 `fork()` 函数返回给父进程的值。
- 连接线表示父进程和子进程之间的关系，以及 `fork()` 函数的调用。

请注意，这个图示是为了简化说明进程关系而设计的，实际的进程执行可能涉及更多细节。


The `fork(void)` function is a crucial part of process management in Unix-like operating systems, such as Linux and BSD. Its primary purpose is to create a new process, which is called a child process. Here are the key aspects of the `fork` function:

- **Function Prototype:** The function is declared as `pid_t fork(void)`. 
It doesn't take any arguments and returns a value of type `pid_t`, which is used to represent process IDs.

- **Return Values:** 
  - **In the Parent Process:** `fork()` returns the process ID (PID) of the newly created child process.
  - **In the Child Process:** `fork()` returns `0`.
  - **On Failure:** If the fork fails, typically due to insufficient resources, it returns `-1`.

- **Behavior:** When `fork()` is called, it creates a new process by duplicating the calling (parent) process. The new process is a copy of the parent process but has its unique process ID. Both processes will then execute the next instruction following the `fork()` call.

- **Memory Space:** The child process gets a separate memory space. Initially, this memory space is a duplicate of the parent's memory space, but as either process modifies its memory, the changes are not reflected in the other process. This is known as "copy-on-write" behavior.

- **Usage:** `fork()` is used in various scenarios, like creating a new process to execute another program (in conjunction with `exec` family functions), implementing parallel processing, handling client requests in servers, etc.

- **Header File:** To use `fork()`, you typically need to include the header file `<unistd.h>` in your program.

Here's a simple example to illustrate its usage in a C program:

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        // This is the child process
        printf("Child process with PID %d\n", getpid());
    } else {
        // This is the parent process
        printf("Parent process with PID %d, Child PID %d\n", getpid(), pid);
    }

    return 0;
}
```

In this program, after the `fork()` call, there are two processes running concurrently: the parent and the child. The `if-else` block differentiates between them based on the return value of `fork()`.

如果你是第一次学习 fork 函数，画进程图通常会有所帮助，进程图是刻画程序语句的偏序的一种简单的前趋图。
每个顶点 a 对应于一条程序语句的执行。有向边 `a → b` 表示语句 a 发生在语句 b 之前。
边上可以标记出一些信息，例如一个变量的当前值。对应于 printf 语句的顶点可以标记上 printf 的输出。

每张图从一个顶点开始，对应于调用 main 的父进程。这个顶点没有入边，并且只有一个出边。每个进程的顶点序列结束于一个对应于 exit 调用的顶点。这个顶点只有一条入边，没有出边。

例如，图 8-16 展示了图 8-15 中示例程序的进程图。初始时，父进程将变量 x 设置为 1。
父进程调用 fork，创建一个子进程，它在自己的私有地址空间中与父进程并发执行。

        +-----> 0 -----> exit (Child Precess)
        |
0 ----> 0-----> 0 -----> exit (Parent Process)
main    fork

对于运行在单处理器上的程序，对应进程图中所有顶点的拓扑排序（`topological sort`）表示程序中语句的一个可行的全序排列。
下面是一个理解拓扑排序概念的简单方法：给定进程图中顶点的一个排列，把顶点序列从左到右写成一行，然后画出每条有向边。排列是一个拓扑排序，当且仅当画出的每条边的方向都是从左往右的。
因此，在图 8-15 的示例程序中，父进程和子进程的 `printf` 语句可以以任意先后顺序执行，因为每种顺序都对应于图顶点的某种拓扑排序。

为了理解这段描述中提到的拓扑排序（topological sort）和它如何适用于程序中语句的执行顺序，首先需要明确几个概念：

1. **拓扑排序：** 在有向图中，拓扑排序是一种顶点的排列方式，使得对于图中的每一条有向边 \( U \rightarrow V \)，顶点 \( U \) 都在顶点 \( V \) 之前。这种排序只有在图是有向无环图（DAG）的情况下才有可能。

2. **单处理器上的程序执行：** 在单核处理器上运行的程序，其语句在任意时刻只能按照某一特定的顺序执行。

3. **进程图：** 在您提到的上下文中，进程图可能表示程序的不同部分（如函数、语句等）及其依赖关系。

将这些概念应用到您的问题中，描述的意思是：

- 程序中的语句或操作可以被视为进程图中的顶点。
- 这些语句之间的依赖关系或执行顺序被视为有向边。
- 在单处理器系统中，程序的执行可以被视为对这个图的一种拓扑排序。
- 拓扑排序的一个关键特性是：如果存在一条从顶点 \( A \) 到顶点 \( B \) 的边（即 \( A \) 必须在 \( B \) 之前执行），那么在排序中 \( A \) 必定出现在 \( B \) 之前。
- 如果程序的不同部分之间没有依赖关系（即没有有向边连接它们），那么这些部分可以以任何顺序执行。在您的示例中，父进程和子进程的 `printf` 语句之间没有直接的依赖关系，所以它们可以按任意顺序执行。

总的来说，这段描述是在说：在一个单处理器系统上，程序中可以并行执行的部分（如父进程和子进程中的代码）可以在任何顺序中执行，因为这些执行顺序都对应于进程图顶点的某种拓扑排序。这是因为在并行部分之间没有顺序依赖（即没有从一个部分到另一个部分的有向边）。


进程图特别有助于理解带有嵌套 `fork` 调用的程序。
例如，图 8-17 中的程序源码中两次调用了 fork。对应的进程图可帮助我们看清这个程序运行了四个进程，每个都调用了—次 printf，这些 printf 可以以任意顺序执行。

~~~c
int main() {
  Fork();
  Fork();
  printf("hello\n");
  exit(0)
}
~~~

====================================================================================================
`Fork Bomb`

A fork bomb is a type of denial-of-service (DoS) attack against a computer system which uses the `fork` system call (or its equivalent in different operating systems) to rapidly create new processes in an attempt to exhaust the system's resources. 
Fork bombs operate by self-replicating, rapidly spawning copies of themselves to fill the system's process table, consuming CPU time and memory, and potentially leading to system slowdown or crash.

Here's how a fork bomb typically works:

1. **Self-Replication:** The process created by a fork bomb immediately creates two or more copies of itself. Each of these new processes does the same, leading to an exponential increase in the number of processes.

2. **Resource Depletion:** As the number of processes increases very quickly, the system's resources (like CPU, memory, and process table entries) become fully utilized. This can lead to the inability to start new processes (even essential system processes), and can severely degrade system performance or cause the system to crash.

3. **Simple yet Effective:** Fork bombs are notoriously simple to implement. For example, in Unix-like systems, a fork bomb can be as simple as a shell script with just a few characters. For instance, the following is a classic example of a shell-based fork bomb in Unix/Linux:
   ```shell
   :(){ :|:& };:
   ```
   In this example, a function named `:` is defined, which, when called, starts two copies of itself in the background (`:|:&`). The function is then invoked (`;:`), starting the exponential growth of processes.

4. **Difficult to Stop:** Once a fork bomb has been launched, it can be difficult to stop because it quickly consumes all available system resources, making it hard to run commands or tools to kill the processes. Restarting the affected system is often the fastest way to recover.

5. **Prevention and Mitigation:** To prevent fork bombs, system administrators can set limits on the number of processes that a user can create (using mechanisms like `ulimit` in Unix/Linux). Monitoring tools can also be used to detect unusual process activity and respond accordingly.

Fork bombs are a form of malicious attack and should not be executed on any system without explicit permission, as they can cause significant disruption and damage.

“Fork炸弹”是一种针对计算机系统的拒绝服务（DoS）攻击方式，它利用系统调用 `fork`（或不同操作系统中的等效功能）快速创建新进程，目的是耗尽系统资源。Fork炸弹通过自我复制，迅速生成自身的副本以填满系统的进程表，消耗CPU时间和内存，可能导致系统运行缓慢或崩溃。

下面是fork炸弹的典型工作原理：

1. **自我复制：** Fork炸弹创建的进程会立即创建两个或更多的自身副本。这些新进程同样进行复制，导致进程数量呈指数级增长。

2. **资源耗尽：** 随着进程数量迅速增加，系统的资源（如CPU、内存和进程表条目）将被完全占用。这可能导致无法启动新进程（甚至包括关键的系统进程），严重降低系统性能或导致系统崩溃。

3. **简单却有效：** Fork炸弹因其实现简单而臭名昭著。例如，在类Unix系统中，fork炸弹可以是一个只有几个字符的shell脚本。例如，以下是Unix/Linux中的一个经典的shell fork炸弹示例：
   ```
   :(){ :|:& };:
   ```
   在这个例子中，定义了一个名为 `:` 的函数，当调用时，它会在后台启动自己的两个副本（`:|:&`）。然后调用这个函数（`;:`），开始指数级增长的进程。

4. **难以停止：** 一旦fork炸弹启动，因为它很快消耗了所有可用的系统资源，使得运行命令或工具来杀死这些进程变得困难，因此它很难停止。重启受影响的系统通常是恢复的最快方法。

5. **预防和缓解：** 为了预防fork炸弹，系统管理员可以设置用户可以创建的进程数量的限制（使用类似Unix/Linux中的 `ulimit` 机制）。监控工具也可以用来检测异常的进程活动并相应地做出反应。

Fork炸弹是一种恶意攻击方式，未经明确许可，不应在任何系统上执行，因为它可能造成严重的干扰和损害。

这个Shell脚本是一个典型的fork炸弹示例。它的结构非常简单，但它可以迅速耗尽系统资源。下面是对这个脚本的逐部分解释：

1. **函数定义：** `:(){ ... };` 这部分是函数的定义。在Shell中，`:` 是一个有效的函数名。这里定义了一个名为 `:` 的函数。花括号 `{}` 内是函数的内容。

2. **函数内容：** `:|:&` 这是函数 `:` 的主体部分，它包含了函数的操作。这里，函数 `:` 做了两件事情：
   - `:|:` 是一个管道（pipe）。它执行了两次函数 `:`，一次是管道的左边，另一次是右边。这意味着每次函数 `:` 被调用时，它就会启动两个新的 `:` 实例。
   - `&` 将管道操作放入后台执行。这意味着函数 `:` 不会等待这两个新实例完成，而是会立即返回并继续执行。

3. **函数执行：** 最后的 `:` 是对定义好的函数的首次调用。这启动了整个过程。

把这些部分放在一起，脚本的执行流程如下：

- 当脚本运行时，它首先定义了一个名为 `:` 的函数，该函数创建两个自己的后台副本。
- 然后，脚本执行这个函数。每次函数被调用，它就会创建更多的副本。
- 因为这些函数调用是在后台执行的，它们不会阻塞其他调用的执行。
- 结果就是进程数量呈指数级增长，非常快速地耗尽系统资源。

由于fork炸弹能够迅速使系统变得不响应，甚至崩溃，因此它是一种危险的操作，不应该在任何实际环境中使用。这个脚本通常用于教育和演示目的，以展示简单的Shell命令如何造成重大影响。
====================================================================================================


~~~c
int main()
{
    int x = 1;
    
    if (Fork() == 0)
        printf("p1: x=%d\n", ++x);
    printf("p2: x=%d\n", --x);
    exit(0);
}
~~~

~~~shell
❯ ./fork 
p2: x=0
p1: x=2
p2: x=1
~~~

# 回收子进程

当一个进程由于某种原因终止时，内核并不是立即把它从系统中清除。
相反，进程被保持在一种已终止的状态中，直到被它的父进程回收（`reaped`）。
当父进程回收已终止的子进程时，内核将子进程的退出状态传递给父进程，然后抛弃已终止的进程，从此时开始，该进程就不存在了。
一个终止了但还未被回收的进程称为僵死进程（`zombie`）。

> 旁注 - 为什么已终止的子进程被称为僵死进程
> 在民间传说中，僵尸是活着的尸体，一种半生半死的实体。僵死进程已经终止了
> 而内核仍保留着它的某些状态直到父进程回收它为止，从这个意义上说它们是类似的。

1. **进程终止：** 当一个进程结束其执行时（可能是正常结束，也可能是因为错误或其他原因被迫终止），它并不立即从系统中完全消失。
此时，进程完成了其执行，但它在系统中保留了一些信息，如退出状态码、运行时间等。这些信息对父进程来说可能是重要的。

2. **僵死进程（Zombie Process）：** 这样一个已终止但尚未被其父进程“回收”（或“清理”）的进程被称为僵死进程。
僵死进程已经释放了大部分资源，如内存和打开的文件，但它在进程表中仍占有一个条目，其中包含一些基本信息，如退出状态。

3. **回收（Reaping）过程：** 父进程通过调用 `wait()` 或 `waitpid()` 系统调用来“回收”已终止的子进程。
这些调用使父进程可以获得子进程的退出状态，并允许操作系统清除僵死进程，释放其在进程表中占用的条目。
只有当父进程执行了这一步骤，僵死进程才会完全消失。

4. **如果父进程不回收：** 如果父进程忽略其子进程的终止或因其他原因未能调用 `wait()` 或 `waitpid()`，那么子进程将保持为僵死状态。
这可能导致系统中僵死进程的累积，占用进程表空间。

5. **父进程终止：** 如果父进程在子进程终止之前自己终止了，子进程的“收养”（adoption）通常由 `init` 进程（或在一些现代系统中的其他等效进程，如 `systemd`）接管。`init` 进程将定期回收任何僵死的子进程，防止僵死进程的长期累积。

总的来说，僵死进程和其回收机制是操作系统有效管理资源和进程生命周期的一部分。正确地管理子进程的终止和回收对于维护系统的健康和避免资源泄露至关重要。


如果一个父进程终止了，内核会安排 `init` 进程成为它的孤儿进程的养父。
`init` 进程的 PID 为 1，是在系统启动时由内核创建的，它不会终止，是所有进程的祖先。
如果父进程没有回收它的僵死子进程就终止了，那么内核会安排 init 进程去回收它们。
不过，长时间运行的程序，比如 shell 或者服务器，总是应该回收它们的僵死子进程。即使僵死子进程没有运行，它们仍然消耗系统的内存资源。

~~~shell
❯ pstree
systemd─┬─ModemManager───2*[{ModemManager}]
        ├─NetworkManager───2*[{NetworkManager}]
        ├─accounts-daemon───2*[{accounts-daemon}]
        ├─acpid
        ├─avahi-daemon───avahi-daemon
        ├─colord───2*[{colord}]
        ├─containerd───12*[{containerd}]
        ├─3*[cpptools-srv───13*[{cpptools-srv}]]
        ├─cron
        ├─cups-browsed───2*[{cups-browsed}]
        ├─cupsd
        ├─dbus-daemon
~~~

让我们通过一个具体的例子来阐释僵尻进程的产生和处理。在这个例子中，我们将使用C语言编写一个简单的程序，其中包含一个父进程创建一个子进程，然后子进程终止，但父进程不立即回收它，导致子进程成为僵尸进程。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // 创建子进程
    pid = fork();

    if (pid == -1) {
        // fork失败
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        // 子进程
        printf("Child process: PID = %d\n", getpid());
        // 子进程执行完毕并退出
        exit(0);
    } else {
        // 父进程
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);

        // 父进程故意“忙碌”一段时间，不立即回收子进程
        sleep(10);

        // 父进程现在回收子进程
        waitpid(pid, NULL, 0);
        printf("Child process has been reaped\n");
    }

    return 0;
}
```

这个程序的流程如下：

1. **创建子进程：** 程序首先调用 `fork()` 创建一个子进程。

2. **子进程操作：** 如果 `fork()` 成功，子进程将打印其PID并退出。退出时，子进程会变成僵尸进程，因为它需要父进程来读取它的退出状态。

3. **父进程操作：** 同时，父进程将打印自己和子进程的PID，并执行 `sleep(10)`。这代表父进程在做其他工作，暂时不处理子进程的终止。

4. **子进程变成僵尸：** 在这个10秒的睡眠期间，子进程已经终止，但因为父进程没有调用 `waitpid()`，所以子进程的状态信息仍然保存在系统中，成为僵尸进程。

5. **回收子进程：** 睡眠结束后，父进程通过调用 `waitpid()` 回收子进程，清理子进程的僵尸状态。

在这个示例中，如果您在终端运行该程序，并在子进程退出后、父进程调用 `waitpid()` 之前查看进程状态（例如，使用 `ps` 命令），您将能看到一个僵尸进程。这个僵尸进程将在父进程执行 `waitpid()` 后消失。

一个进程可以通过调用 `waitpid` 函数来等待它的子进程终止或者停止。

~~~c
#include <sys/types.h>
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *statusp, int options);

// 返回：如果成功，则为子进程的 PID，如果 WNOHANG，则为 0，如果其他错误，则为 -1。
~~~

waitpid 函数有点复杂。默认情况下（当 `options=0` 时），waitpid 挂起调用进程的执行，直到它的等待集合（wait set）中的一个子进程终止。如果等待集合中的一个进程在刚调用的时刻就已经终止了，那么 waitpid 就立即返回。在这两种情况中，waitpid 返回导致 waitpid 返回的已终止子进程的 PID。此时，已终止的子进程已经被回收，内核会从系统中删除掉它的所有痕迹。

====================================================================================================

`waitpid` 函数在 `options=0`（即默认情况下）的行为:

1. **挂起调用进程：** 当一个父进程调用 `waitpid` 并且 `options` 参数设置为 `0` 时，这个调用会挂起（即阻塞）调用进程的执行，直到有一个条件得到满足。这个挂起是为了等待父进程的一个或多个子进程发生状态变化（通常是指子进程的终止）。

2. **等待集合（Wait Set）：** 等待集合是由 `waitpid` 的 `pid` 参数指定的一组子进程。这个集合取决于 `pid` 参数的值，如之前所述。

3. **子进程已经终止：** 如果等待集合中的一个子进程在 `waitpid` 被调用的时刻已经终止，`waitpid` 会立即返回。这意味着父进程不需要等待，因为它要等待的事件（子进程的终止）已经发生。

4. **返回值：** 在这两种情况中（子进程终止导致 `waitpid` 返回或子进程在调用时已经终止），`waitpid` 返回的是导致它返回的已终止子进程的 PID。

5. **子进程的回收：** 一旦 `waitpid` 返回，这表示已经终止的子进程被回收。回收过程包括释放该子进程占用的所有资源，如内存、文件描述符等，并清除系统中关于该子进程的所有记录。这就是为什么终止的子进程称为“僵尸进程”直到它们被 `waitpid`（或类似的函数）回收。

通过这种机制，`waitpid` 允许父进程控制对子进程终止的响应，并确保子进程资源的正确释放，从而避免了系统资源的浪费或泄漏。

====================================================================================================

`waitpid` 是一个在类 Unix 系统（如 Linux）中常用的系统调用，用于让一个进程等待其子进程的状态变化（通常是结束）。这是 `wait` 函数的一个更通用的变体。以下是 `waitpid` 函数的详细介绍：

1. **函数原型**：
   ```c
   pid_t waitpid(pid_t pid, int *statusp, int options);
   ```
   其中，`pid_t` 是用于进程ID的数据类型。

2. **参数**：
   - **pid**：指定要等待的子进程。它的不同值有不同的含义：
     - `pid > 0`：等待特定的子进程（其进程ID等于 `pid`）。
     - `pid == 0`：等待任何子进程，其进程组ID等于调用进程的进程组ID。
     - `pid < -1`：等待任何子进程，其进程组ID等于 `pid` 的绝对值。
     - `pid == -1`：等待任何子进程，等效于 `wait`。
   - **statusp**：一个指向整数的指针，用于存储子进程的退出状态。
   当子进程结束时，它的退出状态会存储在这个位置。可以使用一系列的宏来解析这个状态（例如 `WIFEXITED` 和 `WEXITSTATUS`）。
   - **options**：用于改变 `waitpid` 的行为。常见的选项包括：
     - `WNOHANG`：如果没有子进程退出，立即返回，而不是阻塞。
     - `WUNTRACED` 和 `WCONTINUED`：分别用于报告子进程的停止（例如，因为信号而停止）和继续（例如，由 `SIGCONT` 信号继续）状态。

3. **返回值**：
   - 如果成功，则返回收集到状态的子进程的PID。
   - 如果设置了 `WNOHANG` 且没有子进程退出，返回 `0`。
   - 如果出错，返回 `-1`，错误原因存储在 `errno` 中。

4. **用途**：
   `waitpid` 通常用于父进程监控其一个或多个子进程的状态。
   通过这种方式，父进程可以获取子进程的退出状态，并做出相应的处理，如释放资源、继续执行等。

5. **例子**：
   ```c
   pid_t pid;
   int status;

   pid = fork();
   if (pid == 0) {
       // 子进程
       exit(0);  // 子进程退出
   } else {
       // 父进程
       waitpid(pid, &status, 0); // 等待子进程退出
       if (WIFEXITED(status)) {
           printf("子进程正常结束，退出码：%d\n", WEXITSTATUS(status));
       }
   }
   ```

`waitpid` 是处理并发程序中的进程间协作和同步的重要工具。
它可以确保父进程能够正确处理子进程的终止，并根据子进程的状态做出适当的响应。

====================================================================================================


### 1. 判定等待集合的成员
等待集合的成员是由参数 pid 来确定的：
 1. 如果 `Pid>0`，那么等待集合就是一个单独的子进程，它的进程 ID 等于 pid。
 2. 如果 `Pid=-1`，那么等待集合就是由父进程所有的子进程组成的。
`waitpid` 函数还支持其他类型的等待集合，包括 Unix 进程组，对此我们将不做讨论。

`waitpid` 函数比 `wait` 函数更加强大和灵活，因为它可以等待特定的子进程或者一组子进程。这里是 `waitpid` 函数的等待集合成员确定方式的详细解释：

1. **Pid > 0：** 当 `pid` 参数的值大于 0 时，等待集合包含一个单独的子进程，其进程 ID 等于 `pid` 的值。这意味着 `waitpid` 仅等待一个特定的子进程。

2. **Pid = -1：** 当 `pid` 等于 -1 时，等待集合包括调用进程的所有子进程。这使 `waitpid` 的行为类似于 `wait` 函数，等待任何一个子进程改变状态。

除了这些，`waitpid` 还支持基于 Unix 进程组的等待集合：

3. **Pid = 0：** 当 `pid` 等于 0 时，等待集合包括调用进程所在进程组的所有子进程。
这意味着 `waitpid` 会等待与调用进程在同一进程组中的所有子进程。

4. **Pid < -1：** 当 `pid` 小于 -1 时，等待集合包括进程组 ID 等于 `pid` 绝对值的所有子进程。
这允许 `waitpid` 等待一个特定进程组中的子进程。

通过这种方式，`waitpid` 提供了强大的控制，允许父进程更精细地管理其子进程的状态变化，无论是等待特定的子进程还是管理一组子进程。
这在编写需要精确控制子进程行为的并发程序时非常有用。

### 2. 修改默认行为
  ```c
  pid_t waitpid(pid_t pid, int *statusp, int options);
  ```
可以通过将 `options` 设置为常量 `WNOHANG`，`WUNTRACED` 和 `WCONTINUED` 的各种组合来修改默认行为：
 - `WNOHANG`：如果等待集合中的任何子进程都还没有终止，那么就立即返回（返回值为 0）。
 默认的行为是挂起调用进程，直到有子进程终止。在等待子进程终止的同时，如果还想做些有用的工作，这个选项会有用。

 - `WUNTRACED`：挂起调用进程的执行，直到等待集合中的一个进程变成已终止或者被停止，返回的 PID 为导致返回的已终止或被停止子进程的 PID。默认的行为是只返回已终止的子进程。当你想要检査已终止和被停止的子进程时，这个选项会有用。
 
 - `WCONTINUED`：挂起调用进程的执行，直到等待集合中一个正在运行的进程终止或等待集合中一个被停止的进程收到 `SIGCONT` 信号重新开始执行。（8.5 节会解释这些信号。）

您的描述很好地概述了 `waitpid` 函数中 `options` 参数的使用方法，以及如何通过设置不同的选项来修改 `waitpid` 的默认行为。这些选项允许调用进程根据特定的需求来调整它的等待行为。下面是关于这些选项的更详细解释：

1. **WNOHANG：**
   - **作用：** `WNOHANG` 选项用于使 `waitpid` 函数非阻塞。即，如果等待集合中的任何子进程都还没有终止，`waitpid` 会立即返回，而不是挂起（阻塞）调用进程。
   - **返回值：** 当设置了 `WNOHANG` 且没有子进程已终止时，`waitpid` 返回 `0`。
   - **使用场景：** 这个选项在那些需要继续执行而不被阻塞等待子进程终止的场景中非常有用。它允许父进程在等待子进程的同时继续执行其他任务。

2. **WUNTRACED：**
   - **作用：** `WUNTRACED` 选项扩展了 `waitpid` 的行为，使其除了等待终止的子进程外，还可以等待那些被信号停止（但未终止）的子进程。
   - **返回值：** 如果等待集合中的一个子进程被停止，且其状态自上次报告以来还未报告过，则返回该子进程的 PID。
   - **使用场景：** 当你想要监视子进程的运行状态，包括那些由于接收信号而暂时停止执行的子进程时，这个选项很有帮助。

3. **WCONTINUED：**
   - **作用：** `WCONTINUED` 选项使 `waitpid` 可以报告那些已经停止（由于接收到 `SIGSTOP`、`SIGTSTP`、`SIGTTIN` 或 `SIGTTOU` 信号）并且后来由于接收到 `SIGCONT` 信号而恢复执行的子进程的状态。
   - **返回值：** 如果等待集合中的一个已停止的子进程收到 `SIGCONT` 信号继续运行，则返回该子进程的 PID。
   - **使用场景：** 这个选项在处理那些可能会被停止和后续继续的子进程时非常有用。

通过使用这些选项，程序可以更加灵活地处理子进程的不同状态变化，从而更好地控制并发行为和响应系统事件。


 可以用或运算把这些选项组合起来。例如：
  `WNOHANG | WUNTRACED`：立即返回，如果等待集合中的子进程都没有被停止或终止，则返回值为 0；如果有一个停止或终止，则返回值为该子进程的 PID。

在使用 `waitpid` 函数时，可以通过逻辑或运算（`|`）组合多个选项，以实现更复杂的行为。
这种组合使用提供了更大的灵活性，允许您同时应对多种不同的子进程状态变化情况。
您给出的示例 `WNOHANG | WUNTRACED` 正是这种组合用法的一个例子：

- **组合 WNOHANG 和 WUNTRACED：**
  - **作用：** 使用 `WNOHANG | WUNTRACED` 作为 `options` 参数的值，`waitpid` 会立即返回，而不是挂起调用进程，即使没有子进程终止。同时，这个调用也会等待被停止的子进程，而不仅仅是终止的子进程。
  - **返回值：**
    - 如果等待集合中没有子进程终止或被停止，`waitpid` 返回 `0`。
    - 如果有子进程终止或被停止，`waitpid` 返回导致它返回的那个子进程的 PID。
  - **使用场景：** 这种组合适用于那些需要非阻塞地检测子进程终止或停止状态的情况。
  它允许父进程在不被挂起的情况下监控其子进程的状态变化。

通过这种方式，您可以更灵活地控制 `waitpid` 的行为，以适应不同的程序需求和逻辑。
例如，在某些并发程序设计中，可能需要在后台监控子进程的状态，同时执行其他任务，而不希望主进程因等待子进程的状态变化而被阻塞。在这种情况下，使用 `WNOHANG | WUNTRACED` 就非常合适。

当然可以。以下是一个使用 `waitpid` 函数并结合 `WNOHANG` 和 `WUNTRACED` 选项的 C 语言代码示例。这个例子中，父进程创建了一个子进程，然后在一个循环中使用 `waitpid` 检查子进程的状态，同时执行一些其他工作。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == -1) {
        // fork失败
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        // 子进程
        printf("Child process: PID = %d\n", getpid());
        // 模拟子进程执行一些操作
        sleep(5);
        exit(0);  // 子进程退出
    } else {
        // 父进程
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        while (1) {
            // 使用WNOHANG | WUNTRACED选项调用waitpid
            pid_t w = waitpid(pid, &status, WNOHANG | WUNTRACED);
            if (w == 0) {
                // 子进程还在运行
                printf("No change in child process, parent does some work...\n");
                sleep(1); // 父进程执行其他工作
            } else if (w == -1) {
                // 错误发生
                perror("waitpid");
                exit(EXIT_FAILURE);
            } else {
                // 子进程已终止或被停止
                if (WIFEXITED(status)) {
                    printf("Child exited, status=%d\n", WEXITSTATUS(status));
                } else if (WIFSTOPPED(status)) {
                    printf("Child stopped by signal %d\n", WSTOPSIG(status));
                }
                break; // 退出循环
            }
        }
    }

    return 0;
}
```

- 父进程使用 `fork()` 创建一个子进程。
- 子进程执行 `sleep(5)` 来模拟一些工作，然后退出。
- 父进程进入一个循环，在循环中调用 `waitpid`，使用 `WNOHANG | WUNTRACED` 选项。
  这意味着 `waitpid` 将不会阻塞父进程，即使子进程尚未改变状态。
- 如果 `waitpid` 返回 `0`，表示子进程还在运行，父进程会打印一条消息并执行其他工作。
- 如果 `waitpid` 返回子进程的 PID，父进程会检查子进程的状态，然后跳出循环。
- 这个例子展示了如何在等待子进程的同时，让父进程继续执行其他任务。

### 3. 检查已回收子进程的退出状态

  ```c
  pid_t waitpid(pid_t pid, int *statusp, int options);
  ```

如果 `statusp` 参数是非空的，那么 waitpid 就会在 `status` 中放上关于导致返回的子进程的状态信息，`status` 是 `statusp` 指向的值。wait.h 头文件定义了解释 status 参数的几个宏：

1. **WIFEXITED(status)：**
   - **作用：** 检查子进程是否正常终止（即通过调用 `exit` 或正常返回）。
   - **返回值：** 如果子进程正常终止，返回真（非零）；否则返回假（零）。

2. **WEXITSTATUS(status)：**
   - **作用：** 如果 `WIFEXITED` 为真，该宏可以获取子进程的退出状态码。
   - **返回值：** 返回子进程的退出状态码。

3. **WIFSIGNALED(status)：**
   - **作用：** 检查子进程是否因为接收到未被捕获的信号而异常终止。
   - **返回值：** 如果子进程因信号而终止，返回真；否则返回假。

4. **WTERMSIG(status)：**
   - **作用：** 如果 `WIFSIGNALED` 为真，该宏可以获取导致子进程终止的信号编号。
   - **返回值：** 返回导致子进程终止的信号编号。

5. **WIFSTOPPED(status)：**
   - **作用：** 检查子进程是否当前处于停止状态。
   - **返回值：** 如果子进程当前被停止，返回真；否则返回假。

6. **WSTOPSIG(status)：**
   - **作用：** 如果 `WIFSTOPPED` 为真，该宏可以获取导致子进程停止的信号编号。
   - **返回值：** 返回导致子进程停止的信号编号。

7. **WIFCONTINUED(status)：**
   - **作用：** 检查子进程是否因为接收到 `SIGCONT` 信号而从停止状态中继续运行。
   - **返回值：** 如果子进程因接收 `SIGCONT` 信号而重新启动，返回真；否则返回假。

使用这些宏可以让父进程精确地了解子进程的终止状态和原因，从而做出适当的响应。这是处理并发编程和进程间通信时非常重要的一部分。

当然可以。以下是一个使用 `waitpid` 和状态宏来获取和解释子进程退出状态的 C 语言示例。在这个例子中，父进程创建了一个子进程。子进程可能正常退出，也可能因为信号而异常终止。父进程使用 `waitpid` 来等待子进程的状态改变，并根据退出状态做出相应处理。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == -1) {
        // fork失败
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        // 子进程
        printf("Child process: PID = %d\n", getpid());

        // 根据需要修改这里以模拟不同的退出情况
        // 正常退出
        // exit(0);

        // 通过信号终止
        abort();  // 生成 SIGABRT 信号

    } else {
        // 父进程
        printf("Parent process: PID = %d, Child PID = %d\n", getpid(), pid);
        waitpid(pid, &status, 0); // 等待子进程状态改变

        if (WIFEXITED(status)) {
            printf("Child exited normally with exit status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child terminated by signal %d\n", WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("Child stopped by signal %d\n", WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("Child continued\n");
        }
    }

    return 0;
}
```

在这个示例中：
- 父进程使用 `fork()` 创建了一个子进程。
- 子进程可以选择正常退出（使用 `exit(0)`）或通过产生 `SIGABRT` 信号来异常终止（使用 `abort()`）。
- 父进程使用 `waitpid` 等待子进程状态的改变，并检查 `status` 参数的值来确定子进程是如何终止的。
- 父进程使用 `WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`, `WTERMSIG`, `WIFSTOPPED`, `WSTOPSIG` 和 `WIFCONTINUED` 宏来解析 `status` 的值，并打印相应的信息。

这个例子展示了如何在实际程序中使用 `waitpid` 和相关宏来处理和解释子进程的不同退出情况。

~~~shell
[czy@czy-307-thinkcentre-m720q-n000] ~/new_space/Exceptional_Control_Flow/04 (master) ⚡ 
❯ ./waitpid               
Parent process: PID = 105321, Child PID = 105322
Child process: PID = 105322
Child terminated by signal 6
~~~

### 检查已回收子进程的退出状态的作用

在操作系统中，当一个子进程结束其执行时，父进程需要知道这个子进程是如何终止的。这对于父进程来说非常重要，因为：

1. **决定后续操作：** 父进程可能会根据子进程的退出状态来决定接下来要执行的操作。
例如，如果子进程因错误而异常终止，父进程可能需要记录错误、重启子进程或采取其他修复措施。

2. **资源管理：** 正确地了解子进程的终止状态有助于父进程管理系统资源。
例如，确保所有由子进程分配的资源（如内存、文件句柄等）得到适当释放。

3. **错误追踪和日志记录：** 对于调试和日志记录来说，了解子进程的退出原因（是正常退出、由于错误退出还是被信号杀死等）是非常有用的信息。

### 设计原理

操作系统提供了 `wait` 和 `waitpid` 等系统调用来实现这一功能。这些调用的设计原理包括：

1. **子进程状态信息的保留：** 当子进程终止时，操作系统不会立即释放它所占用的所有资源。
它会保留一些关键信息（如退出状态码、终止原因等），以便父进程查询。

2. **通信机制：** `wait`/`waitpid` 等函数为父进程和操作系统之间提供了一个通信机制。
通过这些调用，父进程可以获取有关已终止子进程的信息。

3. **状态解析：** 操作系统提供了一系列宏（如 `WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED` 等），以帮助父进程解析从 `wait`/`waitpid` 获得的状态信息。这些宏可以告诉父进程子进程是否正常退出、被信号终止、被信号停止等。

总的来说，检查已回收子进程的退出状态允许父进程获得子进程终止的详细信息，这对于程序的错误处理、资源管理和行为逻辑控制至关重要。操作系统通过在子进程终止时保留关键信息，并提供相应的系统调用和解析宏，使父进程能够有效地执行这一任务。


### 4. 错误条件

如果调用进程没有子进程，那么 `waitpid` 返回 -1，并且设置 errno 为 `ECHILD`。
如果 `waitpid` 函数被一个信号中断，那么它返回 -1，并设置 errno 为 `EINTR`。

您的描述准确地指出了 `waitpid` 函数在特定情况下的行为及其对应的错误代码。让我们逐一解释这些情况：

1. **没有子进程的情况 (`ECHILD`)：**
   - **行为：** 当调用 `waitpid` 的进程没有子进程时（即没有任何可以等待的子进程），`waitpid` 函数会返回 `-1`。
   - **错误代码：** 在这种情况下，`errno` 被设置为 `ECHILD`。这是一个错误情况，指明调用进程没有子进程。
   - **典型场景：** 这种情况可能发生在尝试等待一个已经不存在的子进程，或者程序逻辑错误导致重复等待子进程的情况。

2. **被信号中断的情况 (`EINTR`)：**
   - **行为：** 如果 `waitpid` 函数在等待子进程状态改变的过程中被某个信号中断，那么它同样会返回 `-1`。
   - **错误代码：** 在这种情况下，`errno` 被设置为 `EINTR`。这表明 `waitpid` 函数的执行被一个信号中断。
   - **典型场景：** 这通常发生在多线程或多进程环境中，其中一个进程或线程正在等待子进程，同时另一个进程或线程发送了信号。这种情况下，程序可以选择重新调用 `waitpid` 或者执行其他逻辑来处理这个中断。

在处理 `waitpid` 的返回值时，检查 `errno` 的值是很重要的，因为它提供了关于 `waitpid` 失败原因的具体信息。了解这些错误情况和相应的 `errno` 值对于正确编写和调试涉及子进程管理的程序至关重要。
====================================================================================================
旁注 - 和 Unix 函数相关的常量
像 WNOHANG 和 WUNTRACED 这样的常量是由系统头文件定义的。例如，WNO¬HANG 和 WUNTRACED 是由 wait.h 头文件（间接）定义的：
~~~c
/* Bits in the third argument to 'waitpid'. */
#define WNOHANG    1   /* Don’t block waiting. */
#define WUNTRACED  2   /* Report status of stopped children. */
~~~
为了使用这些常量，必须在代码中包含 wait.h 头文件：
~~~c
#include <sys/wait.h>
~~~
每个 Unix 函数的 man 页列出了无论何时你在代码中使用那个函数都要包含的头文件。
同时，为了检查诸如 ECHILD 和 EINTR 之类的返回代码，你必须包含 errno.h。
====================================================================================================
~~~c
// 列出下面程序所有可能的输出序列：
int main()
{
    if (Fork() == 0) {
        printf("a"); fflush(stdout);
    }
    else {
        printf("b"); fflush(stdout);
        waitpid(-1, NULL, 0);
    }
    printf("c"); fflush(stdout);
    exit(0);
 }
~~~

我们知道序列 `acbc`、`abcc` 和 `bacc` 是可能的，因为它们对应有进程图的拓扑排序（图 8-48）。
而像 `bcac` 和 `cbca` 这样的序列不对应有任何拓扑排序，因此它们是不可行的。

### 5. wait 函数
wait 函数是 waitpid 函数的简单版本：
~~~c
#include <sys/types.h>
#include <sys/wait.h>
~~~

~~~c
pid_t wait(int *statusp);

// 返回：如果成功，则为子进程的 PID，如果出错，则为 -1。
~~~
调用 `wait(&status)` 等价于调用 `waitpid(-1, &status, 0)`。

### 6. 使用 waitpid 的示例
因为 waitpid 函数有些复杂，看几个例子会有所帮助。

图 8-18 展示了一个程序，它使用 waitpid，不按照特定的顺序等待它的所有 N 个子进程终止。
在第 11 行，父进程创建 N 个子进程，在第 12 行，每个子进程以一个唯一的退出状态退出。
在我们继续讲解之前，请确认你已经理解为什么每个子进程会执行第 12 行，而父进程不会。

~~~c
#include "csapp.h"
#define N 2

int main()
{
    int status, i;
    pid_t pid;

    /* Parent creates N children */
    for (i = 0; i < N; i++)
        if ((pid = Fork()) == 0) /* Child */
            exit(100+i);

    /* Parent reaps N children in no particular order */
    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status))
            printf("child %d terminated normally with exit status=%d\n",
                   pid, WEXITSTATUS(status));
        else
            printf("child %d terminated abnormally\n", pid);
    }

    /* The only normal termination is if there are no more children */
    if (errno != ECHILD)
        unix_error("waitpid error");
   
    exit(0);
}
~~~

这个 C 程序片段展示了一个父进程创建多个子进程，并在子进程结束后收集它们的退出状态的例子。这里是这段代码的逐行解释和功能说明：

### 代码解释

1. **变量声明：**
   ```c
   int status, i;
   pid_t pid;
   ```
   这里声明了几个变量：`status` 用于存储子进程的退出状态，`i` 用于循环计数，`pid` 用于存储 `fork`/`waitpid` 调用的返回值。

2. **创建子进程：**
   ```c
   for (i = 0; i < N; i++)
       if ((pid = Fork()) == 0) /* Child */
           exit(100+i);
   ```
   这个循环创建了 `N` 个子进程（`N` 应该是一个事先定义的常量）。对于每个子进程，`fork` 的返回值是 `0`，然后它就调用 `exit(100+i)` 正常结束，返回一个独特的退出码。

3. **父进程回收子进程：**
   ```c
   while ((pid = waitpid(-1, &status, 0)) > 0) {
       if (WIFEXITED(status))
           printf("child %d terminated normally with exit status=%d\n",
                  pid, WEXITSTATUS(status));
       else
           printf("child %d terminated abnormally\n", pid);
   }
   ```
   父进程在这个 `while` 循环中回收所有子进程。`waitpid(-1, &status, 0)` 表示父进程等待任何一个子进程结束。当一个子进程结束时，如果它是正常结束的（通过 `WIFEXITED` 宏检查），打印它的 PID 和退出状态；如果是非正常结束，只打印它的 PID。

4. **错误处理：**
   ```c
   if (errno != ECHILD)
       unix_error("waitpid error");
   ```
   循环结束后，正常情况下应该是没有更多子进程可等待，此时 `errno` 应该设置为 `ECHILD`。如果 `errno` 的值不是 `ECHILD`，则表示有其他错误发生。

5. **程序结束：**
   ```c
   exit(0);
   ```
   程序正常结束。

### 注意事项

- 这里使用的 `Fork()` 函数（注意大写 `F`）可能是对 `fork()` 系统调用的一个包装，这在某些教材或库中常见。它的目的通常是处理 `fork` 可能出现的错误，使得代码更加健壮。
- 确保 `N` 在程序中被正确定义和初始化。
- 这个程序示例简洁地展示了如何使用 `fork` 和 `waitpid` 来管理多个子进程，包括创建子进程、等待子进程结束并获取它们的退出状态。这是 Unix 类操作系统中进程管理的典型用法。



图 8-19 展示了一个简单的改变，它消除了这种不确定性，按照父进程创建子进程的相同顺序来回收这些子进程。
在第 11 行中，父进程按照顺序存储了它的子进程的 PID，然后通过用适当的 PID 作为第一个参数来调用 waitpid，按照同样的顺序来等待每个子进程。
~~~c
#include "csapp.h"
#define N 2

int main()
{
    int status, i;
    pid_t pid[N], retpid;

    /* Parent creates N children */
    for (i = 0; i < N; i++)
        if ((pid[i] = Fork()) == 0) /* Child */
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
~~~
使用 waitpid 按照创建子进程的顺序来回收这些僵死子进程

这段代码的目的是创建 `N` 个子进程，然后父进程依次等待每个子进程结束并打印它们的退出状态。

这里有一些关键点需要注意：

1. **`N` 的定义：** 确保在您的代码中定义了常量 `N`，它代表您打算创建的子进程数量。

2. **`pid` 数组：** 您定义了一个 `pid_t` 类型的数组 `pid[N]`，用于存储每个子进程的 PID。这是一个很好的做法，因为它允许父进程跟踪每个子进程的标识。

3. **子进程的创建：** 在 `for` 循环中，您使用 `fork()` 创建了 `N` 个子进程，并将每个子进程的 PID 存储在 `pid` 数组中。每个子进程执行 `exit(100+i)`，返回一个唯一的退出状态。

4. **子进程的回收：** 在另一个 `while` 循环中，父进程使用 `waitpid` 依次等待每个特定的子进程结束。这里 `i` 被用作 `pid` 数组的索引，确保按顺序等待每个子进程。

5. **错误检查：** 在所有子进程都被回收后，您检查 `errno` 是否为 `ECHILD`。如果不是，调用 `unix_error` 打印错误信息并退出。

6. **程序结束：** 最后，程序正常退出。

这段代码是一个很好的示例，展示了如何在父进程中有效地创建、管理和回收多个子进程。
只需确保 `N` 被正确定义且您的系统有足够资源来创建这么多子进程。在实际应用中，可能还需要考虑额外的错误处理和资源管理措施，以确保程序的健壮性和稳定性。


====================================================================================================
Question

~~~c
int main()
{
    int status;
    pid_t pid;
    
    printf("Hello\n");
    pid = Fork();
    printf("%d\n", !pid);
    if (pid != 0) { // Parents
        if (waitpid(-1, &status, 0) > 0) {
            if (WIFEXITED(status) != 0)
                printf("%d\n", WEXITSTATUS(status));
        }
    }
    printf("Bye\n");
    exit(2);
}
~~~
A. 这个程序会产生多少输出行？
B. 这些输出行的一种可能的顺序是什么？

A. 只简单地计算进程图（图 8-49）中 printf 顶点的个数就能确定输出行数。在这里，有 6 个这样的顶点，因此程序会打印 6 行输出。
B. 任何对应有进程图的拓扑排序的输出序列都是可能的。例如：`Hello、1、0、Bye、2、Bye` 是可能的。
~~~shell
[czy@czy-307-thinkcentre-m720q-n000] ~/new_space/Exceptional_Control_Flow/04 (master) ⚡ 
❯ ./prob1                                                                                                      ⏎
Hello
0
1
Bye
2
Bye
~~~

在实际的工程开发中，进程控制和管理是一个复杂且多样化的领域，它涉及到不同的方法和工具，具体取决于应用的需求、操作系统和所用的编程语言。虽然基本的系统调用（如 `fork`, `exec`, `wait`, `waitpid` 等）在 Unix 和类 Unix 系统中是进程管理的基础，但在现代软件工程中，通常会采用更高级、更安全和更容易管理的方法和工具。以下是一些常见的工程实践：

1. **高级语言和框架：**
   - 现代编程语言（如 Python, Java, C#）提供了更高级的进程管理和并发编程功能。
   这些语言通常提供库和框架，使得进程创建和管理更安全、更简单。
   - 例如，Python 的 `multiprocessing` 模块提供了强大的进程创建和通信工具，而无需直接使用底层的系统调用。

2. **并发模型：**
   - 除了传统的多进程模型，还有其他并发模型，如线程、协程（轻量级线程），以及基于事件的异步编程模型。
   - 这些模型通常更适合于构建高性能的并发应用，因为它们可以减少进程创建和上下文切换的开销。

3. **容器化和虚拟化技术：**
   - 在某些情况下，容器化技术（如 Docker）被用于隔离和管理进程。容器为进程提供了隔离的环境，而无需创建完整的虚拟机。
   - 这种方法适用于微服务架构和云计算，其中独立的服务可以在隔离的容器中运行。

4. **进程监控和管理工具：**
   - 在生产环境中，通常使用进程监控和管理工具（如 systemd, supervisord）来管理后台进程。这些工具可以帮助自动重启失败的进程、记录日志、管理进程依赖等。

5. **信号处理和错误处理：**
   - 在使用低级系统调用时，正确处理信号和错误是很重要的。例如，当使用 `fork` 和 `exec` 创建新进程时，应该小心处理可能的失败情况，并确保资源被正确清理。

6. **安全性和权限管理：**
   - 在多进程程序中，确保安全性和适当的权限管理也是非常重要的。这可能涉及到使用适当的用户权限运行进程，以及在进程间安全地传输数据。

在实际的工程实践中，选择哪种方法取决于具体的应用需求、性能要求、安全性考虑以及开发和维护的复杂性。
通常，现代软件开发倾向于使用更高级的抽象和工具，以减少直接处理底层系统调用的需要，从而降低错误的风险并提高开发效率。
====================================================================================================


### 让进程休眠

sleep函数将一个进程挂起一段指定的时间。

~~~c
#include <unistd.h>
unsigned int sleep(unsigned int secs);

// 返回：还要休眠的秒数。
~~~

如果请求的时间量已经到了，sleep 返回 0，否则返回还剩下的要休眠的秒数。
后一种情况是可能的，如果因为 sleep 函数被一个信号中断而过早地返回。我们将在 8.5 节中详细讨论信号。

我们会发现另一个很有用的函数是 pause 函数，该函数让调用函数休眠，直到该进程收到一个信号。

~~~c
#include <unistd.h>
int pause(void);

// 总是返回 -1。
~~~
====================================================================================================
编写一个 sleep 的包装函数，叫做 `snooze`，带有下面的接口：
`unsigned int snooze(unsigned int secs);`
snooze 函数和 sleep 函数的行为完全一样，除了它会打印出一条消息来描述进程实际休眠了多长时间：
~~~c
Slept for 4 of 5 secs.
~~~

Answer:
~~~c
unsigned int snooze(unsigned int secs) {
    unsigned int rc = sleep(secs);

    printf("Slept for %d of %d secs.\n", secs - rc, secs);
    return rc;
}
~~~
====================================================================================================

### 加载并运行程序
execve 函数在当前进程的上下文中加载并运行一个新程序。

~~~c
#include <unistd.h>
int execve(const char *filename, const char *argv[],
           const char *envp[]);

// 如果成功，则不返回，如果错误，则返回 -1。
~~~

execve 函数加载并运行*可执行目标文件* `filename`，且带参数列表 `argv` 和环境变量列表 `envp`。
只有当出现错误时，例如找不到 filename，execve 才会返回到调用程序。
所以，与 fork—次调用返回两次不同，execve 调用一次并从不返回。

参数列表是用图 8-20 中的数据结构表示的。
argv 变量指向一个以 `null` 结尾的指针数组，其中每个指针都指向一个参数字符串。
按照惯例，argv[0] 是可执行目标文件的名字。

环境变量的列表是由一个类似的数据结构表示的，如图 8-21 所示。
envp 变量指向一个以 `null` 结尾的指针数组，其中每个指针指向一个环境变量字符串，每个串都是形如 “`name=value`” 的名字—值对。

在 execve 加载了 filename 之后，它调用 7.9 节中描述的启动代码。
启动代码设置栈，并将控制传递给新程序的主函数，该主函数有如下形式的原型

~~~c
  int main(int argc, char **argv, char **envp);
~~~
或者等价的
~~~c
int main(int argc, char *argv[], char *envp[]);
~~~

当 main 开始执行时，用户栈的组织结构如图 8-22 所示。
让我们从栈底（高地址）往栈顶（低地址）依次看一看。首先是参数和环境字符串。
往上紧随其后的是以 null 结尾的指针数组，其中每个指针都指向栈中的一个环境变量字符串。
全局变量 environ 指向这些指针中的第一个 envp[0]。
紧随环境变量数组之后的是以 null 结尾的 argv[] 数组，其中每个元素都指向栈中的一个参数字符串。
在栈的顶部是系统启动函数 `libc_start_main`（见 7.9 节）的栈帧。

在大多数类 Unix 系统中，当一个新程序开始执行时，其用户栈的典型组织结构可以用 ASCII 图表示如下：

```
+------------------+
|    Stack         |  <--- 高地址
|    Growth        |
|    Direction     |
|                  |
|                  |
|                    |
| ------------------ |
| Environment Vars   |
| ------------------ |
| Argv[n]            |
| ...                |
| Argv[1]            |
| Argv[0]            |
| ------------------ |
| Argc               |
| ------------------ | <--- 栈指针 (SP) 初始位置 |
|                    |
| Free Space         |
| (for growth)       |
|                    |
|                    |
+------------------+  <--- 低地址
```

这个图显示了程序刚开始执行时的用户栈布局。具体细节可能根据不同的系统架构和编译器略有不同，但大体结构通常如下：

- **栈的方向**：在大多数现代体系结构中，栈是向下增长的，即从高地址向低地址增长。

- **参数计数 (`argc`)**：这是传递给 `main` 函数的参数数量，位于栈的最底部（相对于栈的初始位置）。

- **参数值 (`argv[]`)**：这是指向传递给 `main` 函数的字符串参数的指针数组。`argv[0]` 是程序的名称，`argv[1]` 到 `argv[n]` 是传递给程序的命令行参数。

- **环境变量**：在 `argv[]` 之上是环境变量，它们以类似于 `argv[]` 的方式组织。

- **自由空间**：这是栈的其余部分，用于程序运行时的局部变量、函数调用等。

main 函数有 3 个参数：
argc，它给出 argv[ ] 数组中非空指针的数量；
argv，指向 argv[ ] 数组中的第一个条目；
envp，指向 envp[ ] 数组中的第一个条目。

Linux 提供了几个函数来操作环境数组：
~~~c
#include <stdlib.h>
char *getenv(const char *name);

// 返回：若存在则为指向 name 的指针，若无匹配的，则为 NULL。
~~~
`getenv` 函数在环境数组中搜索字符串 “`name=value`”。如果找到了，它就返回一个指向 value 的指针，否则它就返回 NULL。

~~~c
#include <stdlib.h>

int setenv(const char *name, const char *newvalue, int overwrite);
// 返回：若成功则为 0，若错误则为 -1。

void unsetenv(const char *name);
// 返回：无。
~~~
如果环境数组包含一个形如 “`name=oldva1ue`” 的字符串，那么 `unsetenv` 会删除它
而 setenv 会用 newvalue 代替 oldvalue，但是只有在 overwirte 非零时才会这样。
如果 name 不存在，那么 setenv 就把 “name=newvalue” 添加到数组中。

您提供的信息是关于 Linux 环境中用于操作环境变量的几个标准函数的描述。以下是对这些函数的详细解释：

### 1. `getenv`
- **函数原型：**
  ```c
  char *getenv(const char *name);
  ```
- **功能：** 搜索环境数组，查找名为 `name` 的环境变量。
- **返回值：**
  - **若存在：** 返回一个指向该环境变量值的指针。
  - **若不存在：** 返回 `NULL`。
- **用途：** `getenv` 用于获取环境变量的值。例如，获取 PATH 环境变量的值。

### 2. `setenv`
- **函数原型：**
  ```c
  int setenv(const char *name, const char *newvalue, int overwrite);
  ```
- **功能：** 添加或修改环境变量 `name` 的值。
- **参数：**
  - `name`：要设置或修改的环境变量名。
  - `newvalue`：新的值。
  - `overwrite`：控制是否覆盖现有的值（非零表示覆盖）。
- **返回值：**
  - **成功：** 返回 `0`。
  - **失败：** 返回 `-1`。
- **用途：** `setenv` 用于设置或修改环境变量。如果 `overwrite` 为非零且变量已存在，它会更新变量的值；如果变量不存在，它会创建新的环境变量。

### 3. `unsetenv`
- **函数原型：**
  ```c
  void unsetenv(const char *name);
  ```
- **功能：** 删除名为 `name` 的环境变量。
- **返回值：** 无返回值。
- **用途：** `unsetenv` 用于从环境中删除一个环境变量。

### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 获取环境变量
    char *path = getenv("PATH");
    if (path) {
        printf("Original PATH: %s\n", path);
    }

    // 设置新的环境变量
    if (setenv("MY_VAR", "HelloWorld", 1) != -1) {
        printf("MY_VAR set to: %s\n", getenv("MY_VAR"));
    }

    // 修改现有的环境变量
    if (setenv("PATH", "/usr/local/bin", 1) != -1) {
        printf("Modified PATH: %s\n", getenv("PATH"));
    }

    // 删除环境变量
    unsetenv("MY_VAR");
    if (!getenv("MY_VAR")) {
        printf("MY_VAR removed\n");
    }

    return 0;
}
```
这段代码演示了如何使用 `getenv`, `setenv`, 和 `unsetenv` 函数来获取、设置和删除环境变量。
这些函数在处理程序的环境设置时非常有用，尤其是在需要根据环境配置来调整程序行为的情况下。

> 旁注 - 程序与进程
这是一个适当的地方，停下来，确认一下你理解了程序和进程之间的区别。
 - 程序是一堆代码和数据；程序可以作为目标文件存在于磁盘上，或者作为段存在于地址空间中。
 - 进程是执行中程序的一个具体的实例；
 程序总是运行在某个进程的上下文中。如果你想要理解 fork 和 execve 函数，理解这个差异是很重要的。
 fork 函数在新的子进程中运行相同的程序，新的子进程是父进程的一个复制品。
 `execve` 函数在当前进程的上下文中加载并运行一个新的程序。它会覆盖当前进程的地址空间，但并没有创建一个新进程。
 新的程序仍然有相同的 PID，并且继承了调用 `execve` 函数时已打开的所有文件描述符。

====================================================================================================
您提到的关于程序与进程之间的区别以及 `fork` 和 `execve` 函数的作用和重要性是非常准确的。
这是理解操作系统和进程管理的基础概念。下面是对这些概念的进一步阐述：

### 程序与进程的区别

1. **程序（Program）**:
   - **定义**: 程序是一系列指令和数据的集合，用于执行特定的任务。它是静态的，可以存储在文件系统中，如可执行文件或脚本。
   - **属性**: 程序本身不执行任何操作，它只是一系列要执行的操作的描述。

2. **进程（Process）**:
   - **定义**: 进程是程序的运行实例。当程序被操作系统加载到内存并开始执行时，它成为一个进程。
   - **属性**: 进程是动态的，它包括程序代码、当前活动、程序计数器、寄存器的内容、变量的值、进程堆栈、环境变量等。每个进程都有其独特的进程标识符（PID）。

### `fork` 和 `execve` 函数

- **`fork` 函数**:
  - **作用**: `fork` 创建一个新的进程，这个新进程几乎是父进程的完整副本。两个进程（父进程和子进程）有相同的程序代码和数据，但它们有不同的执行流和独立的内存空间。
  - **使用场景**: `fork` 通常用于创建一个新的执行环境，父进程和子进程可以在这个点上分叉，执行不同的任务。

- **`execve` 函数**:
  - **作用**: `execve` 加载并运行一个新的程序。它替换当前进程的地址空间，包括程序代码和数据，用新程序的内容替换。
  - **重要性**: 调用 `execve` 后，原来的程序被新程序替换，但进程ID不变。这允许一个进程启动另一个程序，如在 shell 环境中执行命令。
  - **继承特性**: `execve` 执行的新程序继承了调用进程的某些属性，例如打开的文件描述符。

理解这些概念对于深入理解多进程编程、进程间通信（IPC）、系统编程等方面非常关键。在 Unix 和类 Unix 系统中，`fork` 和 `execve` 是操作进程和执行程序的基础工具。

当执行 `execve()` 系统调用时，会发生一系列复杂的操作，这些操作涉及到 CPU、寄存器以及程序的地址空间。
以下是在 `execve()` 调用过程中发生的主要变化：

### 1. CPU 和寄存器的变化

1. **系统调用入口**：
   - 当 `execve()` 被调用时，程序执行一个系统调用指令，通知 CPU 进入内核模式。
   - CPU 切换到内核模式，保存程序的上下文（包括程序计数器和其他寄存器状态），以便稍后能够恢复。

2. **参数传递**：
   - 调用 `execve()` 时传递的参数（包括要执行的程序路径、环境变量等）会被复制到内核空间，通常是通过寄存器或栈来实现。

3. **内核处理**：
   - 在内核模式下，操作系统接管控制权。它首先验证 `execve()` 调用的参数，如检查文件路径的有效性和访问权限。

### 2. 地址空间的变化

4. **地址空间替换**：
   - `execve()` 的核心作用是用新程序的代码和数据替换当前进程的地址空间。这包括清除或覆盖原来的程序代码、数据段、堆和栈。
   - 操作系统将新程序的代码和数据加载到进程的地址空间中。这通常意味着读取新程序的可执行文件，并将其内容映射到进程的地址空间。

5. **堆栈和寄存器重置**：
   - 堆栈被初始化为新程序的起始堆栈状态。这通常包括程序参数和环境变量的设置。
   - CPU 寄存器被设置为新程序的初始状态。特别是，程序计数器被设置为新程序的入口点。

### 3. 新程序开始执行

6. **从内核模式返回用户模式**：
   - 一旦新程序被加载并且所有必要的初始化完成，CPU 从内核模式切换回用户模式。
   - 控制权传递给新程序，新程序开始执行。

### 4. 结果

- `execve()` 调用本身不返回到原程序，因为原程序的代码和数据已经被新程序替换。如果 `execve()` 调用失败（例如，由于无法找到可执行文件），它会返回错误，并且原程序继续执行。

这个过程是操作系统内核管理进程和程序执行的一个关键方面，展示了系统调用、内存管理和进程控制的复杂性。通过 `execve()`，操作系统能够在同一个进程上下文中启动一个全新的程序，这是 Unix 系统编程中的一个强大特性。


在 `execve()` 调用过程中的“堆栈和寄存器重置”步骤是将原始进程的运行环境转换为新程序所需的环境的重要阶段。
这一步涉及以下几个关键操作：

### 1. 堆栈重置

- **清除原始堆栈：** 原始进程的用户堆栈（包括局部变量、函数调用的返回地址等）会被清空或覆盖。这是因为新程序将有自己的堆栈布局和需求。

- **初始化新堆栈：** 新程序的堆栈被初始化。这包括：
  - **设置新的栈顶指针：** 栈指针（通常是寄存器 `esp` 或 `rsp`，取决于体系结构）被设置为新程序的起始堆栈位置。
  - **传递参数和环境变量：** 新程序的命令行参数和环境变量被放置在新堆栈中。这些信息通常以数组的形式存储，数组的元素是指向具体参数和变量字符串的指针。

### 2. 寄存器重置

- **寄存器状态清除：** 原始进程的寄存器状态（包括通用寄存器、指令指针、标志寄存器等）会被清除或重置。

- **初始化程序计数器：** 程序计数器（PC，也称为指令指针，如 `eip` 或 `rip` 寄存器）被设置为新程序的入口点地址。这确保了新程序开始执行时从正确的位置开始。

- **其他寄存器初始化：** 根据新程序的需求，可能还需要初始化其他寄存器。例如，在某些体系结构中，可能需要设置数据段和堆栈段的寄存器。

### 3. 开始执行新程序

- **转换控制权：** 一旦堆栈和寄存器准备就绪，操作系统会将控制权交给新程序。这是通过跳转到程序计数器指向的新程序入口点来实现的。

- **新程序执行：** 新程序开始执行其主函数（如 `main`）。
此时，新程序的运行环境（包括堆栈和寄存器）已经完全是基于新程序的代码和数据，与原始进程完全不同。

这一过程的核心是将进程的执行环境从一个程序转换为另一个程序，同时保持进程本身的连续性（即进程ID和其他系统资源保持不变）。
`execve()` 的这一操作是现代操作系统灵活处理程序执行的基础之一。

Question:
写一个叫做 `myecho` 的程序，打印出它的命令行参数和环境变量。
~~~shell
linux> ./myecho arg1 arg2
Command-ine arguments:
    argv[0]: myecho
    argv[1]: arg1
    argv[2]: arg2
Environment variables:
    envp[0]: PWD=/usr0/droh/ics/code/ecf
    envp[1]: TERM=emacs
    .
    .
    .
    envp[25]: USER=droh
    envp[26]: SHELL=/usr/local/bin/tcsh
    envp[27]: HOME=/usr0/droh
~~~

Answer:
~~~c
#include "csapp.h"

int main(int argc, char *argv[], char *envp[])
{
    int i;

    printf("Command-line arguments:\n");
    for (i = 0; argv[i] != NULL; i++)
        printf(" argv[%2d]: %s\n", i, argv[i]);

    printf("\n");
    printf("Environment variables:\n");
    for (i = 0; envp[i] != NULL; i++)
        printf(" envp[%2d]: %s\n", i, envp[i]);

    exit(0);
}
~~~

### 4. 利用 fork 和 execve 运行程序利用 fork 和 execve 运行程序
像 Unix `shell` 和 `Web 服务器`这样的程序大量使用了 `fork` 和 `execve` 函数。
shell 是一个交互型的应用级程序，它代表用户运行其他程序。最早的 shell 是 sh 程序，后面出现了—些变种，比如 csh、tcsh、ksh 和 bash。
shell 执行一系列的读/求值（read/evaluate）步骤，然后终止。读步骤读取来自用户的一个命令行。求值步骤解析命令行，并代表用户运行程序。

图 8-23 展示了一个简单 shell 的 main 例程。shell 打印一个命令行提示符，等待用户在 stdin 上 输入命令行，然后对这个命令行求值。

~~~c
shellex.c

#include "csapp.h"
#define MAXARGS 128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int main()
{
    char cmdline[MAXLINE]; /* Command line */

    while (1) {
        /* Read */
        printf("> ");
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);

        /* Evaluate */
        eval(cmdline);
    }
}
~~~

图 8-24 展示了对命令行求值的代码。它的首要任务是调用 `parseline` 函数（见图 8-25），这个函数解析了以空格分隔的命令行参数，并构造最终会传递给 execve 的 argv 向量。

第一个参数被假设为要么是一个内置的 shell 命令名，马上就会解释这个命令
要么是一个可执行目标文件，会在一个新的子进程的上下文中加载并运行这个文件。

~~~c
/* eval - Evaluate a command line */
void eval(char *cmdline)
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;   /* Ignore empty lines */

    if (!builtin_command(argv)) {
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        /* Parent waits for foreground job to terminate */
        if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
            }
        else
            printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{
    if (!strcmp(argv[0], "quit")) /* quit command */
        exit(0);
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
        return 1;
    return 0;                     /* Not a builtin command */
}


/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;

    if (argc == 0) /* Ignore blank line */
        return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}
~~~

如果最后一个参数是一个 “`&`” 字符，那么 `parseline` 返回 1，表示应该在后台执行该程序（shell 不会等待它完成）
否则，它返回 0，表示应该在前台执行这个程序（shell 会等待它完成）。

在解析了命令行之后，`eval` 函数调用 `builtin_command` 函数，该函数检查第一个命令行参数是否是一个内置的 shell 命令。
如果是，它就立即解释这个命令，并返回值 1。否则返回 0。
简单的 shell 只有一个内置命令—— quit 命令，该命令会终止 shell。实际使用的 shell 有大量的命令，比如 pwd、jobs 和 fg。

如果 builtin_cornnand 返回 0，那么 shell 创建一个子进程，并在子进程中执行所请求的程序。
如果用户要求在后台运行该程序，那么 shell 返回到循环的顶部，等待下一个命令行。
否则，shell 使用 waitpid 函数等待作业终止。当作业终止时，shell 就开始下一轮迭代。

注意，这个简单的 shell 是有缺陷的，因为它并不回收它的后台子进程。
修改这个缺陷就要求使用信号，我们将在下一节中讲述信号。



### Rust 实现

```rust
use std::process::{Command, Stdio, Child};
use std::io::{stdin, stdout, Write};
use std::env;

fn main() {
    loop {
        print!("> ");
        stdout().flush().unwrap();

        let mut input = String::new();
        stdin().read_line(&mut input).unwrap();
        let input = input.trim();

        if input == "quit" {
            break;
        }

        let (command, args) = parse_line(&input);
        if command.is_empty() {
            continue;
        }

        let background = args.contains(&"&");
        execute_command(command, args, background);
    }
}

fn parse_line(input: &str) -> (&str, Vec<&str>) {
    let mut parts = input.split_whitespace();
    let command = parts.next().unwrap_or_default();
    let args = parts.collect::<Vec<&str>>();
    (command, args)
}

fn execute_command(command: &str, args: Vec<&str>, background: bool) {
    let mut child = Command::new(command)
        .args(&args)
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .spawn()
        .expect("failed to execute command");

    if !background {
        child.wait().expect("failed to wait on child");
    } else {
        println!("Started {} in background", child.id());
    }
}

```

### 代码解释

1. **主循环**：程序进入一个无限循环，提示用户输入命令。
2. **解析输入**：`parse_line` 函数接收用户输入的字符串，并将其分割成命令和参数。
3. **执行命令**：`execute_command` 函数使用 `Command` 构造器从 `std::process` 模块来执行命令。这个函数接收命令、参数列表以及是否在后台运行的标志。
4. **后台执行**：如果命令应该在后台运行（即命令行以 "&" 结尾），则使用 `spawn` 而不是 `wait`。这允许父进程继续运行而不等待子进程结束。
5. **内置命令**：`quit` 命令被作为内置命令处理，用于退出 shell。

这个 Rust 版本的 shell 实现简洁且高效，充分利用了 Rust 的安全性和表达力。
与 C 版本相比，它避免了手动管理内存和字符串的需要，并利用 Rust 的强类型系统和错误处理机制。