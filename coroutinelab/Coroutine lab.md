# part0:



## 任务一：

函数调用过程：

1. **调用函数：** 从 `main` 函数调用 `foo` 函数。
2. **保存返回地址：** 当调用一个函数时，当前函数的返回地址（调用该函数的指令的地址）会被压入栈中，以便在函数执行完毕后返回到正确的位置。这是为了保证程序执行的流畅性。
3. **保存参数：** 如果有参数传递给 `foo` 函数，这些参数也会被压入栈中。
4. **创建局部变量：** `foo` 函数中的局部变量也会在栈上分配空间。
5. **执行函数体：** `foo` 函数的代码开始执行，可以访问参数和局部变量。
6. **返回：** 当 `foo` 函数执行完成后，会将返回值存放在事先约定的位置，然后弹出栈顶的返回地址，程序跳转到该地址，继续执行。

从 `main` 调用 `foo` 到 `foo` 返回的栈帧变化过程：

```plaintext
main 函数调用 foo 函数：
--------------------------------------------------------------------------------------
| foo 函数局部变量 | foo 函数参数 | 返回地址 | main 函数局部变量 | 
--------------------------------------------------------------------------------------


foo 函数执行中：
--------------------------------------------------------------------------------------
| foo 函数局部变量 | foo 函数参数 | 返回地址 | main 函数局部变量 |
--------------------------------------------------------------------------------------


foo 函数返回：
--------------------------------------------------------------------------------------
返回地址 | main 函数局部变量 |
--------------------------------------------------------------------------------------

main函数继续执行：
--------------------------------------------------------------------------------------
 | main 函数局部变量 |
--------------------------------------------------------------------------------------

```

RBP 指向当前栈帧的基址，RSP 指向当前栈帧的栈顶

函数参数是如何传递的：前6个整数和指针类型的参数（按顺序）会被依次放入寄存器 `rdi`, `rsi`, `rdx`, `rcx`, `r8`, 和 `r9` 中。如果参数超过6个，额外的参数会被压入调用栈，从左到右依次放在相对于栈顶的位置。

```csharp
csharpCopy code0x55555540086f <add(int, int)+10>    mov    edx, dword ptr [rbp - 0x14]  ; 将第一个参数放入寄存器 edx
0x555555400872 <add(int, int)+13>    mov    eax, dword ptr [rbp - 0x18]  ; 将第二个参数放入寄存器 eax
```

函数的返回值是如何传递的：在调用 `add` 函数的地方，`foo` 函数中的 `ret` 值是通过 `EAX` 寄存器传递给 `foo` 函数调用处。接下来，`foo` 函数调用的返回值也会通过 `EAX` 寄存器返回给 `main` 函数调用处。

```csharp
0x55555540087a <add(int, int)+21>    mov    eax, dword ptr [rbp - 4]  ; 将返回值放入寄存器 eax
0x55555540087d <add(int, int)+24>    pop    rbp
0x55555540087e <add(int, int)+25>    ret
```



<img src="https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231206190902541.png" alt="image-20231206190902541" style="zoom: 50%;" />



## 任务2:

#### 1.  **一个“普通”的函数支持哪两个操作，分别承担了什么功能？**

“正常”函数可以被看作具有两种操作：调用和返回

调用操作创建一个==激活帧==（*activation frame*），暂停调用函数的执行，并将执行权转移到被调用函数的开始。

返回操作将返回值传递给调用者，销毁激活帧，然后在调用函数被调用的点之后恢复调用者的执行。

#### 2. **为什么我们说调用栈不能满足协程的要求？**

*Since coroutines can be suspended without destroying the activation frame, we can no longer guarantee that activation frame lifetimes will be strictly nested. This means that activation frames cannot in general be allocated using a stack data-structure and so may need to be stored on the heap instead.*协程的主要特征之一是其能够在执行过程中暂停并随后恢复。调用栈通常用于存储函数调用之间的状态，包括局部变量、返回地址等，并随着函数的调用和返回进行压栈和出栈操作。然而，对于协程来说，暂停和恢复的操作不仅仅涉及函数调用和返回，还包括部分状态的保持，以便在恢复时恢复执行。这导致了与传统调用栈模型不同的需求。另外协程可能在执行过程中多次暂停和恢复，需要保存不同暂停点的状态，这些状态不适合在传统调用栈上维护。传统的调用栈是按照严格的嵌套顺序来进行压栈和出栈的，而协程的状态在暂停时需要保持，因此无法完全使用调用栈的嵌套结构来满足其需求。而且，协程有时需要在其执行期间使用堆内存来保持状态，因为它们的生命周期不再严格嵌套，不能简单地使用调用栈进行内存分配和释放。这也是调用栈不足以满足协程需求的原因之一。

