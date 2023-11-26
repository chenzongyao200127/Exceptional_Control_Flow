# Processes
# 进程

异常是允许操作系统内核提供进程（process）概念的基本构造块，进程是计算机科学中最深刻、最成功的概念之一。

在现代系统上运行一个程序时，我们会得到一个假象，就好像我们的程序是系统中当前运行的唯一的程序一样。我们的程序好像是独占地使用处理器和内存。处理器就好像是无间断地一条接一条地执行我们程序中的指令。最后，我们程序中的代码和数据好像是系统内存中唯一的对象。这些假象都是通过进程的概念提供给我们的。

进程的经典定义就是一个执行中程序的实例。系统中的每个程序都运行在某个进程的上下文（`context`）中。
上下文是由程序正确运行所需的状态组成的。这个状态包括存放在内存中的程序的代码和数据，它的栈、通用目的寄存器的内容、程序计数器、环境变量以及打开文件描述符的集合。

每次用户通过向 shell 输入一个可执行目标文件的名字，运行程序时，shell 就会创建一个新的进程，然后在这个新进程的上下文中运行这个可执行目标文件。
应用程序也能够创建新进程，并且在这个新进程的上下文中运行它们自己的代码或其他应用程序。

关于操作系统如何实现进程的细节的讨论超出了本书的范围。反之，我们将关注进程提供给应用程序的关键抽象：
 - 一个独立的逻辑控制流，它提供一个假象，好像我们的程序独占地使用处理器。
 - 一个私有的地址空间，它提供一个假象，好像我们的程序独占地使用内存系统。让我们更深入地看看这些抽象。

> 状态机视角
In computer science, a process is a fundamental concept related to the execution of computer programs. 
Here is a detailed explanation:

### Definition and Basic Concept

1. **Execution Instance**: A process is essentially an instance of a computer program that is being executed. It includes the program code and its current activity. Each process has a unique process identifier (PID).

2. **Memory Allocation**: A process typically has its own dedicated memory space, including the code segment (where the executable program resides), data segment (for variables and dynamically allocated memory), and stack (for tracking function calls and local variables).

3. **Execution State**: Processes can be in one of several states, such as `running` (actively executing instructions), `ready` (ready to run but waiting for CPU time), `blocked` (waiting for some event to occur), and `terminated`.

### Process Management

1. **Lifecycle**: The lifecycle of a process includes its creation (typically through system calls like `fork` or `exec` in Unix-based systems), execution, and termination.

2. **Scheduling**: The operating system manages processes through a scheduler, which allocates CPU time to various processes, handling multitasking by switching between them.

3. **Context Switching**: When the CPU switches from executing one process to another, it saves the state of the old process and loads the saved state of the new process. This is known as context switching.

4. **Inter-process Communication (IPC)**: Processes often need to communicate with each other. They do so using various IPC techniques like pipes, message queues, shared memory, or sockets.

### Threads vs. Processes

- **Threads**: A thread is a smaller execution unit within a process. A process can have multiple threads, all of which share the same memory space but can run independently and simultaneously.

### Security and Isolation

- Processes provide a level of security and isolation by separating the memory spaces. This isolation prevents processes from inadvertently interfering with each other’s memory.

### Modern Operating Systems

- In modern operating systems, processes are the key entities to which the OS allocates resources (like CPU time, memory, file handles). The OS uses various mechanisms for process management, including process scheduling, synchronization, and communication.

### Resource Allocation

- Each process has its own set of resources allocated by the operating system, such as file descriptors, network sockets, and memory.

In summary, a process in computer science is an active instance of a program that includes its code, data, and state. Process management is a critical aspect of operating systems, facilitating resource allocation, multitasking, and system stability and security.


## 8.2.1 逻辑控制流
即使在系统中通常有许多其他程序在运行，像它在独占地使用处理器。

如果想用调试器单步执行程序，我们会看到一系列的程序计数器（PC）的值，这些值唯一地对应于包含在程序的可执行目标文件中的指令，或是包含在运行时动态链接到程序的共享对象中的指令。这个 PC 值的序列叫做逻辑控制流，或者简称逻辑流。

考虑一个运行着三个进程的系统，如图 8-12 所示。处理器的一个物理控制流被分成了三个逻辑流，每个进程一个。图 8 一每个竖直的条表示一个进程的逻辑流的一部分。在这个例子中，三个逻辑流的执行是交错的。进程 A 运行了一会儿，然后是进程 B 开始运行到完成。然后，进程 C 运行了一会儿，进程 A 接着运行直到完成。最后，进程 C 可以运行到结束了。

