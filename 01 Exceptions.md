# Exceptions
# 异常

异常是异常控制流的一种形式，它一部分由硬件实现，一部分由操作系统实现。因为它们有一部分是由硬件实现的，所以具体细节将随系统的不同而有所不同。

然而，对于每个系统而言，基本的思想都是相同的。在这一节中我们的目的是让你对异常和异常处理有一个一般性的了解，并且向你揭示现代计算机系统的一个经常令人感到迷惑的方面。

异常（exception）就是控制流中的突变，用来响应处理器状态中的某些变化。图 8-1 展示了基本的思想

> Anatomy of an exception. 
> A change in the processor’s state (an event) triggers an abrupt control transfer (an exception) from the application program to an exception handler. 
> After it finishes processing, the handler either returns control to the interrupted program or aborts.

> 处理器状态中的变化（事件）触发从应用程序到异常处理程序的突发的控制转移（异常）。
> 在异常处理程序完成处理后，它将控制返回给被中断的程序或者终止

在图中，当处理器状态中发生一个重要的变化时，处理器正在执行某个当前指令 `I_curr` 。
在处理器中，状态被编码为不同的位和信号。状态变化称为事件（`event`）. 事件可能和当前指令的执行直接相关。比如，发生虚拟内存缺页、算术溢出，或者一条指令试图除以零。另一方面，事件也可能和当前指令的执行没有关系。比如，一个系统定时器产生信号或者一个 I/O 请求完成。

在任何情况下，当处理器检测到有事件发生时，它就会通过一张叫做异常表（`exception table`）的跳转表，进行一个间接过程调用（indirect procedure call异常），到一个专门设计用来处理这类事件的操作系统子程序（异常处理程序（exception handler））.
当异常处理程序完成处理后，根据引起异常的事件的类型，会发生以下 3 种情况中的一种：

 - 处理程序将控制返回给当前指令 `I_curr` ，即当事件发生时正在执行的指令。
 - 处理程序将控制返回给 `I_next` ，如果没有发生异常将会执行的下一条指令。
 - 处理程序终止被中断的程序。

> 8.1.2 节将讲述关于这些可能性的更多内容。
> 旁注 - 硬件异常与软件异常
> C++ 和 Java 的程序员会注意到术语 “异常” 也用来描述由 C++ 和 Java 以 catch、throw 和 try 语句形式提供的应用级 ECF。
> 如果想严格清晰，我们必须区别 “硬件” 和“软件” 异常，但这通常是不必要的，因为从上下文中就能够很清楚地知道是哪种含义。

## 8.1.1 异常处理

异常可能会难以理解，因为处理异常需要硬件和软件紧密合作。很容易搞混哪个部分执行哪个任务。让我们更详细地来看看硬件和软件的分工吧。

系统中可能的每种类型的异常都分配了一个唯一的非负整数的异常号（`exception number`）。
其中一些号码是由处理器的设计者分配的，其他号码是由操作系统内核（操作系统常驻内存的部分）的设计者分配的。
前者的示例包括被`零除、缺页、内存访问违例、断点以及算术运算溢出`。
后者的示例包括`系统调用`和来自`外部 I/O 设备的信号`。

在系统启动时（当计算机重启或者加电时），操作系统分配和初始化一张称为异常表的跳转表，使得表目 k 包含异常 k 的处理程序的地址
图 8-2 展示了异常表的格式。

异常表是一张跳转表，其中表目 k 包含异常 k 的处理程序代码的地址

Here is an ASCII representation of an exception handling table that might be visible in a shell or a terminal environment. 
This table is a simplified and generic version, as the actual structure and content can vary depending on the specific system or application.

```shell
+-------------------+------------------+----------------------------+
| Exception Type    | Exception Code   | Exception Handler Address  |
+-------------------+------------------+----------------------------+
| Divide by Zero    | 0x00             | 0x0040FFF1                 |
+-------------------+------------------+----------------------------+
| Invalid Operation | 0x01             | 0x0040FFB2                 |
+-------------------+------------------+----------------------------+
| Overflow          | 0x02             | 0x0040FAB4                 |
+-------------------+------------------+----------------------------+
| Segmentation Fault| 0x03             | 0x0040FBC4                 |
+-------------------+------------------+----------------------------+
| Access Violation  | 0x04             | 0x0040FCC5                 |
+-------------------+------------------+----------------------------+
| Illegal Instruction| 0x05            | 0x0040FD8D                 |
+-------------------+------------------+----------------------------+
| Stack Overflow    | 0x06             | 0x0040FE9E                 |
+-------------------+------------------+----------------------------+
```

This table consists of three columns:
- **Exception Type**: Describes the type of exception (e.g., divide by zero, invalid operation).
- **Exception Code**: A unique code identifying the exception. This is often a hexadecimal value.
- **Exception Handler Address**: The memory address of the exception handler that should be called when the exception occurs.

Note: The values and addresses in this table are for illustrative purposes only and do not represent real exception codes or handler addresses. In a real system, these values would be specific to the architecture and the operating system's implementation of exception handling.

在运行时（当系统在执行某个程序时），处理器检测到发生了一个事件，并且确定了相应的异常号 `k`。
随后，处理器触发异常，方法是*执行间接过程调用*，通过异常表的表目 `k`，转到相应的处理程序。
图 8-3 展示了处理器如何使用异常表来形成适当的异常处理程序的地址。
异常号是到异常表中的索引，异常表的起始地址放在一个叫做异常表基址寄存器（`exception table base register`）的特殊 CPU 寄存器里。