#### 3.协程作为一种泛化的函数，支持了哪几个操作，分别承担了什么功能？

协程支持以下三个额外的操作：*'Suspend'、'Resume'* 和 '*Destroy*'。这些操作分别具有以下功能：

1. ***Suspend***：暂停协程的执行，并将执行权转移回调用者或恢复者，同时保留激活帧，即函数执行的当前状态。在暂停点，协程中的对象继续保持活跃状态。

2. ***Resume***：恢复被暂停的协程的执行，重新激活协程的激活帧，继续执行被暂停的位置。

3. ***Destroy***：销毁协程的激活帧，但不会恢复协程的执行。这个操作仅在协程暂停时执行，用于释放激活帧占用的内存空间和清理被暂停点所引用的对象。

#### 4.如果不能使用栈来实现协程，那么我们可以将函数运行时所需的信息存储在哪里？

1. **内存**：部分协程状态和信息可以存储在堆内存中。协程可能在不同的暂停和恢复点之间切换，而传统的栈结构难以支持这种灵活性。使用堆内存可以在协程暂停时保留状态，并在稍后恢复。

2. **协程框架**（*Coroutine Frames*）：协程框架是专门针对协程的概念，用于存储执行过程中所需的信息和状态。这包括局部变量、暂停点信息、恢复点等。由于协程状态可能不再严格嵌套于函数调用，这种框架可能需要存储在堆上，以便在多次暂停和恢复时保持状态。

3. **特殊寄存器/变量**：有些编程语言或平台可能提供特殊的寄存器或变量来存储协程的状态信息。这些寄存器或变量可能记录暂停点、恢复点等信息，使得在恢复时能够准确返回到之前暂停的位置。



## 任务三：



### CPU的状态包括哪些？一个显然的问题是，我们不能一下保存所有的状态，结合函数调用约定，以及任务二中第四个问题的回答，你认为我们需要保存哪些状态用于暂停并恢复一个函数的运行？

CPU的状态包括众多寄存器中的内容，这些寄存器通常涵盖了程序执行所需的各种信息。

在实际操作中，并非需要保存和恢复所有的状态，因为一些状态可以在程序运行时被重建；我认为比较重要的几个内容：

1. **程序计数器**：指示了程序执行的位置，即当前指令的地址。



2. **栈指针**：指向当前栈帧的栈顶地址。



3. **栈框架**：包括局部变量、返回地址和其他调用相关信息。



4. **寄存器内容**：一些特定寄存器可能包含重要的临时数据或计算中间状态。

考虑到函数调用约定，暂停和恢复函数的运行需要保存这些状态。当函数被暂停时，这些状态需要被存储以便稍后恢复函数执行。典型的做法是保存这些状态到一个指定的存储区域（例如堆内存），然后在恢复时重新加载这些状态。 这些状态的保存和恢复使得函数在恢复执行时可以准确地返回到暂停的地方，并继续执行，同时保持原本函数调用的上下文不受影响。



# part 1：

![image-20231206194322025](./../AppData/Roaming/Typora/typora-user-images/image-20231206194322025.png)

## 函数实现：

#### creat函数：

`create`函数用于创建一个协程，并返回该协程的指针。

`coroutine* create(func_t func, void* args)`：

- 设置协程对象的上下文链接字段为 `nullptr`，表示在当前上下文执行完毕后，不切换到其他上下文，即结束执行。

- 使用 `makecontext` 函数设置协程对象的上下文，指定了入口函数为 `func_wrap`，参数为协程对象的指针。

- 返回创建的协程对象的指针。

`coroutine(func_t func, void* args)`：

- 初始化协程对象的上下文。

- 设置上下文的栈起始地址为协程对象的 `stack` 数组。

- 设置上下文的栈大小为协程对象的 `stack` 数组大小。

- 设置上下文的链接字段为 `nullptr`，表示在当前上下文执行完毕后，不切换到其他上下文，即结束执行。