图 8-12 的关键点在于进程是轮流使用处理器的。每个进程执行它的流的一部分，然后被抢占（preempted）（暂时挂起），然后轮到其他进程。对于一个运行在这些进程之一的上下文中的程序，它看上去就像是在独占地使用处理器。
唯一的反面例证是，如果我们精确地测量每条指令使用的时间，会发现在程序中一些指令的执行之间，CPU 好像会周期性地停顿。然而，每次处理器停顿，它随后会继续执行我们的程序，并不改变程序内存位置或寄存器的内容。
[黑客帝国]
[进程为每个程序提供了一种假象，好像程序在独占地使用处理器。每个竖直的条表示一个进程的逻辑控制流的一部分]


## 8.2.2 并发流
计算机系统中逻辑流有许多不同的形式。异常处理程序、进程、信号处理程序、线程和 Java 进程都是逻辑流的例子。
一个逻辑流的执行在时间上与另一个流重叠，称为并发流（`concurrent flow`），这两个流被称为并发地运行。更准确地说，流 X 和 Y 互相并发，当且仅当 X 在 Y 开始之后和 Y 结束之前开始，或者 Y 在 X 开始之后和 X 结束之前开始。
例如，图 8-12 中，进程 A 和 B 并发地运行，A 和 C 也一样。另一方面，B 和 C 没有并发地运行，因为 B 的最后一条指令在 C 的第一条指令之前执行。

多个流并发地执行的一般现象被称为并发（`concurrency`）。一个进程和其他进程轮流运行的概念称为多任务（`multitasking`）
一个进程执行它的控制流的一部分的每一时间段叫做时间片（`time slice`）。因此，多任务也叫做时间分片（`timeslicing`）。例如，图 8-12 中，进程 A 的流由两个时间片组成。

注意，并发流的思想与流运行的处理器核数或者计算机数无关。如果两个流在时间上重叠，那么它们就是并发的，即使它们是运行在同一个处理器上。
不过，有时我们会发现确认*并行流*是很有帮助的，它是并发流的一个真子集。
如果两个流并发地运行在不同的处理器核或者计算机上，那么我们称它们为并行流（*parallel flow*），它们并行地运行（running in parallel），且并行地执行（parallel execution）。

Concurrent flow, in the context of computer science and programming, refers to the scenario where multiple sequences of operations (processes, threads, or tasks) are executed in overlapping time periods. It does not necessarily mean these sequences are being executed at the exact same instant (as in parallelism), but rather that their executions are interleaved or overlap in time. Here's a more detailed look:

### Key Concepts of Concurrent Flow

1. **Overlapping Execution**: In concurrent computing, multiple sequences of operations are active at the same time. They may not be processing simultaneously (like in parallel computing) but their lifecycles overlap.

2. **Multitasking on Single Processor**: On a single processor, concurrency is achieved through multitasking, where the processor rapidly switches between different tasks, giving the illusion that they are executing simultaneously.

3. **Multithreading**: In a single application, concurrency is often implemented using threads. Multiple threads within the same process can run concurrently, sharing resources but executing independently.

4. **Asynchronous Operations**: Concurrency also encompasses asynchronous operations, where a task does not need to wait for the completion of another task and can proceed independently.

### Importance of Concurrency

- **Efficiency**: Concurrency allows for more efficient use of resources. For instance, while one task waits for I/O operations to complete, another task can use the CPU.

- **Responsiveness**: In user interfaces and real-time systems, concurrency is essential for maintaining responsiveness, as different operations or requests can be handled as they occur.

- **Scalability**: Concurrent systems can handle more tasks in a given time frame, which is vital in high-load scenarios like web servers or database management systems.

### Challenges

- **Complexity in Design and Debugging**: Concurrent programming introduces complexity, making it challenging to design, debug, and test. Issues like race conditions and deadlocks are common pitfalls.

- **Synchronization**: Managing access to shared resources and ensuring data consistency requires careful synchronization, which can be achieved using mechanisms like locks, semaphores, or message queues.

- **Context Switching Overhead**: While multitasking, context switching (switching the CPU from one task to another) can introduce overhead, affecting performance.

### Concurrent vs. Parallel Computing

- **Concurrent Computing**: Focuses on managing multiple tasks at the same time. It's more about the structure of the system.

