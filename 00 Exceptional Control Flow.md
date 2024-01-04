# Exceptional Control Flow
# Intro

第 8 章：异常控制流

从给处理器加电开始，直到你断电为止，程序计数器假设一个值的序列:

        a_1, a_2, ... a_n-1

其中，每个 a_k 是某个相应的指令 `I_k` 的地址。每次从 a_k 到 a_k+1 的过渡称为控制转移（`control transfer`）。
这样的控制转移序列叫做处理器的控制流（**flow of control** 或 **control flow**）。

最简单的一种控制流是一个“平滑的”序列，其中每个`I_k`和`I_k+1`在内存中都是相邻的。这种平滑流的突变（也就是与不相邻）通常是由诸如跳转、调用和返回这样一些熟悉的程序指令造成的。这样一些指令都是必要的机制，使得程序能够对由程序变量表示的内部程序状态中的变化做出反应。

但是系统也必须能够对系统状态的变化做出反应，这些系统状态不是被内部程序变量捕获的，而且也不一定要和程序的执行相关。
比如，一个硬件定时器定期产生信号，这个事件必须得到处理。包到达网络适配器后，必须存放在内存中。程序向磁盘请求数据，然后休眠，直到被通知说数据已就绪。当子进程终止时，创造这些子进程的父进程必须得到通知。

现代系统通过使控制流发生突变来对这些情况做出反应。一般而言，我们把这些突变称为异常控制流（*Exceptional Control Flow*，**ECF**）。
异常控制流发生在计算机系统的各个层次。
比如，在硬件层，硬件检测到的事件会触发控制突然转移到异常处理程序。
在操作系统层，内核通过上下文切换将控制从一个用户进程转移到另一个用户进程。
在应用层，一个进程可以发送信号到另一个进程，而接收者会将控制突然转移到它的一个信号处理程序。—个程序可以通过回避通常的栈规则，并执行到其他函数中任意位置的非本地跳转来对错误做出反应。

作为程序员，理解 ECF 很重要，这有很多原因：

 - **理解 ECF 将帮助你理解重要的系统概念**。
   ECF 是操作系统用来实现 I/O、进程和虚拟内存的基本机制。在能够真正理解这些重要概念之前，你必须理解 ECF。

 - **理解 ECF 将帮助你理解应用程序是如何与操作系统交互的**
   应用程序通过使用一个叫做陷阱（`trap`）或者系统调用（`system call`）的 ECF 形式，向操作系统请求服务。
   比如，向磁盘写数据、从网络读取数据、创建一个新进程，以及终止当前进程，都是通过应用程序调用系统调用来实现的。
   理解基本的系统调用机制将帮助你理解这些服务是如何提供给应用的。

 - **理解 ECF 将帮助你编写有趣的新应用程序**
   操作系统为应用程序提供了强大的 ECF 机制，用来创建新进程、等待进程终止、通知其他进程系统中的异常事件，以及检测和响应这些事件。
   如果理解了这些 ECF 机制，那么你就能用它们来编写诸如 Unix shell 和 Web 服务器之类的有趣程序了。

 - **理解 ECF 将帮助你理解并发**
   ECF 是计算机系统中实现并发的基本机制。
   在运行中的并发的例子有：中断应用程序执行的异常处理程序，在时间上重叠执行的进程和线程，以及中断应用程序执行的信号处理程序。理解 ECF 是理解并发的第一步。我们会在第 12 章中更详细地研究并发。

 - **理解 ECF 将帮助你理解软件异常如何工作**。
  像 C++ 和 Java 这样的语言通过 try、catch 以及 throw 语句来提供软件异常机制。
  *软件异常允许程序进行非本地跳转（即违反通常的调用/返回栈规则的跳转）来响应错误情况*。
  非本地跳转是一种*应用层 ECF*，在 C 中是通过 `setjmp` 和 `longjmp` 函数提供的。理解这些低级函数将帮助你理解高级软件异常如何得以实现。  

  对系统的学习，到目前为止你已经了解了应用是如何与硬件交互的。
  本章的重要性在于你将开始学习应用是如何与操作系统交互的。
  有趣的是，这些交互都是围绕着 ECF 的。我们将描述存在于一个计算机系统中所有层次上的各种形式的 ECF。
  从异常开始，异常位于硬件和操作系统交界的部分。我们还会讨论系统调用，它们是为应用程序提供到操作系统的入口点的异常。
  然后，我们会提升抽象的层次，描述进程和信号，它们位于应用和操作系统的交界之处。最后讨论非本地跳转，这是 ECF 的一种应用层形式。