创建一个协程对象，初始化其上下文，为其分配栈空间，设置入口函数和参数，最后返回协程对象的指针。



![image-20231205131511300](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231205131511300.png)





#### resume函数：

在第一次 resume 执行协程时进行检测，如果没有进行初始化需要进行初始化以及封装

```cpp

int resume(coroutine* co, int param) { 

    co->data = param; // 设置传入参数

    if(co->started == false){    

        co->started = true;

        makecontext(&(co->ctx), (void(*)())func_wrap, 1, co);

    }
    g_coro_env.push(co); // 将协程推入栈中

    swapcontext(&(g_coro_env.get_coro(0)->ctx), &(co->ctx));

    return g_coro_env.get_coro(0)->data; // 返回yield时的数据

}

```





![image-20231205131523498](https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312052027664.png)![image-20231205131533812](https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312052027622.png)



#### yield函数：

将正在运行的协程pop出来，协程恢复为resume传的数据

```cpp

int yield(int ret ) {

    coroutine* current = g_coro_env.get_coro(1); // 获取当前协程

    int par = current->data;

    g_coro_env.get_coro(0)->data = ret; // 保存返回值

    g_coro_env.pop(); // 弹出栈顶协程    

    swapcontext(&(current->ctx), &(g_coro_env.get_coro(0)->ctx)); // 切换到调用者

    return par; // 返回resume时传入的数据

}

```

# Part2:

![image-20231206180938798](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231206180938798.png)









## `coro_ctx`结构体



![image-20231205090319804](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231205090319804.png)



上面是一个正常的函数调用栈帧，但是我们需要将返回地址压栈之后，把<span style="color:red;">被调用</span>手动转换到另一个内存<span style="color:blue;">被调用</span>，并且保存手动保存<span style="color:red;">ctx</span>，因此**context里模拟一个内存**，需要一个stack储参数以及一个十四位寄存器数组。其中rsp指向了协程函数的首地址，rdi指向了函数的参数。其中stack_rsp为栈底指针，先将return_address压栈，然后创建栈帧。



![image-20231205090335839](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231205090335839.png)

```cpp

    struct context{

        void *regs[14];

        size_t stack_size;

        char *stack_rsp;

    };mespace coro



```

### `ctx_make`函数

`ctx_make`函数中为协程创建上下文并初始化它。![image-20231206175616585](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231206175616585.png)



```cpp
void ctx_make(context *ctx, func_t coro_func, const void *arg)
    {

        char *rsp = ctx->stack_rsp + ctx->stack_size - 8;//栈顶指针下移，防止返回地址覆盖

        memset(ctx->regs, 0, sizeof(ctx->regs));//初始化


        void **return_address = (void **)(rsp);//将72的指针赋给104

        *return_address = (void *)coro_func;//将封装函数封装到72



        ctx->regs[kRSP] = (char *)rsp;//104存到寄存器

        ctx->regs[kRETURN] = (char *)coro_func;//72存到寄存器


        ctx->regs[kRDI] = (char *)arg;



    }

```

### `coro_ctx_swap`

coro_ctx_swap函数首先保存上下文(六个save寄存器、六个传参寄存器、rsp、rdi)rsp->rdi

然后恢复callee的上下文rsi->%rsp(……),并且把返回地址赋值给栈顶rsi，然后ret栈顶（rsi）交出控制权~~有好多寄存器一直是0（比如64什么的）也一并传过去算了~~。



```cpp

    leaq (%rsp),%rax

    movq %rax, 104(%rdi)
//从104到0
    movq %r15, (%rdi)
	xorq %rax, %rax


    movq (%rsi), %r15
//0到104

    movq 104(%rsi), %rsp    


	movq 72(%rsi),%rax//注意不可以两边取（）

    movq %rax,%(rsp)//把返回地址赋值给rsp

	ret//交出控制权



```



==*coroutine与从coroutine_env与v1基本一致*==







# part 2.5





### Share_stack



![image-20231205203925724](https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312052039784.png)



![image-20231205204010434](https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312052040493.png)

在内存中圈出一块内存，作为共享栈；采用循环队列的方式，==**每一个stack_member对应位置自从初始化之后就对应该位置**==；一直到如果共享栈被全部占用，就重新从零号位置开始。当一个栈$\alpha$被分配到共享栈对应位置0，但是对应位置0已经有了栈a；那么就把栈a的信息存放在私有栈里，然后将栈$\alpha$存放进共享栈。



