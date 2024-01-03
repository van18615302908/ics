# Y86-64实验报告

![image-20231221033428567](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231221033428567.png)

![image-20231221020735897](https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312210207946.png)

我的亮点就是采用了更加丰富的指令集，以及利用了LRU实现的cache来优化cpu的运行；在接下来的报告中我会进行讲解。

## 前端

采用了`html`加上`JavaScript`写了我的前端，在运行过程中，统计了我缓存的hit以及miss、eviction来让查找自己是哪一步导致了eviction，在前端可以通过按键，可以进行上一个PC以及下一个PC之间的切换，以及memory的查找。

<img src="https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312210122672.png" alt="image-20231221012246584" style="zoom:13%;" /><img src="https://van-1322764606.cos.ap-shanghai.myqcloud.com/202312210125607.png" alt="image-20231221012544392" style="zoom:14%;" />

### `memory`初始化

最开始进行memory的初始化，把yo文件中所有的指令全部按照地址信息读取进内存，定义了一个大小为一千的map`<int ,string>`进行索引；对于每一个字节，即两个十六进制数字进行储存。

```cpp
void CPUSimulator::initMemory() 
```

### 指令读取

![image-20231221025453946](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231221025453946.png)

```cpp
void CPUSimulator::run()
```

​	最开始从0位置根据PC的值进行读取指令，根据指令的`instuction_code`来进行判断读取几个byte，然后将指令读取进我的缓存中，此时地址将会被解析为三部分，来进行缓存中的定位以及hit、miss、eviction的判定，也就是`cache_lab`中学的知识。

​	对于每一条指令，将会从小端法十六进制补码被解读为大端法十进制long long格式被存储在我的缓存里，同时寄存器里也是这种形式，而内存中为了更加方便地进行`instuction_Code`的解析，我选择使用小端法十六进制补码进行储存以及处理。

### 数据存储

```cpp
    // 将数据写入缓存，如果缓存未命中，先从内存中读取数据
    void write(long long address, long long data);
    //从内存中读取信息
    std::string read_mem(long long b);
   // 从缓存中读取数据，如果缓存中没有，从内存中读取
    long long  read(long long address);
   //信息写入内存
    void write_mem(std::string a,long long b);
```

![image-20231221024727137](https://van-1322764606.cos.ap-shanghai.myqcloud.com/image-20231221024727137.png)

​	当数据被读取进来的时候，数据存储在我的缓存中，并且当数据进行更改的时候，也是在缓存中寻找对应位置进行更改；当需要在进行内存以及寄存器之间的信息交换的时候，信息从缓存读取到我的寄存器之中。当且仅当，缓存发生了`eviction`，需要移除一个或多个缓存项以腾出空间给新的数据的时候，缓存中最少次数被使用的一项会被更新到`memory`里面。

​	同理，在读取一个信息的时候，先看缓存中有没有这项数据，没有的话，先将数据从memory读取进我的缓存中，再对数据进行操作。因为cache和内存之间是异步操作，内存不会被及时更新；所以这里我每次更新缓存的同时把内存也更新了测试样例就过不了了，所以每次cache更新的时候我都把内存更新掉了。

### 指令的处理

```cpp
void CPUSimulator::executeInstruction(const std::string& instruction)
```

​	当指令通过`instuction_code`被解析之后，我们通过调用各种函数来进行相关操作。其中再调用函数后需要更新CPU的PC值来进行判断下一步读取什么指令。

其中又三个点需要注意

- `opq`操作时注意更新状态码，方便需要条件判断的函数比如`jl`、`jne`等等，来判断是否执行。
- `pushq`需要 如果注意调用`rsp`寄存器，需要先把寄存器信息存起来
- 在进行`pushq`的时候防止地址越界，如果越界需要更新状态码，终止程序的运行

对于新的指令集，然后我想起了自己在写`boom_lab`的时候，经常`rax`,`eax`到处倒腾，非常难受，我就写了一套直接对`rax`,`rdi`之类地址进行高低位操作的指令；还有一些直接对寄存器的值与某个值进行比较，避免了寄存器之间数值的相互倒腾；最后增添了乘除指令。

```cpp
   //两个寄存器的高位相加，存放在第三个寄存器里
    void highaddq(Register source1,Register source2);

    //两个寄存器的低位相加,存放在第三个寄存器里
    void lowaddq(Register source1,Register source2);

    //两个寄存器的高位相减,存放在第三个寄存器里
    void highsubq(Register source1,Register source2);

    //两个寄存器的低位相减,存放在第三个寄存器里
    void lowsub(Register source1,Register source2);

    //如果寄存器中的值大于某个值，就跳转
    void highjump(Register source,long long V, long long D);

    //如果寄存器中的值不大于某个值，就跳转
    void nothighjump(Register source,long long V, long long D);

    //如果寄存器中的值小于某个值，就跳转
    void lowjump(Register source,long long V, long long D);

    //如果寄存器中的值不小于某个值，就跳转
    void notlowjump(Register source,long long V, long long D);

    //两个寄存器的值相乘，结果存在第二个寄存器里
    void mulq(Register source1,Register source2);

    //两个寄存器的值相除，结果存放在第二个寄存器里
    void divq(Register source1,Register source2);
```

代码引用了nlohmann::json，运行需要的json.hpp文件我一并放入文件夹了