- **Parallel Computing**: Emphasizes simultaneous execution of tasks (often on multiple processors), aiming to increase computational speed.

In essence, concurrent flow is about dealing with multiple tasks by interleaving their execution, optimizing resource use, and improving system responsiveness and efficiency. It's a fundamental concept in modern software and system design, particularly in environments with limited computing resources or those requiring high responsiveness.


## 私有地址空间

进程也为每个程序提供一种假象，好像它独占地使用系统地址空间。在一台 n 位地址的机器上，地址空间是个可能地址的集合，0，1，⋯， 2^n - 1。
进程为每个程序提供它自己的私有地址空间。一般而言，和这个空间中某个地址相关联的那个内存字节是不能被其他进程读或者写的，从这个意义上说，这个地址空间是私有的。
尽管和每个私有地址空间相关联的内存的内容一般是不同的，但是每个这样的空间都有相同的通用结构。比如，图 8-13 展示了一个 x86-64 Linux 进程的地址空间的组织结构。

Creating a detailed ASCII representation of the x86-64 Linux process address space structure can be quite complex due to the intricacies and specific alignments of various segments. However, I'll provide a simplified view. In a 64-bit Linux environment, the process address space is typically divided into several segments:

```
+-------------------+ 0xFFFFFFFFFFFFFFFF (64-bit)
|                   |
|    Kernel Space   |
|                   |
+-------------------+ Some Upper Address (depends on kernel config)
|                   |
|    (unused)       |
|                   |
+-------------------+ ~0x800000000000 (commonly)
|                   |
|   User Stack      |
|    (grows down)   |
+-------------------+
|                   |
|    (unused)       |
|                   |
+-------------------+
|   Memory Mappings |
| (shared libs, etc)|
+-------------------+
|                   |
|    Heap Segment   |
|    (grows up)     |
+-------------------+
|    BSS Segment    |
+-------------------+
|    Data Segment   |
+-------------------+
|    Text Segment   |
| (Program Code)    |
+-------------------+ 0x0000000000000000
```

- **Kernel Space**: 
The upper portion of the address space is reserved for the kernel. 
This space is not accessible to user-space processes.

- **User Stack**: 
This is where the call stack is located. It grows downwards towards lower addresses. 
The stack is used for storing local variables, function parameters, return addresses, and for handling function calls.

- **Memory Mappings**: 
This area can include memory-mapped files, shared libraries, and other mappings.

- **Heap Segment**: 
This is where dynamically allocated memory (e.g., using `malloc` in C) resides. 
The heap grows upwards towards higher addresses.

- **BSS Segment**: 
The Block Started by Symbol segment contains uninitialized global and static variables.

- **Data Segment**: 
Contains initialized global and static variables.

- **Text Segment**: 
The lowest part of the address space where the actual machine code of the program resides.

Note that the exact layout can vary based on system configuration, compiler behavior, and runtime conditions. This representation is a simplification and does not cover every aspect of the Linux x86-64 process memory layout, such as specific alignment details, guard regions, or special mappings like the VDSO (Virtual Dynamically linked Shared Object).

请注意，确切的布局可能会根据系统配置、编译器行为和运行时条件而有所不同。
这种表示是一种简化，并没有涵盖Linux x86-64进程内存布局的每一个方面，比如特定的对齐细节、保护区域或特殊映射，如VDSO（虚拟动态链接共享对象）。

地址空间底部是保留给用户程序的，包括通常的代码、数据、堆和栈段。
代码段总是从地址 `0x400000` 开始。地址空间顶部保留给内核（操作系统常驻内存的部分）。
地址空间的这个部分包含内核在代表进程执行指令时（比如当应用程序执行系统调用时）使用的代码、数据和栈。


### 8.2.4 用户模式和内核模式

为了使操作系统内核提供一个无懈可击的进程抽象，处理器必须提供一种机制，限制一个应用可以执行的指令以及它可以访问的地址空间范围。

处理器通常是用某个控制寄存器中的一个模式位（*mode bit*）来提供这种功能的，该寄存器描述了进程当前享有的特权。
当设置了模式位时，进程就运行在内核模式中（有时叫做超级用户模式）。一个运行在内核模式的进程可以执行指令集中的任何指令，并且可以访问系统中的任何内存位置。