```cpp
struct share_stack {
  stack_mem** stack_array;//指向栈内存块的指针数组

  int count = 0;//共享栈大小

  size_t stack_size = 0;//每个栈的大小

  int position = 0;//如果共享栈全部被占用，需要进行保存、覆盖操作的计数器



  share_stack(int count, size_t stack_size) : count(count), stack_size(stack_size) {

    stack_array = new stack_mem*[count];

    for (int i = 0; i < count; ++i) {

      stack_array[i] = new stack_mem(stack_size); 

    }

  }
```



### Save_stack:

```cpp
void save_stack(coroutine* co) {//旧的栈被清走的时候，保存旧的结构体的信息

    int len = co->stack->stack_start +co->stack->stack_size - co->stack_sp;
    if(co-> stack_mem_sp ){//如果之前进行过save_stack，需要先把之前的free掉
        free(co-> stack_mem_sp);
        co->stack_mem_sp = nullptr;
    }
    co->stack_mem_sp = (char *)malloc(len);//开辟一块新的内存
    co->stack_mem_size = len;
    memcpy(co->stack_mem_sp,co->stack_sp, len);
}
```



### Swap：



![image-20231206103344060](https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312061033509.png)



原始版本⬇️

实现之前Share_stack所说的是否对`callee`对应栈上的`stack_mem`的`last_user`进行`save_stack`功能，以及进行`coro_ctx_swap`

如果按照字面意思写，就对每一步进行判断，如果对应共享站位置已经被占用并且使用者`last_user`不是caller，就将这个栈上内容保存在`last_user`私有栈。

> 如果共享栈被全部占用，就重新从零号位置开始。当一个栈$\alpha$被分配到共享栈对应位置0，但是对应位置0已经有了栈a；那么就把栈a的信息存放在私有栈里，然后将栈$\alpha$存放进共享栈。



```cpp

    void swap(coroutine *caller, coroutine *callee)

    {

        char c;
        caller->stack_sp = &c;//通过创建在内存上新建变量，让其正好在未使用的内存部位
        static coroutine *last_member = nullptr;
        static coroutine *next_member = nullptr;
        if (!pending->is_shared)//没有使用过共享栈
        {

            next_member = nullptr;

            last_member = nullptr;//不需要进行save_stack以及memcpy操作
        }
        else//如果使用共享栈
        {
            next_member = callee;
            coroutine *occupy = callee->stack->last_user;//callee所对应共享栈的上一人使用者
            callee->stack->last_user = callee;//把callee放到共享栈上
            last_member = occupy;
            if (occupy && occupy != callee){
                save_stack(occupy)；}//如果上一任使用者不为callee，将其save_stack

        }

        //callee是否进行过save_stack

        if (last_member && next_member && last_member != next_member) {

            if (next_member->stack_mem_sp && next_member->stack_mem_size > 0){

                memcpy(next_member->stack_sp, next_member->stack_mem_sp, next_member->stack_mem_size);}//如果进行过save_stack，将其私有栈中信息进行复制 

        }

        coro_ctx_swap(&(caller->ctx), &(pending->ctx));

    }

```



但是我注意到，如果B因为不再共享栈上，如果经历过`save_stack`需要`memcpy`，如果没经历过`save_stack`说明只进行过初始化，也可以进行`memcpy`，所以可以直接把`memcpy`放进`save_stack`的条件判断里。



简化版⬇️



```cpp

    void swap(coroutine *caller, coroutine *callee)

    {
        if (callee->is_shared){
            coroutine *occupy = callee->stack->last_user;
            callee->stack->last_user = callee;//与之前一致
            if (occupy && occupy != callee){//如果需要对上一任使用者save_stack，就把callee私有栈上信息赋值到共享栈
                save_stack(occupy);
                memcpy( callee->stack_sp,  callee->stack_mem_sp,  callee->stack_mem_size);
            }
        }
        coro_ctx_swap(&(caller->ctx), &(callee->ctx));
    }

```



# part 3:

![image-20231206194619133](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231206194619133.png)

先用c++看一遍这几个宏在干什么，通过使用`switch`语句和`goto`语句来实现状态的切换。

接下来，逐步把逻辑过程用宏来替换。

对应思路写出的c++程序