[Exception Table Base Register] (+) [Exception number]  -> [Address of entry for exception #k]

====================================================================================================
The Exception Table Base Register (ETBR) is a concept primarily found in certain types of computer architectures, such as those with explicit support for hardware-based exception handling mechanisms. 
The ETBR is a register in the CPU that holds the base address of the exception handling table, a critical data structure used in the process of handling exceptions.

### Understanding Exception Table and ETBR

1. **Exception Table**: This table contains information about how to handle various exceptions (such as interrupts, faults, and traps) that can occur during the execution of a program. Each entry in the table typically includes a pointer to an exception handler routine, along with other relevant data.

2. **Role of ETBR**: The Exception Table Base Register points to the base address (starting point) of this exception table in memory. When an exception occurs, the processor uses the ETBR to quickly find and access the exception table and determine the appropriate action, which usually involves jumping to an exception handler routine.

### How it Works

1. **Exception Occurrence**: When an exception occurs (e.g., a division by zero, page fault, or invalid instruction), the processor needs to transfer control to the appropriate handler to address the issue.

2. **Accessing the Exception Table**: The processor uses the address stored in the ETBR to access the exception table. The type of exception that occurred determines which entry in the table to use.

3. **Determining the Handler**: Each entry in the exception table corresponds to a specific type of exception and contains the address of the handler routine for that exception. 

4. **Handler Execution**: The processor then transfers control to the specified exception handler routine. This routine will execute the necessary steps to handle the exception, which might involve correcting an error, terminating a faulty process, or simply logging the event.

5. **Return from Handler**: After the handler has dealt with the exception, control is typically returned to the point in the program where the exception occurred, or to a different specified location, depending on the nature of the exception and the handling strategy.

### Importance

- **Efficiency**: Using an ETBR allows for a more efficient and organized approach to exception handling. The processor can quickly access the relevant handler without the need for extensive searching or complex decision-making logic.

- **Modularity**: The exception table provides a modular approach to exception handling. Handlers can be updated or changed without affecting the overall program flow.

- **Reliability**: This structured approach to handling exceptions is crucial for the stability and reliability of software, especially in systems where unexpected events must be carefully managed, such as in operating systems or embedded systems.

### Architecture Specific

It's important to note that the implementation and use of an ETBR can vary greatly between different CPU architectures and operating systems. In some systems, the concept might be implemented differently or not used at all, depending on the design of the exception handling mechanism.


异常表基地址寄存器（ETBR）主要出现在某些类型的计算机架构中，这些架构显式支持基于硬件的异常处理机制。ETBR是CPU中的一个寄存器，它保存着异常处理表的基地址，这是处理异常过程中的关键数据结构。

### 理解异常表和ETBR

1. **异常表**：该表包含了如何处理程序执行过程中可能发生的各种异常（例如中断、故障和陷阱）的信息。表中的每个条目通常包括一个指向异常处理程序的指针以及其他相关数据。

2. **ETBR的作用**：异常表基地址寄存器指向内存中此异常表的基地址（起始点）。当发生异常时，处理器使用ETBR快速找到并访问异常表，并确定适当的操作，通常涉及跳转到一个异常处理程序。

### 它是如何工作的

1. **异常发生**：当发生异常（例如除零、页面错误或无效指令）时，处理器需要将控制权转移给适当的处理程序以解决问题。

2. **访问异常表**：处理器使用ETBR中存储的地址来访问异常表。发生的异常类型决定了使用表中的哪个条目。

3. **确定处理程序**：异常表中的每个条目对应于特定类型的异常，并包含该异常的处理程序的地址。

4. **处理程序执行**：处理器随后将控制权转移到指定的异常处理程序。该程序将执行处理异常所需的步骤，可能涉及纠正错误、终止错误进程或简单地记录事件。

5. **从处理程序返回**：处理程序处理完异常后，通常将控制权返回到程序中发生异常的点，或根据异常的性质和处理策略返回到其他指定位置。

### 重要性

- **效率**：使用ETBR允许更有效且有组织的异常处理方式。处理器可以快速访问相关处理程序，无需进行广泛搜索或复杂的决策逻辑。

- **模块化**：异常表提供了一种模块化的异常处理方法。可以更新或更改处理程序，而不影响整体程序流。

- **可靠性**：这种结构化的异常处理方法对于软件的稳定性和可靠性至关重要，尤其是在必须仔细管理意外事件的系统中，例如操作系统或嵌入式系统。

### 架构特定

值得注意的是，ETBR的实现和使用在不同的CPU架构和操作系统之间可能有很大差异。在某些系统中，这个概念可能有所不同，或根据异常处理机制的设计完全不使用。
====================================================================================================


异常类似于过程调用，但是有一些重要的不同之处：

1. **返回地址的处理**：
   - 在常规的过程调用中，当跳转到一个函数或过程时，处理器会自动将返回地址（即调用指令后的下一条指令的地址）压入栈中。这样，函数执行完毕后，可以通过这个地址返回到调用点。
   - 对于异常处理，返回地址的确定取决于异常的类型和发生时机。如果异常发生在指令执行过程中（例如，执行过程中发生了除零错误），返回地址可能是当前指令的地址；如果是在指令执行完成后发生的异常（例如，执行完指令后发生了页面错误），返回地址可能是下一条指令的地址。

2. **处理器状态的保存**：
   - 当发生异常时，除了返回地址外，处理器还会保存当前的处理器状态到栈中。这包括各种寄存器的内容，如程序计数器（PC）、栈指针（SP）、状态寄存器（如 x86-64 架构中的 EFLAGS），以及其他可能影响程序执行的信息。
   - 这些信息的保存对于异常处理后能够正确地恢复到异常发生前的程序状态是必要的。

3. **栈的使用**：
   - 在用户模式下发生异常时，由于控制权需要转移到内核（以执行异常处理程序），因此异常相关的信息（如返回地址、处理器状态等）被保存到内核栈上，而不是用户栈上。
   - 这是为了安全和隔离的考虑，确保用户程序不能直接访问或修改这些敏感的内核信息。

4. **异常处理程序的权限**：
   - 异常处理程序通常在内核模式下运行，这使得它们拥有对系统所有资源的完全访问权限。这一点与用户模式下运行的普通程序不同。
   - 这种高权限的运行环境允许异常处理程序执行必要的系统级操作，如访问硬件资源、管理内存等。

总的来说，异常处理在计算机系统中是一种特殊且重要的机制，它允许系统有效地应对各种非预期情况，确保系统的稳定和安全运行。 


一旦硬件触发了异常，剩下的工作就是由异常处理程序在软件中完成。

在处理程序处理完事件之后，它通过执行一条特殊的“从中断返回 `return from interrupt`”指令，可选地返回到被中断的程序，该指令将适当的状态弹回到处理器的控制和数据寄存器中，如果异常中断的是一个用户程序，就将状态恢复为用户模式（见 8.2.4 节），然后将控制返回给被中断的程序。


## 8.1.2 异常的类别

异常可以分为四类：中断（interrupt），陷阱（trap）、故障（fault）和 终止（abort）。
图 8-4 中的表对这些类别的属性做了小结。

`Class`     `Cause`                         `Async/sync`    `Return behavior`
Interrupt   Signal from I/O device          Async           Always returns to next instruction
Trap        Intentional exception           Sync            Always returns to next instruction
Fault       Potentially recoverable error   Sync            Might return to current instruction
Abort       Nonrecoverable error            Sync            Never returns

异步异常是由处理器外部的 I/O 设备中的事件产生的。同步异常是执行一条指令的直接产物

1. **中断（Interrupt）**：
   - **原因**：来自I/O设备的信号。
   - **异步/同步（Async/sync）**：异步。中断可以在程序执行的任何时间点发生，通常由外部事件触发，如用户输入、计时器到期或网络活动。
   - **返回行为**：总是返回到下一条指令。中断处理完成后，程序从中断发生时的下一条指令继续执行。

2. **陷阱（Trap）**：
   - **原因**：有意触发的异常。
   - **异步/同步**：同步。陷阱通常是由程序显式请求的，如系统调用或断点。
   - **返回行为**：总是返回到下一条指令。陷阱处理完成后，程序通常从陷阱指令之后的下一条指令继续执行。

3. **故障（Fault）**：
   - **原因**：可能可恢复的错误。
   - **异步/同步**：同步。故障是由程序执行中的错误条件触发的，如分页错误或访问违规。
   - **返回行为**：可能返回到当前指令。如果故障可以被修正（如加载缺失的页面），程序可能会重新执行导致故障的指令。

4. **中止（Abort）**：
   - **原因**：不可恢复的错误。
   - **异步/同步**：同步。中止是由严重的错误条件触发的，如硬件故障或一致性检查失败。
   - **返回行为**：永远不会返回。中止通常意味着程序无法继续执行，可能需要系统级别的干预或程序的终止。

`1. 中断`
中断是异步发生的，是来自处理器外部的 I/O 设备的信号的结果。硬件中断不是由任何一条专门的指令造成的，从这个意义上来说它是异步的。
硬件中断的异常处理程序常常称为中断处理程序（`interrupt handler`）。
I/O 设备，例如网络适配器、磁盘控制器和定时器芯片，通过向处理器芯片上的一个引脚发信号，并将异常号放到系统总线上，来触发中断，这个异常号标识了引起中断的设备。

在当前指令完成执行之后，处理器注意到中断引脚的电压变高了，就从系统总线读取异常号，然后调用适当的中断处理程序。
当处理程序返回时，它就将控制返回给下一条指令（也即如果没有发生中断，在控制流中会在当前指令之后的那条指令）。
结果是程序继续执行，就好像没有发生过中断一样。

剩下的异常类型（陷阱、故障和终止）是同步发生的，是执行当前指令的结果。我们把这类指令叫做故障指令（`faulting instruction`）。

`2. 陷阱和系统调用 (Traps and System Calls)`

陷阱是有意的异常，是执行一条指令的结果。就像中断处理程序一样，陷阱处理程序将控制返回到下一条指令。
`Trap`最重要的用途是在用户程序和内核之间提供一个像过程一样的接口，叫做系统调用。

用户程序经常需要向内核请求服务，比如读一个文件（`read`）、创建一个新的进程（`fork`），加载一个新的程序（`execve`），或者终止当前进程（`exit`）。
为了允许对这些内核服务的受控的访问，处理器提供了一条特殊的 “`syscall n`” 指令，当用户程序想要请求服务 `n` 时，可以执行这条指令。
执行 `syscall` 指令会导致一个到异常处理程序的陷阱，这个处理程序解析参数，并调用适当的内核程序。

The trap handler returns control to the next instruction in the application program’s control flow.

从程序员的角度来看，系统调用和普通的函数调用是一样的。然而，它们的实现非常不同。
普通的函数运行在用户模式中，用户模式限制了函数可以执行的指令的类型，而且它们只能访问与调用函数相同的栈。
系统调用运行在内核模式中，内核模式允许系统调用执行特权指令，并访问定义在内核中的栈。

====================================================================================================
系统调用和普通的函数调用在多个方面有显著的不同，这些不同主要源于它们的用途、执行方式、效率和安全性方面的差异：

1. **用途和功能**：
   - **系统调用**：系统调用是程序与操作系统交互的接口。它们允许用户程序执行诸如文件操作、进程控制、网络通信等不能直接在用户空间执行的操作，因为这些操作需要更高级别的权限和对硬件的直接访问。
   - **普通函数调用**：普通函数调用通常用于执行用户程序内的任务，如计算、数据处理、内存分配等，这些操作不需要直接访问硬件或操作系统的内核资源。

2. **执行模式**：
   - **系统调用**：执行系统调用时，CPU从用户模式切换到内核模式。这是因为系统调用通常需要访问受保护的内核资源。
   - **普通函数调用**：执行普通函数调用时，CPU保持在用户模式，不涉及模式切换。

3. **性能和开销**：
   - **系统调用**：由于涉及模式切换（从用户模式到内核模式），系统调用的开销比普通函数调用大。这个模式切换是必要的，但它增加了执行时间。
   - **普通函数调用**：执行普通函数调用的开销较小，因为它仅在用户空间内进行，没有额外的上下文切换或安全检查。

4. **安全性和隔离**：
   - **系统调用**：系统调用提供了一种安全的机制，允许用户程序请求操作系统服务，同时保护操作系统免受恶意程序的影响。操作系统通过系统调用对用户程序的请求进行管理和限制。
   - **普通函数调用**：在用户程序的上下文中执行，不涉及对操作系统核心部分的直接访问，因此相对来说不需要严格的安全检查。

5. **接口和实现**：
   - **系统调用**：系统调用的接口和实现是由操作系统提供的。这些调用通常是操作系统API的一部分，且在不同的操作系统间可能不兼容。
   - **普通函数调用**：普通函数可以由用户自定义或使用标准库提供的函数。这些函数通常是跨平台的，与操作系统的具体实现无关。

综上所述，系统调用和普通函数调用在用途、执行方式、效率和安全性方面都有明显的区别。系统调用为用户程序提供了一种访问操作系统服务和资源的方式，但这种访问是受控的，并且比普通函数调用有更高的开销。
====================================================================================================

`3.Faults 故障`

故障由错误情况引起，它可能能够被故障处理程序修正。
当故障发生时，处理器将控制转移给故障处理程序。如果处理程序能够修正这个错误情况，它就将控制返回到引起故障的指令，从而重新执行它。
否则，处理程序返回到内核中的 `abort` 例程，abort 例程会终止引起故障的应用程序

一个经典的故障示例是缺页异常 `page fault exception`，当指令引用一个虚拟地址，而与该地址相对应的物理页面不在内存中，因此必须从磁盘中取出时，就会发生故障。
就像我们将在第 9 章中看到的那样，一个页面就是虚拟内存的一个连续的块（典型的是 *4KB*）。
缺页处理程序从磁盘加载适当的页面，然后将控制返回给引起故障的指令。当指令再次执行时，相应的物理页面已经驻留在内存中了，指令就可以没有故障地运行完成了。

When a page fault exception occurs, it indicates that a program has attempted to access a portion of memory that is not currently in its address space, or it lacks the proper permissions to access it. Here’s what happens in a typical scenario when a page fault occurs:

1. **Attempted Memory Access**: The program tries to access a memory location that is not present in the physical memory (RAM). This could be because the data is not loaded yet (not present), or the memory access is invalid (protection violation).

2. **Interrupt and Save State**: The CPU recognizes that the requested page is not in RAM and triggers a page fault interrupt. This interrupt causes the CPU to stop its current operations and save the state of the process. This includes the program counter, registers, and other critical data so that the process can be resumed later.

3. **Transfer Control to Kernel**: Control is transferred to the operating system kernel. The kernel has an interrupt handler for page faults, which begins to execute.

4. **Determine the Cause**: The kernel examines the address that caused the fault and the fault status to determine the cause of the page fault. It checks if the fault is due to a non-existent page (the page is not in memory) or a permission violation (the process does not have the right to access the page).

5. **Handling the Fault**:
   - **If the Page is Not in Memory**: The kernel tries to resolve the fault by locating the required data. If the data is in a swap area (or page file) on the disk, the kernel allocates a frame in physical memory, reads the data into that frame from the disk, and then updates the process's page table to include the new mapping.
   - **If It's a Protection Violation**: The kernel determines if the access was legitimate. If not, it may terminate the process (e.g., with a segmentation fault in Unix-like systems) or send a signal to it.

6. **Resuming Execution**: After handling the fault, the kernel returns control to the process, adjusting the program counter to retry the instruction that caused the fault. If the kernel successfully resolved the fault by loading the page into memory, the instruction will succeed. If not (e.g., in case of an illegal access), the process may be terminated or receive an error signal.

7. **Continuation or Termination**: The process either continues execution normally if the fault was handled successfully, or it is terminated if the fault was due to an illegal operation or cannot be resolved.

The handling of page faults is a crucial aspect of virtual memory systems in modern operating systems. It allows systems to use disk storage as an extension of RAM, enables memory protection, and supports more applications running concurrently than would otherwise be possible with the available physical memory.

当发生页面错误异常时，这表明程序尝试访问当前不在其地址空间内的内存部分，或者它缺乏访问该部分的适当权限。以下是页面错误发生时的典型场景：

1. **尝试内存访问**：程序试图访问物理内存（RAM）中不存在的内存位置。这可能是因为数据尚未加载（不存在），或者内存访问无效（权限违规）。

2. **中断并保存状态**：CPU识别出请求的页面不在RAM中，并触发页面错误中断。这个中断使CPU停止当前操作并保存进程的状态。这包括程序计数器、寄存器和其他关键数据，以便稍后可以恢复进程。

3. **转移控制权给内核**：控制权转移到操作系统内核。内核有一个页面错误的中断处理程序，开始执行。

4. **确定原因**：内核检查导致故障的地址和故障状态，以确定页面错误的原因。它检查错误是由于不存在的页面（页面不在内存中）还是权限违规（进程没有权利访问该页面）。

5. **处理故障**：
   - **如果页面不在内存中**：内核尝试通过定位所需数据来解决故障。如果数据在磁盘上的交换区（或页面文件）中，内核在物理内存中分配一个帧，从磁盘读取数据到该帧中，然后更新进程的页面表以包含新的映射。
   - **如果是权限违规**：内核确定访问是否合法。如果不是，它可能会终止进程（例如，在类Unix系统中引发段错误）或向其发送信号。

6. **恢复执行**：处理故障后，内核将控制权返回给进程，调整程序计数器以重试导致故障的指令。如果内核通过将页面加载到内存中成功解决了故障，指令将成功执行。如果不是（例如，非法访问的情况），进程可能会被终止或收到错误信号。

7. **继续或终止**：如果成功处理了故障，进程将正常继续执行；如果故障是由于非法操作或无法解决的原因，进程将被终止。

页面错误的处理是现代操作系统中虚拟内存系统的一个关键方面。它允许系统将磁盘存储作为RAM的扩展，实现内存保护，并支持更多应用程序同时运行，超出了可用物理内存的限制。

### *页与帧*
在计算机内存管理中，“帧”和“页面”是两个密切相关但不同的概念，通常用于描述虚拟内存系统的组成部分。理解它们之间的区别对于理解如何有效地管理和使用计算机内存非常重要。

1. **页面（Page）**：
   - 页面是虚拟内存中的一个基本单位。它是操作系统用来管理程序内存的一个固定大小的区块。
   - 页面的大小通常是固定的，例如在许多系统中为4KB。
   - 当程序运行时，它们使用虚拟地址来访问内存。这些虚拟地址被分割成页面。

2. **帧（Frame）**：
   - 帧是物理内存（即实际的RAM）中的一个基本单位，用于存储数据和指令。
   - 帧的大小与页面的大小相匹配，以便将虚拟内存页面直接映射到物理内存的帧。
   - 物理内存被分割成这样的帧，操作系统将虚拟页面映射到这些帧上。

### 区别

- **地址空间**：页面存在于虚拟地址空间中，这是一个由操作系统提供的抽象，使每个程序都像拥有整个内存一样运行。相比之下，帧存在于物理地址空间中，即实际的RAM。

- **用途**：页面用于程序的虚拟内存分配，而帧用于在物理内存中实际存储这些页面的内容。

- **映射关系**：在虚拟内存系统中，页面被映射到帧。一个页面可能不总是映射到一个固定的帧；它可以根据需要被调入或调出到磁盘。

### 总结

简而言之，页面是虚拟内存的一个逻辑单元，而帧是物理内存的一个物理单元。
虚拟内存管理涉及将这些虚拟页面映射到物理帧的过程，这是现代操作系统中内存管理的关键部分。
通过这种映射，操作系统可以有效地管理内存，提高内存使用效率，支持更多的应用程序并发运行，同时保护每个程序的内存空间不被其他程序干扰。


`4. 终止 Aborts`
终止是不可恢复的致命错误造成的结果，通常是一些硬件错误，比如 DRAM 或者 SRAM 位被损坏时发生的奇偶错误。
终止处理程序从不将控制返回给应用程序。如图 8-8 所示，处理程序将控制返回给一个 `abort` 例程，该例程会终止这个应用程序。


## 8.1.3 Linux/x86-64 系统中的异常

为了使描述更具体，让我们来看看为 x86-64 系统定义的一些异常。有高达 256 种不同的异常类型【50】。
0 ∼ 31 的号码对应的是由 Intel 架构师定义的异常，因此对任何 x86-64 系统都是一样的。
32 ∼ 255 的号码对应的是操作系统定义的中断和陷阱。图 8-9 展示了一些示例。

图 8-9 x86-64 系统中的异常示例
```shell
+-------------------+------------------+----------------------------+
| Exception Type    | Exception Code   | Exception Handler Address  |
+-------------------+------------------+----------------------------+
| Divide by Zero    | 0x00             | 0x0040FFF1                 |
+-------------------+------------------+----------------------------+
| Invalid Operation | 0x01             | 0x0040FFB2                 |
+-------------------+------------------+----------------------------+
| Overflow          | 0x02             | 0x0040FAB4                 |
+-------------------+------------------+----------------------------+
| Segmentation Fault| 0x03             | 0x0040FBC4                 |
+-------------------+------------------+----------------------------+
| Access Violation  | 0x04             | 0x0040FCC5                 |
+-------------------+------------------+----------------------------+
| Illegal Instruction| 0x05            | 0x0040FD8D                 |
+-------------------+------------------+----------------------------+
| Stack Overflow    | 0x06             | 0x0040FE9E                 |
+-------------------+------------------+----------------------------+
```

1. Linux/x86-64 故障和终止

 - 除法错误。当应用试图除以零时，或者当一个除法指令的结果对于目标操作数来说太大了的时候，就会发生除法错误（异常 0）。
   Unix 不会试图从除法错误中恢复，而是选择终止程序。
   Linuxshell 通常会把除法错误报告为“浮点异常（Floating exception）”。

 - 一般保护故障。许多原因都会导致不为人知的一般保护故障（异常 13），通常是因为一个程序引用了一个未定义的虚拟内存区域，或者因为程序试图写一个只读的文本段。
   Linux 不会尝试恢复这类故障。Linux shell 通常会把这种一般保护故障报告为“段故障（Segmentation fault）”。

 - 缺页（异常 14）是会重新执行产生故障的指令的一个异常示例。
   处理程序将适当的磁盘上虚拟内存的一个页面映射到物理内存的一个页面，然后重新执行这条产生故障的指令。我们将在第 9 章中看到缺页是如何工作的细节。

 - 机器检查。机器检查（异常 18）是在导致故障的指令执行中检测到致命的硬件错误时发生的。机器检查处理程序从不返回控制给应用程序。


在计算机编程中，"分段错误"（Segmentation Fault），通常被称为 segfault，是一个特定类型的错误，指示程序试图访问它的内存地址空间中不允许或不存在的区域。这通常是由于错误的指针操作或数组越界等原因造成的。以下是分段错误的一些关键方面：

====================================================================================================
### 原因
1. **无效的指针访问**：访问未初始化的指针、空指针或已经释放的内存。
2. **数组越界**：访问数组的边界之外的元素。
3. **栈溢出**：例如，由于深度递归调用导致的栈空间耗尽。
4. **访问受保护的内存区域**：例如，试图写入只读内存区域。
5. **内存损坏**：程序的某些部分意外改变了内存的关键部分（例如，破坏了内存分配的数据结构）。

### 行为
- 当程序试图进行非法内存访问时，操作系统检测到这一行为并将其终止，通常是通过向程序发送一个信号（在Unix-like系统中是SIGSEGV信号）。
- 分段错误通常会导致程序异常终止，而不会给出详细的错误原因或位置。

### 调试和修复
- **核心转储（Core Dump）**：在分段错误发生时，一些系统会产生一个核心转储文件。这个文件包含了程序崩溃时的内存快照，可以用于调试。
- **调试器**：使用调试器（如 GDB）可以帮助开发者找到导致分段错误的具体代码位置。
- **代码审查**：检查代码中可能导致分段错误的常见原因，如指针操作和数组访问。

### 总结

分段错误是一种常见的编程错误，通常与非法内存访问有关。它提示程序员代码中可能存在严重的逻辑错误，这些错误需要调试和修复以确保程序的稳定性和安全性。正确地管理内存访问和使用高级语言的安全特性可以帮助减少分段错误的发生。 
====================================================================================================

### 2. Linux/86-64 系统调用

Linux 提供几百种系统调用，当应用程序想要请求内核服务时可以使用，包括读文件、写文件或是创建一个新进程。
图 8-10 给出了一些常见的 Linux 系统调用。每个系统调用都有一个唯一的整数号，对应于一个到内核中跳转表的偏移量。（注意：这个跳转表和异常表不一样。）

在Linux系统中，系统调用是内核提供给用户空间程序的接口，允许它们执行各种操作，如文件操作、进程控制、网络通信等。
下面是一份简化的Linux系统调用表示例，包含一些常用的系统调用及其基本功能描述：

| 系统调用编号 | 系统调用名称       | 描述                                |
|-------------|-------------------|------------------------------------|
| 0           | `read`            | 从文件中读取数据                    |
| 1           | `write`           | 向文件写入数据                      |
| 2           | `open`            | 打开一个文件                        |
| 3           | `close`           | 关闭一个文件描述符                  |
| 4           | `stat`            | 获取文件状态                        |
| 5           | `fstat`           | 获取打开文件的状态                  |
| 6           | `lstat`           | 获取符号链接的状态                  |
| 7           | `poll`            | 等待某个事件在一组文件描述符上发生  |
| 8           | `lseek`           | 改变文件的读写位置                  |
| 9           | `mmap`            | 映射文件或设备到内存                |
| 10          | `mprotect`        | 设置内存保护                        |
| 11          | `munmap`          | 取消内存映射                        |
| 12          | `brk`             | 改变数据段的大小                    |
| 13          | `rt_sigaction`    | 检查或修改信号处理                  |
| 14          | `rt_sigprocmask`  | 检查或修改屏蔽信号                  |
| 15          | `ioctl`           | 设备特定的操作                      |
| 16          | `pread64`         | 从文件中带偏移量地读取数据          |
| 17          | `pwrite64`        | 向文件中带偏移量地写入数据          |
| 18          | `readv`           | 散列读操作                          |
| 19          | `writev`          | 散列写操作                          |
| 20          | `access`          | 检查用户对文件的访问权限            |
| 21          | `pipe`            | 创建管道                            |
| 22          | `select`          | 等待多个文件描述符变得可读或可写    |
| 23          | `sched_yield`     | 让出CPU                             |
| 24          | `mremap`          | 重新映射动态内存                    |
| 25          | `msync`           | 同步内存映射文件的内容              |
| 26          | `mincore`         | 检查内存中页面是否已经被加载        |
| 27          | `madvise`         | 提供内存使用建议                    |
| 28          | `shmget`          | 获取共享内存段                      |
| 29          | `shmat`           | 连接共享内存段                      |
| 30          | `shmctl`          | 共享内存控制                        |

请注意，这只是一份简化的列表，Linux有数百个系统调用，每个调用都有其特定的参数和功能。
系统调用的实际编号和可用性可能会根据Linux的不同版本和架构有所不同。 若要查看完整的系统调用列表和详细信息，通常可以查阅Linux内核文档或使用如 `man 2 syscalls` 这样的命令行工具。

C 程序用 `syscall` 函数可以直接调用任何系统调用。然而，实际中几乎没必要这么做。对于大多数系统调用，标准 C 库提供了一组方便的包装函数。
这些包装函数将参数打包到一起，以适当的系统调用指令陷入内核，然后将系统调用的返回状态传递回调用程序。
在本书中，我们将系统调用和与它们相关联的包装函数都称为系统级函数，这两个术语可以互换地使用。

在 X86-64 系统上，系统调用是通过一条称为 `syscall` 的陷阱(`Trap`)指令来提供的。研究程序能够如何使用这条指令来直接调用 Linux 系统调用是很有趣的。
所有到 Linux 系统调用的参数都是通过通用寄存器而不是栈传递的。
按照惯例，寄存器 `％rax` 包含系统调用号，寄存器 `%rdi`、`%rsi`、`%rdx`、`%r10`、`%r8`  和 `％r9` 包含最多 6 个参数。第一个参数在  `％rdi` 中，第二个在 `％rsi` 中，以此类推。
从系统调用返回时，寄存器 `%rcx` 和 `％r11` 都会被破坏，`％rax` 包含返回值。-4095 到 -1 之间的负数返回值表明发生了错误，对应于负的 `errno`。

在x86-64架构的Linux系统中，系统调用是通过`syscall`指令实现的，这是一种特殊的陷阱指令，用于从用户空间切换到内核空间来执行系统调用。
下面详细解释了在进行系统调用时CPU寄存器的变化情况：

### 系统调用前的寄存器设置

1. **系统调用号**：在执行`syscall`指令之前，系统调用的编号（即想要执行的具体系统调用）被放入`%rax`寄存器。

2. **参数传递**：系统调用的参数通过寄存器而不是通过栈传递。最多可以传递六个参数，它们的传递方式如下：
   - 第一个参数放在`%rdi`寄存器。
   - 第二个参数放在`%rsi`寄存器。
   - 第三个参数放在`%rdx`寄存器。
   - 第四个参数放在`%r10`寄存器。
   - 第五个参数放在`%r8`寄存器。
   - 第六个参数放在`%r9`寄存器。

### 执行`syscall`指令

- 当执行`syscall`指令时，发生从用户模式到内核模式的切换。
CPU会保存程序计数器（*PC*）的当前值到`%rcx`寄存器，将*FLAGS*寄存器的值保存到`%r11`寄存器，然后开始执行内核中为该系统调用提供服务的代码。

### 系统调用执行后的寄存器变化

1. **返回值**：系统调用完成后，返回值存放在`%rax`寄存器。如果调用成功，返回值通常是非负的。
如果调用失败，`%rax`将包含一个负数，其范围是-4095到-1之间，表示错误。这个负数对应于负的`errno`值。

2. **寄存器破坏**：`%rcx`和`%r11`寄存器的内容在系统调用期间被破坏。这意味着它们在执行`syscall`指令后不再保持原来的值。

在执行`syscall`指令过程中，`%rcx`和`%r11`寄存器被破坏的原因与如何实现从内核模式返回到用户模式有关。以下是具体的解释：
----------------------------------------------------------------------------------------------------
### 为什么要保存`%rcx`和`%r11`
 1. **程序计数器（PC）保存在`%rcx`**：在`syscall`指令执行时，当前的指令指针（即程序计数器PC，指向下一条要执行的用户空间指令的地址）被保存在`%rcx`寄存器中。这是因为`syscall`指令会导致程序计数器跳转到操作系统内核中的一个预定义位置开始执行系统调用。保存原始的程序计数器是为了在系统调用完成后，能够知道从哪里恢复用户空间的程序执行。

 2. **FLAGS寄存器保存在`%r11`**：在进入内核模式时，当前的FLAGS寄存器（包含了控制程序执行流的各种标志位）被保存在`%r11`寄存器中。这样做是为了保持用户态执行环境的状态，以便在返回用户空间时能够恢复这些状态。

### 为什么`%rcx`和`%r11`会被破坏

- **内核模式使用**：在内核模式下，`%rcx`和`%r11`可能被用于其他目的，因为内核代码不需要保持这些寄存器的用户态值。当系统调用执行时，内核可能会使用这些寄存器来存储临时数据或进行其他操作，从而导致它们的内容被改变。

### 返回用户空间

- **通过`sysret`指令返回**：在系统调用完成后，内核通过执行`sysret`指令来返回用户空间。
这时，`%rcx`和`%r11`的原始值不再重要，因为`sysret`指令会使用保存在其他地方的信息来恢复程序计数器和FLAGS状态，从而允许用户态程序从系统调用后的正确位置继续执行。

综上所述，`%rcx`和`%r11`在`syscall`执行时被用于保存重要的程序状态信息，但在内核执行系统调用期间可能被用于其他目的。内核不需要维护这些寄存器在用户态时的状态，因为它有自己的机制来保证在返回用户态时能够正确地恢复程序的执行。
----------------------------------------------------------------------------------------------------

### 错误处理

- 如果系统调用失败（`%rax`中的值为-4095到-1之间的负数），程序可以检查这个值来确定具体发生了什么错误，并据此采取相应的措施。

总的来说，在x86-64架构的Linux系统上，`syscall`指令用于从用户态跳转到内核态执行系统调用。
系统调用的编号和参数通过特定的寄存器传递，而系统调用的返回值也通过寄存器返回。在这个过程中，某些寄存器的值会被改变或破坏，所以在编写涉及系统调用的程序时，需要注意这一点。


例如，考虑大家熟悉的 hello 程序的下面这个版本，用系统级函数 `write`（见 10.4 节）来写，而不是用 printf：
~~~c
int main()
{
    write(1, "hello, world\n", 13);
    _exit(0);
}
~~~

write 函数的第一个参数将输出发送到 stdout。第二个参数是要写的字节序列，而第三个参数是要写的字节数。

图 8-11 给出的是 hello 程序的汇编语言版本，直接使用 syscall 指令来调用 write 和 exit 系统调用。第 9 ∼ 13 行调用 write 函数。首先，第 9 行将系统调用 write 的编号存放在 ％rax 中，第 10 ∼ 12 行设置参数列表。然后第 13 行使用 syscall 指令来调用系统调用。类似地，第 14 ∼ 16 行调用 _exit 系统调用。

~~~s
.section .data
string:
  .ascii "hello, world\n"
string_end:
  .equ len, string_end - string
.section .text
.globl main
main:
  # First, call write(1, "hello, world\n", 13)
  movq $1, %rax                 # write is system call 1
  movq $1, %rdi                 # Arg1: stdout has descriptor 1
  movq $string, %rsi            # Arg2: hello world string
  movq $len, %rdx               # Arg3: string length
  syscall                       # Make the system call

  # Next, call _exit(0)
  movq $60, %rax                # _exit is system call 60
  movq $0, %rdi                 # Arg1: exit status is 0
  syscall                       # Make the system call
~~~

直接用 Linux 系统调用来实现 hello 程序

旁注 - 关于术语的注释

各种异常类型的术语根据系统的不同而有所不同。处理器 ISA 规范通常会区分异步“中断”和同步“异常”，但是并没有提供描述这些非常相似的概念的概括性的术语。
为了避免不断地提到“异常和中断”以及“异常或者中断”，我们用单词“异常”作为通用的术语，而且只有在必要时才区别异步异常（中断）和同步异常（陷阱、故障和终止）。
正如我们提到过的，对于每个系统而言，基本的概念都是相同的，但是你应该意识到一些制造厂商的手册会用“异常" 仅仅表示同步事件引起的控制流的改变。


# OBJDUMP USUAGE
~~~shell
czy-307-thinkcentre-m720q-n000 :: new_space/Exceptional_Control_Flow/01 ‹master*› % objdump --help                                                         1 ↵
用法：objdump <选项> <文件>
 显示来自目标 <文件> 的信息。
 至少必须给出以下选项之一：
  -a, --archive-headers    Display archive header information
  -f, --file-headers       Display the contents of the overall file header
  -p, --private-headers    Display object format specific file header contents
  -P, --private=OPT,OPT... Display object format specific contents
  -h, --[section-]headers  Display the contents of the section headers
  -x, --all-headers        Display the contents of all headers
  -d, --disassemble        Display assembler contents of executable sections
  -D, --disassemble-all    Display assembler contents of all sections
      --disassemble=<sym>  Display assembler contents from <sym>
  -S, --source             Intermix source code with disassembly
      --source-comment[=<txt>] Prefix lines of source code with <txt>
  -s, --full-contents      Display the full contents of all sections requested
  -g, --debugging          Display debug information in object file
  -e, --debugging-tags     Display debug information using ctags style
  -G, --stabs              Display (in raw form) any STABS info in the file
  -W[lLiaprmfFsoRtUuTgAckK] or
  --dwarf[=rawline,=decodedline,=info,=abbrev,=pubnames,=aranges,=macro,=frames,
          =frames-interp,=str,=loc,=Ranges,=pubtypes,
          =gdb_index,=trace_info,=trace_abbrev,=trace_aranges,
          =addr,=cu_index,=links,=follow-links]
                           Display DWARF info in the file
  --ctf=SECTION            Display CTF info from SECTION
  -t, --syms               Display the contents of the symbol table(s)
  -T, --dynamic-syms       Display the contents of the dynamic symbol table
  -r, --reloc              Display the relocation entries in the file
  -R, --dynamic-reloc      Display the dynamic relocation entries in the file
  @<file>                  Read options from <file>
  -v, --version            Display this program's version number
  -i, --info               List object formats and architectures supported
  -H, --help               Display this information

 以下选项是可选的：
  -b, --target=BFDNAME           Specify the target object format as BFDNAME
  -m, --architecture=MACHINE     Specify the target architecture as MACHINE
  -j, --section=NAME             Only display information for section NAME
  -M, --disassembler-options=OPT Pass text OPT on to the disassembler
  -EB --endian=big               Assume big endian format when disassembling
  -EL --endian=little            Assume little endian format when disassembling
      --file-start-context       Include context from start of file (with -S)
  -I, --include=DIR              Add DIR to search list for source files
  -l, --line-numbers             Include line numbers and filenames in output
  -F, --file-offsets             Include file offsets when displaying information
  -C, --demangle[=STYLE]         Decode mangled/processed symbol names
                                  The STYLE, if specified, can be `auto', `gnu',
                                  `lucid', `arm', `hp', `edg', `gnu-v3', `java'
                                  or `gnat'
      --recurse-limit            Enable a limit on recursion whilst demangling.  [Default]
      --no-recurse-limit         Disable a limit on recursion whilst demangling
  -w, --wide                     Format output for more than 80 columns
  -z, --disassemble-zeroes       Do not skip blocks of zeroes when disassembling
      --start-address=ADDR       Only process data whose address is >= ADDR
      --stop-address=ADDR        Only process data whose address is < ADDR
      --prefix-addresses         Print complete address alongside disassembly
      --[no-]show-raw-insn       Display hex alongside symbolic disassembly
      --insn-width=WIDTH         Display WIDTH bytes on a single line for -d
      --adjust-vma=OFFSET        Add OFFSET to all displayed section addresses
      --special-syms             Include special symbols in symbol dumps
      --inlines                  Print all inlines for source line (with -l)
      --prefix=PREFIX            Add PREFIX to absolute paths for -S
      --prefix-strip=LEVEL       Strip initial directory names for -S
      --dwarf-depth=N        Do not display DIEs at depth N or greater
      --dwarf-start=N        Display DIEs starting with N, at the same depth
                             or deeper
      --dwarf-check          Make additional dwarf internal consistency checks.      
      --ctf-parent=SECTION       Use SECTION as the CTF parent
      --visualize-jumps          Visualize jumps by drawing ASCII art lines
      --visualize-jumps=color    Use colors in the ASCII art
      --visualize-jumps=extended-color   Use extended 8-bit color codes
      --visualize-jumps=off      Disable jump visualization

objdump：支持的目标： elf64-x86-64 elf32-i386 elf32-iamcu elf32-x86-64 pei-i386 pei-x86-64 elf64-l1om elf64-k1om elf64-little elf64-big elf32-little elf32-big pe-x86-64 pe-bigobj-x86-64 pe-i386 srec symbolsrec verilog tekhex binary ihex plugin
objdump：支持的体系结构： i386 i386:x86-64 i386:x64-32 i8086 i386:intel i386:x86-64:intel i386:x64-32:intel i386:nacl i386:x86-64:nacl i386:x64-32:nacl iamcu iamcu:intel l1om l1om:intel k1om k1om:intel

The following i386/x86-64 specific disassembler options are supported for use
with the -M switch (multiple options should be separated by commas):
  x86-64      Disassemble in 64bit mode
  i386        Disassemble in 32bit mode
  i8086       Disassemble in 16bit mode
  att         Display instruction in AT&T syntax
  intel       Display instruction in Intel syntax
  att-mnemonic
              Display instruction in AT&T mnemonic
  intel-mnemonic
              Display instruction in Intel mnemonic
  addr64      Assume 64bit address size
  addr32      Assume 32bit address size
  addr16      Assume 16bit address size
  data32      Assume 32bit data size
  data16      Assume 16bit data size
  suffix      Always display instruction suffix in AT&T syntax
  amd64       Display instruction in AMD64 ISA
  intel64     Display instruction in Intel64 ISA
将 bug 报告到 <http://www.sourceware.org/bugzilla/>。
~~~