没有设置模式位(*mode bit*)时，进程就运行在用户模式中。用户模式中的进程不允许执行特权指令（*privileged instruction*），比如停止处理器、改变模式位，或者发起一个 I/O 操作。也不允许用户模式中的进程直接引用地址空间中内核区内的代码和数据。任何这样的尝试都会导致致命的保护故障。反之，用户程序必须通过系统调用接口间接地访问内核代码和数据。

运行应用程序代码的进程初始时是在用户模式中的。进程从用户模式变为内核模式的唯一方法是通过诸如中断、故障或者陷入系统调用这样的异常。当异常发生时，控制传递到异常处理程序，处理器将模式从用户模式变为内核模式。处理程序运行在内核模式中，当它返回到应用程序代码时，处理器就把模式从内核模式改回到用户模式。

Linux 提供了一种聪明的机制，叫做 `/proc` 文件系统，它允许用户模式进程访问内核数据结构的内容。
`/proc` 文件系统将许多内核数据结构的内容输出为一个用户程序可以读的文本文件的层次结构。
比如，你可以使用 `/proc` 文件系统找出一般的系统属性，比如 CPU 类型（`/proc/cpuinfo`），或者某个特殊的进程使用的内存段（`/proc/<process-id>/maps`）。
2.6 版本的 Linux 内核引入 `/sys` 文件系统，它输岀关于系统总线和设备的额外的低层信息。

===================================================================================================
Linux中的`/proc`和`/sys`文件系统是两个特殊的文件系统，提供了用户空间程序访问内核数据结构和系统信息的接口。
它们的设计理念是将系统和内核的信息以文件的形式呈现，使得用户和程序能以读取文件的方式获取系统数据。以下是更详细的解释：

### `/proc` 文件系统
1. **虚拟文件系统**：
`/proc`是一个虚拟文件系统，它不存储数据，而是在访问时动态生成文件内容。它主要表示进程和系统信息。
2. **进程信息**：每个正在运行的进程在`/proc`下都有一个以其进程ID命名的目录（如`/proc/1234`）
包含了该进程的详细信息，如内存映射（`/proc/<process-id>/maps`）、打开的文件描述符（`/proc/<process-id>/fd`）等。
3. **系统信息**：
`/proc`还包含了系统级别的信息，如CPU信息（`/proc/cpuinfo`）、内存使用情况（`/proc/meminfo`）、系统版本（`/proc/version`）等。
4. **交互性**：
某些`/proc`文件不仅可以读取，还可以写入，从而改变内核设置。例如，`/proc/sys`目录包含了可调整的内核参数。

### `/sys` 文件系统
1. **设备和驱动信息**：
`/sys`文件系统（也称为sysfs）提供了关于系统总线、设备和驱动的信息。它是2.6版本的Linux内核的一个特性。
2. **结构化展示**：
sysfs呈现了系统设备树的结构化视图，包括设备与总线、驱动之间的关系。
3. **细粒度控制**：
`/sys`提供了更细粒度的控制和信息，专注于设备和驱动程序的特性。例如，可以查看特定硬件组件的状态或配置特定设备的参数。
4. **硬件抽象**：
sysfs为系统中的硬件资源提供了一个统一的抽象，使得用户空间的程序能够轻松地与硬件交互。

### 用途和应用
- **系统监控**：通过读取`/proc`和`/sys`中的文件，可以监控和分析系统的运行状态和性能。
- **脚本和程序**：开发者和系统管理员经常通过编写脚本或程序来读取这些文件，以自动化地收集系统信息或调整系统配置。
- **调试和故障排除**：这两个文件系统在系统调试和故障排除中也非常有用，提供了深入了解系统内部的窗口。

总的来说，`/proc`和`/sys`文件系统是Linux提供的两个功能强大的工具
使用户能夠以简单直接的方式访问内核和系统级别的详细信息。它们的存在极大地简化了系统管理和监控任务。
===================================================================================================


### 上下文切换

操作系统内核使用一种称为上下文切换（`context switch`）的较高层形式的异常控制流来实现多任务。
上下文切换机制是建立在 8.1 节中已经讨论过的那些较低层异常机制之上的。

内核为每个进程维持一个上下文（*context*）。上下文就是内核重新启动一个被抢占的进程所需的状态。
它由一些对象的值组成，这些对象包括通用目的寄存器、浮点寄存器、程序计数器、用户栈、状态寄存器、内核栈和各种内核数据结构
比如描述地址空间的页表、包含有关当前进程信息的进程表，以及包含进程已打开文件的信息的文件表。