```cpp

class Fib : public CoroutineBase {
private:
    int a = 0, b = 1;
public:
    int operator()() {

        int result = -1;
        //执行CO_BEGIN
        switch (started) {
            case 0:
            //Coroutine logic
            while (true) {
                //执行CO_YIELD(a);
                started = __LINE__;
                return a;
                case __LINE__:; }
            //执行CO_END
        }

        //执行CO_RETURN(...)

        started = -1;

        return result;

    }

};



```

1. **CO_BEGIN 和 CO_END：**

    - `CO_BEGIN`和`CO_END`是函数的起始和结束位置，`CO_BEGIN`中的`switch (started)用于根据`started`的值执行不同的代码块，从而实现协程的状态切换。

    - `CO_END`结束`switch`语句的定义,即加上}。
2. **CO_YIELD：**

    - 当执行到`CO_YIELD`时，将`started`设置为当前行号（`__LINE__`），然后返回相应的值，当下一次的时候通过这个值来进行返回以及定位。
3. **CO_RETURN：**

    - `CO_RETURN`结束协程函数的执行。`started`设置为-1，表示协程函数已经执行完毕。



```cpp

#define CO_BEGIN switch (started) { case 0:

#define CO_END   }

#define CO_YIELD(...) do { started = __LINE__; return __VA_ARGS__; case __LINE__:; } while (0)

#define CO_RETURN(...) do { started = -1; return __VA_ARGS__; } while (0)