====================================================================================================
Exceptional Control Flow (ECF) refers to the mechanisms in computer systems that alter the normal sequential execution of instructions. These mechanisms enable a computer to respond to events that can occur asynchronously (independent of the main program flow) or as a result of executing certain instructions. ECF is crucial for handling unexpected situations, errors, and for providing more flexible control over program execution.

Here are some common forms of Exceptional Control Flow:

1. **Interrupts:** Hardware interrupts are signals sent to the processor from external devices, like a mouse or keyboard, to indicate that an event has occurred which requires immediate attention. Software interrupts are similar but are triggered by software events.

2. **Exceptions:** These are unusual conditions that occur while a program is running, such as divide-by-zero errors, invalid memory accesses, or other illegal instruction usages. When an exception occurs, the normal control flow is interrupted, and control is transferred to a special exception handler.

3. **Signals:** In operating systems like UNIX, signals are used to notify processes that a specific event has occurred. For example, a `SIGKILL` signal instructs a process to terminate, and a `SIGALRM` signal indicates that a timer has expired.

4. **Context Switching:** In multi-tasking operating systems, the scheduler may interrupt the execution of a running process to switch the CPU to another process. This context switch is a type of ECF, as it alters the normal flow of execution.

5. **System Calls:** These are requests in a user program to perform I/O operations, create or terminate processes, or communicate with other processes, which are handled by the operating system. A system call interrupts the current process, transfers control to the OS, and then returns control back to the process.

6. **Non-Local Jumps:** In programming, constructs like `setjmp` and `longjmp` in C provide a way to jump from one point in a program to another, bypassing the normal call and return rules. 

ECF is essential for the efficient and robust operation of computer systems, allowing them to handle asynchronous events, share resources among multiple processes, and recover from errors.

异常控制流（ECF）指的是计算机系统中改变正常指令顺序执行的机制。
这些机制使计算机能够响应可能异步发生的事件（独立于主程序流程）或由于执行某些指令而产生的事件。
异常控制流对于处理意外情况、错误以及提供更灵活的程序执行控制至关重要。

以下是一些常见的异常控制流形式：

1. **中断（Interrupts）：** 硬件中断是外部设备（如鼠标或键盘）向处理器发送的信号，表示发生了需要立即注意的事件。软件中断类似，但由软件事件触发。

2. **异常（Exceptions）：** 这是程序运行时发生的不寻常情况，如除零错误、无效内存访问或其他非法指令使用。发生异常时，正常的控制流被中断，控制权转移到特殊的异常处理程序。

3. **信号（Signals）：** 在像 UNIX 这样的操作系统中，信号用于通知进程某个特定事件已发生。例如，`SIGKILL` 信号指示进程终止，`SIGALRM` 信号表示定时器已到期。

4. **上下文切换（Context Switching）：** 在多任务操作系统中，调度程序可能会中断正在运行的进程的执行，将 CPU 切换到另一个进程。这种上下文切换是一种异常控制流，因为它改变了正常的执行流程。

5. **系统调用（System Calls）：** 这些是用户程序中执行 I/O 操作、创建或终止进程或与其他进程通信的请求，由操作系统处理。系统调用中断当前进程，将控制权转移到操作系统，然后将控制权返回给进程。

6. **非局部跳转（Non-Local Jumps）：** 在编程中，像 C 语言中的 `setjmp` 和 `longjmp` 这样的结构提供了一种从程序的一个点跳到另一个点的方式，绕过了正常的调用和返回规则。

异常控制流对于计算机系统的高效和稳健运行至关重要，使它们能够处理异步事件，共享多个进程之间的资源，并从错误中恢复。
