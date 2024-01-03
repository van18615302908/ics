#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <json.hpp>
#include <bitset>
#include <vector>
#include <sstream>
#include <iomanip>
#include <list>

using json = nlohmann::json;
//十六进制转十进制
std::string hexToBinary(const std::string& hexString) ;

// 将小端法表示的十六进制string转换为大端法表示的十进制long long
long long convertLittleEndianHexToDecimal(const std::string &hexString) ;

// 将大端法表示的十进制long long转换为小端法表示的十六进制字符串
std::string convertDecimalToLittleEndianHex(long long decimalValue) ;

int hexCharToInt(char hexChar) ;

//得到一个地址的高地址部分
std::string getLowAddress(const std::string& input) ;


// 定义缓存行的大小和缓存的大小
const int CACHE_LINE_SIZE = 2;
const long long CACHE_SIZE = 16;

// 缓存行的数据结构
struct CacheLine {
    bool valid;
    long long tag;
    std::vector<long long> data;
    CacheLine() : valid(false), tag(-1), data(CACHE_LINE_SIZE, 0) {}
};

// LRU缓存替换策略
class LRUCache {
private:
    std::unordered_map<long long, std::list<long long>::iterator> indexMap;
    std::list<long long> lruList;
    long long capacity;
public:
    LRUCache(long long capacity) : capacity(capacity) {}

    long long index_will_delet();

    // 添加一个索引到LRU列表中
    void addIndex(long long index) ;

    // 获取最近最少使用的索引
    long long getLRUIndex() const ;
};





enum class Register {
    RAX, RCX, RDX, RBX, RSP, RBP,RSI,RDI,
    R8, R9, R10, R11, R12, R13, R14,F,
    REG_COUNT
};


//利用index索引寄存器
Register getRegisterAtIndex( int index) ;

// 辅助函数，将Register转换为字符串
std::string getRegisterName(Register reg) ;

class CacheSimulator;

// CPU状态结构体
struct CPUState {
    int64_t registers[static_cast<int>(Register::REG_COUNT)] = {0};
    long long PC = 0;
    std::map<int, std::string> memory={};
    int ZF = 1;
    int SF = 0;
    int OF = 0;
    int STATE = 1;
    int hits = 0;
    int misses = 0;
    int eviction = 0;

    // 将CPU状态转换为JSON对象
    json toJSON() const;
    json toJSON1() const;
};

// 模拟器类
class CPUSimulator {
public:
    CPUSimulator(long long capacity): lruCache(capacity) {
        for (int i = 0; i < static_cast<int>(Register::REG_COUNT); ++i) {
            CP.registers[i] = 0;
        }
        
    }

    void initMemory();
   //信息写入内存
    void write_mem(std::string a,long long b);
    //从内存中读取信息
    std::string read_mem(long long b);
    

    void run();

    // 从缓存中读取数据，如果缓存中没有，从内存中读取
    long long  read(long long address);

    // 将数据写入缓存，如果缓存未命中，先从内存中读取数据
    void write(long long address, long long data);

    // 获取缓存命中率
    double getHitRate() const ;

private:
    CPUState CP;
    //缓存
    std::unordered_map<long long, CacheLine> cache;
    //缓存的LRU逻辑实现
    LRUCache lruCache;


    // 更新缓存行
    void updateCache(long long index, long long tag, long long data,long long address);

    // 执行一条指令
    void executeInstruction(const std::string& instruction) ;

    void rrmovq(Register source, Register destination);

    void comvle(Register source, Register destination);

    void cmovl(Register source, Register destination);

    void cmove(Register source, Register destination);

    void cmovne(Register source, Register destination);

    void cmovge(Register source, Register destination);

    void cmovg(Register source, Register destination);

    void irmovq(Register destination, long long V);

    void rmmovq(Register source, Register destination, long long D);

    void mrmovq(Register destination, Register source, long long D);

    void addq(Register source, Register destination);

    void subq(Register source, Register destination);

    void andq(Register source, Register destination);

    void xorq(Register source, Register destination);

    void jle(long long Dest);

    void jl(long long Dest);

    void je(long long Dest);

    void jne(long long Dest);

    void jge(long long Dest);

    void jg(long long Dest);

    void call(long long Dest, int n);

    void ret();

    void pushq(Register source);

    void popq(Register destination);

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

};