```

# Part4:

![image-20231206194756316](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231206194756316.png)

## 任务四：

#### 1.协程函数的返回值`Coroutine Functor`需要有哪些成员？

- 构造函数和析构函数。

- `get_return_object()`: 返回与协程相关联的对象。

- `initial_suspend()`: 定义协程开始时的行为。

- `final_suspend()`: 定义协程结束时的行为。

- `unhandled_exception()`: 处理协程内未捕获的异常。

- `return_value()` 或 `return_void()`: 用于返回一个值或指示完成而没有值



#### 2.`Promise`对象需要提供哪些函数？



- 构造函数，包括默认构造函数。

- `get_return_object()`: 返回协程句柄或表示协程最终输出的其他对象。

- `initial_suspend()`: 在协程开始时提供一个挂起点。

- `final_suspend()`: 协程结束时的挂起点。

- `unhandled_exception()`: 用于处理协程内的异常。

- `return_void()` 或 `return_value()`: 用于返回一个值或指示完成而没有值



#### 3.`Awaitable object`需要提供哪些接口？



- `await_ready()`: 检查可等待对象是否就绪。

- `await_suspend()`: 如果不就绪，则挂起协程。

- `await_resume()`: 一旦就绪，恢复协程



#### 4.`Coroutine handle`通常需要提供哪些函数？



- `resume()`: 恢复协程的执行。

- `destroy()`: 销毁协程状态。

- `from_address()`: 从给定地址创建协程句柄。

- `address()`: 返回协程状态的地址。

- `done()`: 检查协程是否已经执行完成



#### 5.为什么说`co_yield`和`co_return`是`co_await`的语法糖？



它们被编译器转换为`co_await`表达式，通过提供便捷的方式在协程中返回或产生值，同时在幕后利用`co_await`机制进行suspending and resuming execution



#### 6.简述协程函数的调用过程并阐述上述每个接口函数的功能。



**协程函数调用过程**:



- 当协程函数被调用时，编译器首先根据协程的返回类型确定`promise_type`。

- 创建协程状态（Coroutine State），其中包含`promise`对象、函数参数副本、挂起点信息以及跨挂起点的局部变量/临时变量的存储空间。

- 调用`get_return_object()`方法，从`promise`对象获取协程的返回值。

- 进入初始挂起点（Initial Suspend Point），通常使用`co_await promise.initial_suspend()`。

- 执行协程体。在协程体中，可以有多个挂起点，通过`co_await`、`co_yield`或`co_return`表达式实现。

- 最终挂起点（Final Suspend Point）被执行，通常是`co_await promise.final_suspend()`。

- 协程结束，清理协程状态。



**接口函数的功能**:



- `promise_type`中的方法：

    - `get_return_object()`: 创建与协程相关联的对象，通常是协程的句柄。

    - `initial_suspend()`: 在协程开始时提供一个挂起点。

    - `final_suspend()`: 在协程结束时提供一个挂起点。

    - `unhandled_exception()`: 处理协程内部未捕获的异常。

    - `return_void()` 或 `return_value()`: 返回值或表示协程完成但无返回。

- `Awaitable`对象的接口：

    - `await_ready()`: 检查对象是否已准备好。

    - `await_suspend()`: 如果对象未准备好，则挂起协程。

    - `await_resume()`: 一旦准备好，恢复协程执行。

- `coroutine_handle`的功能：

    - `resume()`: 恢复协程的执行。

    - `destroy()`: 销毁协程状态。

    - `from_address()`: 从给定地址创建协程句柄。

    - `address()`: 返回协程状态的地址。

    - `done()`: 检查协程是否已完成执行。



以上描述了协程函数的调用过程和相关接口函数的功能，这些功能共同管理了协程的生命周期、状态和控制流。



## generator.h:



1. **Promise Type**: 

    - 实现`promise_type`类，它是协程的核心，用于管理协程的状态和生命周期。

    - 包含方法如`get_return_object()`，`initial_suspend()`，`final_suspend()`，`return_void()`或`return_value()`以及`yield_value()`（用于`co_yield`）。

2. **Generator 构造函数和析构函数**:

    - 构造函数应该初始化协程。

    - 析构函数应该确保协程被正确销毁。

3. **Iterator 类**:

    - 实现迭代器构造函数，可能需要使用promise的句柄。

    - 实现运算符`==`和`!=`，以支持迭代器比较。

    - 实现前缀和后缀的`++`运算符，用于移动迭代器。

    - 实现`*`和`->`运算符，用于访问迭代器指向的元素。

4. **Begin() 和 End() 成员函数**:

    - `begin()`应该返回一个指向协程第一个元素的迭代器。

    - `end()`应该返回一个表示协程结束的迭代器。

5. **成员变量**:

    - 根据需要添加任何必要的成员变量，如协程句柄。

一眼看上去不知道从哪里开始入手，那就从上往下一步一步实现，然后再统一接口。



#### generator

##### **构造函数**

通过协程句柄来进行generator的构造，将句柄的所有权从一个生成器对象 `other` 转移到新创建的生成器对象；句柄放在`private`里。

```cpp
generator(generator&& other) : coro_(other.coro_) {
    other.coro_ = nullptr;
}
```



####  **`promise_type`**

从构造到暂停到恢复（resume通过handle执行），需要promise有相对应的关键字函数



###### `get_return_object`

`get_return_object` 当协程函数被调用时，编译器会根据协程的返回类型确定 `promise_type`。然后，编译器会调用 `promise_type` 中的 `get_return_object` 函数来创建与协程相关联的 `generator` 对象。

所以我需要一个接受协程句柄作为参数，用于创建生成器对象的generator的构造函数，我把它写在了generator的`private`里面

```cpp
explicit generator(std::coroutine_handle<promise_type> coro) : coro_(coro) {}
```

```cpp
generator get_return_object() {
    return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
}
```



###### `yield_value`

`co_yield` 语句用于在协程中产生一个值。这个值会被传递给与协程相关联的 `promise_type` 结构体中的 `yield_value` 函数，所以我需要先把`generator`储存这个值；然后返回 `std::suspend_always{}`，告诉编译器在这里挂起协程。

```cpp
std::optional<Value> current_value;
std::suspend_always yield_value(Value value) {
    current_value = std::move(value);
    return {};
}

```

###### 其他

`initial_suspend()` 控制协程开始时的挂起，`final_suspend()` 控制协程执行完成时的挂起。`return_void()` 函数用于处理无返回值的协程（当函数调用的时候）。当协程使用 `co_return;` 语句结束时，会调用这个函数。`unhandled_exception()` 函数用于处理协程内部未捕获的异常。当协程内发生了未捕获的异常时，这个函数会被调用。

#### `iterator`

> `iterator` 类的基本原理是为 `generator` 类提供一个迭代器，使得通过迭代器能够遍历生成器产生的序列。`iterator` 类通过存储协程句柄、定义迭代器特性别名以及实现迭代器的基本行为，使得通过迭代器可以有效地访问生成器产生的序列。它是生成器与迭代器之间的桥梁，提供了在范围表达式和标准库算法中使用生成器的便捷方式。

`iterator` 类中的两个 `operator++` 函数，分别用于前置和后置递增；在序列上移动到下一个元素。即从一个co_yield暂停点一直运行到下一个暂停点；前置递增恢复协程，并且返回引用；后置递增需要先保存当前迭代器，然后恢复协程，返回之前保存的协程，完成多次迭代。

```cpp
iterator& operator++() {

    coro_.resume();//恢复与迭代器关联的协程的执行
    if (coro_.done()) coro_ = nullptr;//表示迭代器已经到达末尾
    return *this;
}