在进程执行的某些时刻，内核可以决定抢占当前进程，并重新开始一个先前被抢占了的进程。
这种决策就叫做调度（`scheduling`），是由内核中称为调度器（scheduler）的代码处理的。
当内核选择一个新的进程运行时，我们说内核调度了这个进程。

在内核调度了一个新的进程运行后，它就抢占当前进程，并使用一种称为上下文切换的机制来将控制转移到新的进程，上下文切换: 
    1）保存当前进程的上下文
    2）恢复某个先前被抢占的进程被保存的上下文
    3）将控制传递给这个新恢复的进程。

====================================================================================================
进程的上下文（context）是指在任意时刻进程所有状态信息的集合。
这个上下文对于操作系统在执行任务切换时正确保存和恢复进程状态是至关重要的。一个进程的上下文通常包含以下几个关键部分：

1. **CPU寄存器状态**：
   - **通用寄存器**：包括累加器、索引寄存器、栈指针和基指针等。
   - **程序计数器（PC）**：存储了下一条要执行的指令的地址。
   - **状态寄存器**：包括标志位，如零位、符号位、溢出位等，它们提供了有关前一指令执行结果的信息。

2. **程序的内存空间**：
   - **代码段（Text Segment）**：程序的机器语言代码。
   - **数据段（Data Segment）**：全局和静态变量。
   - **堆（Heap）**：动态分配的内存（如C中的malloc或C++中的new分配的内存）。
   - **栈（Stack）**：局部变量、函数参数、返回地址等。

3. **进程控制块（PCB）**：
   - 包括进程状态（运行、就绪、等待等）、进程ID、程序计数器、CPU寄存器和内存管理信息（如页面表或段表）。
   - 还可能包括账户信息（如CPU使用时间）、进程优先级、指向父进程和子进程的指针、进程间通信信息等。

4. **I/O状态信息**：
   - 包括打开文件的列表、分配给进程的文件描述符、用于网络通信的套接字等。
   - 有关文件系统的信息，如当前工作目录、根目录等。

5. **安全和权限信息**：
   - 用户ID（UID）和组ID（GID）。
   - 其他与安全相关的信息，如访问控制列表、安全策略等。

当操作系统执行进程切换时，它保存当前正在运行的进程的上下文，并恢复下一个要运行的进程的上下文。
这确保了每个进程都能从中断点恢复执行，就像它从未被中断过一样。这个机制是多任务操作系统中并发执行多个进程的基础。

====================================================================================================
页面表（Page Table）和段表（Segment Table）是操作系统用于内存管理的两种关键数据结构，它们分别用于实现基于页的内存管理和基于段的内存管理。这两种结构帮助操作系统将虚拟地址转换为物理地址。

### 页面表（Page Table）

1. **基于页的虚拟内存**：
在基于页的内存管理中，虚拟内存和物理内存被划分为大小固定的块
这些块称为“页”（在虚拟内存中）和“页帧”（在物理内存中）。

2. **作用**：
页面表是用于跟踪虚拟页与物理页帧之间映射关系的数据结构。每个进程都有自己的页面表。

3. **结构**：
页面表通常是一个数组或者更复杂的数据结构，如多级页面表。
表中的每个条目包含了虚拟页到物理页帧的映射以及一些控制位（如访问权限、是否已加载到物理内存等）。

4. **地址转换**：
当程序访问其虚拟地址空间时，操作系统使用页面表来找到对应的物理地址。
如果所需的页面不在物理内存中（发生了页面错误），操作系统会从磁盘中调入该页面。

### 段表（Segment Table）

1. **基于段的内存管理**：
在基于段的内存管理中，内存被划分为不同的逻辑段，如代码段、数据段、堆栈段等。每个段都有一个起始地址和长度。

2. **作用**：
段表用于存储每个内存段的基址（起始物理地址）、界限（段的长度）和一些状态信息（如访问权限）。

3. **结构**：
段表通常是一个数组，每个条目对应一个内存段。条目包含段的基址、段的长度和其他管理信息。

4. **地址转换**：
当程序访问其内存段时，操作系统使用段表来确定对应段的物理内存位置。
虚拟地址由段标识符和段内偏移量组成，操作系统将这些信息与段表中的数据结合起来，得到实际的物理地址。

### 页面表与段表的比较

- **页面表**侧重于将整个虚拟地址空间均匀地映射到物理内存，支持更细粒度的内存管理。

- **段表**侧重于将内存分割为逻辑上不同的部分，反映了程序的逻辑结构。