iterator operator++(int) {

    iterator temp = *this;//代表了当前迭代器

    ++(*this);//当前迭代器对象执行一次递增操作

    return temp;//返回的是递增之前的迭代器状态的副本

}

```

==     ！=用于比较迭代对象 coro_,判断是否正确执行

```cpp
bool operator==(const iterator& other) const {

    return coro_ == other.coro_;

}

bool operator!=(const iterator& other) const {
    
    return !(*this == other);
    
}

```

###### 起始和终止迭代器：

**`iterator begin()`**：

- `begin` 函数用于获取生成器的起始迭代器。

- 如果当前协程句柄 `coro_` 不为 `nullptr`，表示生成器尚未结束，调用 `coro_.resume()` 恢复协程的执行。这确保在开始迭代时生成器的协程已经开始执行。

- 返回一个迭代器对象。如果协程已经执行完成，返回的是结束迭代器；否则返回当前协程句柄关联的迭代器对象。

```cpp
iterator begin() {
    if (coro_) coro_.resume();
    return coro_.done() ? end() : iterator{coro_};
}
```



# Part 5:

![image-20231206194838335](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231206194838335.png)

## generator.h:

在很大程度上借鉴了参考文档，对于调用树的维护

### 递归实现思路：



1. 当前生成器通过 `await_suspend` 将协程控制权传递给嵌套生成器，同时设置嵌套生成器的 `root_` 指针为当前生成器的根协程。

2. 如果嵌套生成器内部发生了异常，异常指针会在 `await_resume` 中设置，并通过 `std::rethrow_exception` 递归传递给当前生成器。

3. 嵌套生成器执行完成后，通过 `await_suspend` 将协程控制权返回给当前生成器，同时重新设置当前生成器的一些属性（如 `leaf_` 指针等）。

### 需要的三个生成器：

1. **当前生成器（Current Generator）**:

    - 当前生成器即是正在执行的生成器，它是调用嵌套生成器的生成器。表示为 `current`。

2. **嵌套生成器（Nested Generator）**:

    - 嵌套生成器是在当前生成器内部通过 `yield_sequence_awaiter` 结构体嵌套调用的生成器。表示为 `nested`。

3. **父生成器（Parent Generator）**:

    - 父生成器是当前生成器的直接调用者，也是嵌套生成器的外层生成器。表示为 `parent`。

### 调用树：

由此，我们还原调用树的建立以及维护过程：

1. **根协程的创建（Root Coroutine Creation）**:

    - 当创建一个生成器对象时，会同时创建一个根协程。根协程是整个协程调用树的根节点。（和）

2. **嵌套生成器的调用（Invocation of Nested Generator）**:

    - **当一个生成器在其协程体内通过 `co_yield` 调用另一个生成器时，会触发嵌套生成器的执行。**

    - 在 `yield_sequence_awaiter` 的 `await_suspend` 函数中，建立了嵌套生成器与当前生成器之间的关联关系。具体来说，将嵌套生成器的 `root_` 设置为当前生成器的根协程，将当前生成器的 `leaf_` 设置为嵌套生成器。

3. **异常的传递（Exception Propagation）**:

    - 在嵌套生成器执行过程中，如果发生了异常，异常会被捕获并设置在 `yield_sequence_awaiter` 的 `await_resume` 函数中。然后，通过 `std::rethrow_exception` 将异常递归传递给当前生成器。这样，异常就能够从嵌套生成器一直传递到当前生成器，形成了异常的递归传递。

4. **协程调用关系的维护（Coroutine Relationship Maintenance）**:
- 通过 `yield_sequence_awaiter` 中的操作，维护了生成器之间的调用关系。具体来说：
    
    - 当前生成器的 `leaf_` 指针指向了嵌套生成器。
    
    - 嵌套生成器的 `root_` 指针指向当前生成器的根协程。
    
    - 嵌套生成器的 `parent_` 指针指向当前生成器。

5. **协程的恢复和挂起（Resumption and Suspension of Coroutines）**:

    - 在 `await_suspend` 函数中，通过挂起当前生成器，将控制权转交给嵌套生成器。当嵌套生成器执行完成后，再次通过 `await_suspend` 将控制权返回给当前生成器。

这样，通过以上步骤，就在协程调用链上建立了一棵树状的结构。这棵树反映了生成器之间的嵌套关系，同时通过指针关联实现了协程调用的树状结构。在每一层的生成器调用中，都会维护相应的关联关系，使得协程能够递归调用并传递异常。



### 代码实现：

根据搭建调用树的思路，我们进行代码的实现

#### 创建根节点

```cpp
explicit generator(std::coroutine_handle<promise_type> coro) noexcept: coro_(coro) {}
```

###  `yield_sequence_awaiter` 结构体的实现



1. 在 `yield_sequence_awaiter` 结构体中，`await_suspend` 函数是嵌套生成器执行前的挂起点。在这里，当前生成器（`current`）将控制权挂起，将协程的执行权交给嵌套生成器（`gen_`）。

2. 同时，还设置了嵌套生成器的 `parent_` 指针指向当前生成器，建立了嵌套生成器与当前生成器的父子关系。

```cpp