现代操作系统通常采用基于页的管理方式，但也有一些系统结合了段和页两种机制，例如x86架构的早期版本。
在这些系统中，虚拟地址首先通过段表转换，然后通过页面表进行二次转换以定位物理地址。

让我们通过具体的例子来说明页面表和段表的工作原理：

### 页面表的例子

假设我们有一个简单的系统，其中虚拟内存和物理内存的大小都是 16KB，页和页帧的大小都是 4KB。
这意味着我们有4个虚拟页（0-3）和4个物理页帧（0-3）。

1. **页面表结构**：每个进程都有一个页面表，它可以是这样的简单数组：

   ```
   页面表:
   虚拟页 | 物理页帧
   ---------------
    0    |   2
    1    |   0
    2    |   3
    3    |   1
   ```

   在这个例子中，虚拟页0映射到物理页帧2，虚拟页1映射到物理页帧0，依此类推。

2. **地址转换**：如果一个程序想要访问虚拟地址`0x0000`（位于虚拟页0），系统会查看页面表，发现虚拟页0对应物理页帧2。
因此，虚拟地址`0x0000`实际上映射到物理地址`0x8000`（假设页帧2从物理地址`0x8000`开始）。

### 段表的例子

考虑一个将内存分为三个段的简单系统：代码段、数据段和栈段。每个段有一个基地址和长度。

1. **段表结构**：段表可能如下所示：

   ```
   段表:
   段    | 基址   | 长度
   ---------------
   代码  | 0x0000 | 0x4000
   数据  | 0x4000 | 0x2000
   栈    | 0x6000 | 0x2000
   ```

   在这个例子中，代码段从`0x0000`开始，长度为`0x4000`；
   数据段从`0x4000`开始，长度为`0x2000`；栈段从`0x6000`开始，同样长度为`0x2000`。

2. **地址转换**：如果一个程序想要访问数据段中的第一个字节（在段内偏移为0），并且数据段的虚拟地址为`0x4000`，
那么系统将直接使用段表中的基址将此虚拟地址转换为物理地址`0x4000`。

在实际的操作系统中，这些机制通常要复杂得多，并且可能会结合使用。
例如，现代x86架构的操作系统通常在段表的基础上使用分页，这意味着地址首先通过段表转换，然后再通过页面表进行二次转换，以确定最终的物理地址。
====================================================================================================

当内核代表用户执行系统调用时，可能会发生上下文切换。如果系统调用因为等待某个事件发生而阻塞，那么内核可以让当前进程休眠，切换到另一个进程。
比如，如果一个 `read` 系统调用需要访问磁盘，内核可以选择执行上下文切换，运行另外一个进程，而不是等待数据从磁盘到达。
另一个示例是 `sleep` 系统调用，它显式地请求让调用进程休眠。一般而言，即使系统调用没有阻塞，内核也可以决定执行上下文切换，而不是将控制返回给调用进程。

中断也可能引发上下文切换。比如，所有的系统都有某种产生周期性定时器中断的机制，通常为每 1 毫秒或每 10 毫秒。
每次发生定时器中断时，内核就能判定当前进程已经运行了足够长的时间，并切换到一个新的进程。

图 8-14 展示了一对进程 A 和 B 之间上下文切换的示例。
在这个例子中，进程 A 初始运行在用户模式中，直到它通过执行系统调用 read 陷入到内核。
内核中的陷阱处理程序请求来自磁盘控制器的 DMA 传输，并且安排在磁盘控制器完成从磁盘到内存的数据传输后，磁盘中断处理器。

磁盘取数据要用一段相对较长的时间（数量级为几十毫秒），所以内核执行从进程 A 到进程 B 的上下文切换，而不是在这个间歇时间内等待，什么都不做。
注意在切换之前，内核正代表进程 A 在用户模式下执行指令（即没有单独的内核进程）。
在切换的第一部分中，内核代表进程 A 在内核模式下执行指令。然后在某一时刻，它开始代表进程 B（仍然是内核模式下）执行指令。
在切换之后，内核代表进程 B 在用户模式下执行指令。

随后，进程 B 在用户模式下运行一会儿，直到磁盘发出一个中断信号，表示数据已经从磁盘传送到了内存。
内核判定进程 B 已经运行了足够长的时间，就执行一个从进程 B 到进程 A 的上下文切换，将控制返回给进程 A 中紧随在系统调用 read 之后的那条指令。
进程 A 继续运行，直到下一次异常发生，依此类推。