std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> h) noexcept {

    auto& current = h.promise();

    auto& nested = gen_.coro_.promise();

    auto& root = current.root_;
        nested.root_ = root;

    root->leaf_ = &nested;

    nested.parent_ = &current;

    nested.exception_ = &exception_;
        nested.root_ = root;

    root->leaf_ = &nested;
    nested.parent_ = &current;
    nested.exception_ = &exception_;
        return gen_.coro_;

}

```



3. 在 `await_suspend` 函数中，首先获取了当前生成器的 `root_` 指针，然后将嵌套生成器的 `root_` 设置为当前生成器的根协程，建立了嵌套生成器与当前生成器之间的关联。

4. 如果在嵌套生成器的执行过程中发生了异常，异常指针将被设置，并在 `await_resume` 函数中通过 `std::rethrow_exception` 递归传递回当前生成器。



```cpp


void await_resume() {

    if (exception_)

        std::rethrow_exception(std::move(exception_));

}


```



### 异常值处理：



**`void unhandled_exception()`**函数用于处理协程执行过程中的未处理异常。如果协程执行过程中发生了异常且未被处理，即 `exception_` 指针不为 `nullptr`，则将当前异常对象存储在 `exception_` 指向的位置。如果 `exception_` 为 `nullptr`，则直接抛出当前异常。

```cpp

void unhandled_exception() {

    if (exception_ == nullptr)

        throw;

    *exception_ = std::current_exception();

}

```



## sleep.h：



为 `sleep` 结构体添加一些函数，以使其成为可等待对象（awaitable object），需要添加的函数：



- `await_ready` 函数返回 `false`，表示协程需要挂起。

- `await_suspend` 函数将一个带有延迟逻辑的函数（lambda）推送到 `task_queue` 中，然后协程挂起。

- `await_resume` 函数为空操作。

```cpp

    bool await_ready() const noexcept {

        return false;

    }



    void await_suspend(std::coroutine_handle<> h) const noexcept {

        auto start = std::chrono::steady_clock::now();

        task_queue.push([start, h, d = delay] {

            if (decltype(start)::clock::now() - start > d) {

                h.resume();

                return true;

            }

            else

                return false;

        });

    }



    void await_resume() const noexcept {}

```

Task结构体中需要添加一些函数，让 `Task` 成为一个协程句柄（coroutine handle）。

`wait_task_queue_empty`通过循环检查任务队列是否为空，如果任务队列不为空，则继续等待。这样的设计可能是为了确保所有在队列中的任务都得到执行，而不会在当前线程继续执行之前就返回。确保在程序的某个点，所有的异步任务都已经完成。

- 当队列不为空时，从队列头部取出一个任务执行。如果任务返回 `false`，表示任务未完成，则将任务重新推送到队列尾部，否则任务被弹出队列。

- 通过 `std::this_thread::sleep_for` 等待一小段时间，以避免过于频繁地尝试执行任务。

```cpp

void wait_task_queue_empty() {

    while (!task_queue.empty()) {

        auto task = task_queue.front();

        if (!task())

            task_queue.push(task);

        task_queue.pop();

        std::this_thread::s

```
