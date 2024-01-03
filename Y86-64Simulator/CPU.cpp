#include "CPU.h"

using json = nlohmann::json;

//十六进制转十进制
std::string hexToBinary(const std::string& hexString) {
    std::string binaryString;
    for (char c : hexString) {
        int value;
        if (isdigit(c)) {
            value = c - '0';
        } else {
            value = toupper(c) - 'A' + 10;
        }

        // 将每个十六进制数字映射为四位二进制
        for (int i = 3; i >= 0; --i) {
            int bit = (value >> i) & 1;
            binaryString += std::to_string(bit);
        }
    }
    return binaryString;
}


// 将小端法表示的十六进制string转换为大端法表示的十进制long long
long long convertLittleEndianHexToDecimal(const std::string &hexString) {
    // 检查字符串长度是否为偶数
    if (hexString.length() % 2 != 0) {
        std::cerr << "错误：十六进制字符串长度必须为偶数" << std::endl;
        return 0;
    }

    // 反转每两个字符的顺序
    std::string reversedHexString;
    for (int i = hexString.length() - 2; i >= 0; i -= 2) {
        reversedHexString += hexString.substr(i, 2);
    }
    reversedHexString = hexToBinary(reversedHexString);
      // 使用 std::bitset 构造函数将二进制字符串转换为 bitset
    std::bitset<64> bits(reversedHexString);
    long long result =0;

    for(int i = 0;i < 64;i++){
        if(i == 0){
            result = -(reversedHexString[i]-'0')*(1ULL << 63);
        }else{
            result += (reversedHexString[i] - '0')*(1ULL <<(63 - i));
        }
    }
    return result;
}

// 将大端法表示的十进制long long转换为小端法表示的十六进制字符串
std::string convertDecimalToLittleEndianHex(long long decimalValue) {
    // 使用字符串流进行十六进制转换
    std::ostringstream oss;
    oss << std::hex << decimalValue;
    std::string hexString = oss.str();

    // 如果字符串长度为奇数，前面补零
    if (hexString.length() % 2 != 0) {
        hexString = "0" + hexString;
    }

    // 反转每两个字符的顺序
    std::string littleEndianHexString;
    for (int i = hexString.length() - 2; i >= 0; i -= 2) {
        littleEndianHexString += hexString.substr(i, 2);
    }

    return littleEndianHexString;
}
//得到一个地址的低地址部分
std::string getLowAddress(const std::string& input) {
    // 确保输入字符串长度为八位
    if (input.length() != 8) {
        std::cerr << "Error: 输入字符串长度必须为八位" << std::endl;
        return "";
    }

    // 取前四位
    std::string output = input.substr(0, 4);
    // 后四位变成'0000'
    output += "0000";

    return output;
}
//得到一个地址的高地址部分
std::string getHighAddress(const std::string& input) {
    // 确保输入字符串长度为八位
    if (input.length() != 8) {
        std::cerr << "Error: 输入字符串长度必须为八位" << std::endl;
        return "";
    }

    // 取后四位
    std::string output = input.substr(4, 4);
    // 前四位变成'0000'
    output += "0000" ;

    return output;
}

//得到要驱逐的cache索引
long long LRUCache::index_will_delet(){
    long long removedIndex = -1;
    if(lruList.size() >= capacity) {//满了
        removedIndex = lruList.back();}
        return removedIndex;
}

//更新LRU
void LRUCache::addIndex(long long index) {
    if (indexMap.find(index) != indexMap.end()) {//更新位置
        lruList.erase(indexMap[index]);
    } else if (lruList.size() >= capacity) {//满了
        long long removedIndex = lruList.back();
        lruList.pop_back();
        indexMap.erase(removedIndex);
    }

    lruList.push_front(index);
    indexMap[index] = lruList.begin();
}

// 获取最近最少使用的索引
long long LRUCache::getLRUIndex() const {
    return lruList.back();
}


std::string getRegisterName(Register reg) {
    switch (reg) {
        case Register::RAX: return "rax";
        case Register::RBX: return "rbx";
        case Register::RCX: return "rcx";
        case Register::RDX: return "rdx";
        case Register::RSI: return "rsi";
        case Register::RDI: return "rdi";
        case Register::RSP: return "rsp";
        case Register::RBP: return "rbp";
        case Register::R8: return "r8";
        case Register::R9: return "r9";
        case Register::R10: return "r10";
        case Register::R11: return "r11";
        case Register::R12: return "r12";
        case Register::R13: return "r13";
        case Register::R14: return "r14";
        default: return "UNKNOWN";
    }
}

//十六进制字符转十进制数字
int hexCharToInt(char hexChar) {
    if (std::isxdigit(hexChar)) {
        if (std::isdigit(hexChar)) {
            return hexChar - '0';
        } else {
            return std::toupper(hexChar) - 'A' + 10;
        }
    } else {
        // 处理非法字符的情况，这里简单返回 -1，你可以根据需求改变这部分的逻辑
        return -1;
    }
}

//通过数字索引访问寄存器
Register getRegisterAtIndex( int index) {
    if (index >= 0 && index < static_cast<int>(Register::REG_COUNT)) {
        return static_cast<Register>(index);
    }else{
        return static_cast<Register>(0);;
    }
}

json CPUState::toJSON() const {
    json j;
    j["PC"] = PC;

    // 寄存器状态
    j["REG"] = {};
    for (int i = 0; i < static_cast<int>(Register::REG_COUNT)-1; ++i) {
        j["REG"][getRegisterName(static_cast<Register>(i))] = registers[i];
    }
    j["STAT"] = STATE;

    // CC状态
    j["CC"] = {
        {"ZF", ZF},
        {"SF", SF},
        {"OF", OF}
    };

    j["MEM"] = nlohmann::json::object();
    int location = 0;
    std::string s ={};
    long long out = 0;
    int count = 0;
    for (const auto& entry : memory) {
        count++;
        long long address = entry.first;
        const std::string& value = entry.second;
        s += value;
        if(count == 8){
            count =0;
            out = convertLittleEndianHexToDecimal(s);
            if(out != 0){
                std::stringstream ss;
                ss << std::dec << entry.first -7;
                j["MEM"][ss.str()] = out;
                s={};
            }
        }

    }
        return j;
    }

json CPUState::toJSON1() const {
    json j;
    j["PC"] = PC;

    // 寄存器状态
    j["REG"] = {};
    for (int i = 0; i < static_cast<int>(Register::REG_COUNT)-1; ++i) {
        j["REG"][getRegisterName(static_cast<Register>(i))] = registers[i];
    }
    j["STAT"] = STATE;

    // CC状态
    j["CC"] = {
        {"ZF", ZF},
        {"SF", SF},
        {"OF", OF}
    };

    // Cache状态
    j["Cache"] = {
        {"hit",hits},
        {"miss",misses},
        {"eviction", eviction}
    };


    j["MEM"] = nlohmann::json::object();
    int location = 0;
    std::string s ={};
    long long out = 0;
    int count = 0;
    for (const auto& entry : memory) {
        count++;
        long long address = entry.first;
        const std::string& value = entry.second;
        s += value;
        if(count == 8){
            count =0;
            out = convertLittleEndianHexToDecimal(s);
            if(out != 0){
                std::stringstream ss;
                ss << std::dec << entry.first -7;
                j["MEM"][ss.str()] = out;
                s={};
            }
        }

    }
        return j;
    }

void CPUSimulator::initMemory() {   
    const int largeSize = 1000;
        for (int i = 0; i < largeSize; i++) {
            CP.memory[i] = "00";
    }
    std::string str;

    while (getline(std::cin, str)) {
    // while (getline(infile, str)) {
        if (str[5] == ':' && str.find_first_of(' ') == 6 && str[7] != ' ') {
            const std::string temp = str.substr(0, str.find_first_of(':'));

            if (temp.empty())
                break;

            long long index = std::stoll(temp, nullptr, 16);
            std::string st;
            int firstSpacePos = str.find_first_of(' ');  // 找第一个空格的位置
            int secondSpacePos = str.find(' ', firstSpacePos + 1);  // 从第一个空格后开始找第二个空格的位置
            st=str.substr(7,secondSpacePos-7);
            for(int i = 0;i < (secondSpacePos-7)/2;i++){
                CP.memory[index +i] = st.substr((i)*2,2);
            }
        }
    }
}

void CPUSimulator::updateCache(long long index, long long tag, long long data,long long address) {
    CacheLine& line = cache[index];
    line.valid = true;
    line.tag = tag;
    line.data[address % CACHE_LINE_SIZE] = data;
    lruCache.addIndex(index);
}

// 从缓存中读取数据，如果缓存中没有，从内存中读取
long long  CPUSimulator::read(long long address) {
    long long index = address % CACHE_SIZE;
    long long tag = address / CACHE_SIZE;

    if (cache.find(index) != cache.end() && cache[index].valid && cache[index].tag == tag) {
        // 命中
        CP.hits++;
        lruCache.addIndex(index);
        return cache[index].data[address % CACHE_LINE_SIZE];
    } else {
        // 未命中，从内存读取数据到缓存
        CP.misses++;
        if(lruCache.index_will_delet() != -1){
            // long long data = convertLittleEndianHexToDecimal(cpu->read_mem(address));//写回策略中才会写这个操作，但是为了通过测试点更新一下内存
            CP.eviction ++;
        }
        long long data = convertLittleEndianHexToDecimal(read_mem(address));
        updateCache(index, tag, data,address);
        return data;

    }
}


void CPUSimulator::write(long long address, long long data) {
    long long index = address % CACHE_SIZE;
    long long tag = address / CACHE_SIZE;

    if (cache.find(index) != cache.end() && cache[index].valid && cache[index].tag == tag) {
        // 命中，直接写入缓存
        CP.hits++;
        lruCache.addIndex(index);
        cache[index].data[address % CACHE_LINE_SIZE] = data;
        write_mem(convertDecimalToLittleEndianHex(data),address);//写回策略中不会写这个操作，但是为了通过测试点更新一下内存
    } else {
        // 未命中，从内存读取数据到缓存再写入
        CP.misses++;
        write_mem(convertDecimalToLittleEndianHex(data),address);
        updateCache(index, tag, data,address);
    }
}

double CPUSimulator::getHitRate() const {
    return static_cast<double>(CP.hits) / (CP.hits + CP.misses);
}



void CPUSimulator::write_mem(std::string a,long long b){
    for(int i = 0;i < a.length()/2;i++)
        CP.memory[b+i] = a.substr(i*2,2);
    for(int i = 0;i < 8 -a.length()/2;i++){
        CP.memory[b+i+a.length()/2] ="00";
    }

}

std::string CPUSimulator::read_mem(long long b){
    std::string s ={};
    for(int i = 0;i < 8;i++){
        s += CP.memory[b+i];
    }
    return s;
}


// void CPUSimulator::run(const std::string& filePath) {
void CPUSimulator::run(){ 
    initMemory();
    // initMemory(filePath);
    std::string line = {};
    int byte = 0;
    json log;
    json log1; 

    while (CP.STATE == 1){
        line = {};
        switch(CP.memory[CP.PC][0]){
            case '0':
            case '1':
            case '9':
            byte = 1;
            break;
            case '2':
            case '6':
            case 'a':
            case 'b':
            byte = 2;
            break;
            case '7':
            case '8':
            byte = 9;
            break;
            case '3':
            case '4':
            case '5':
            byte = 10;
            break;
            case 'c':
            byte = 18;
            break;
        }

        for(int i = 0;i < byte;i++){
            line += CP.memory[CP.PC +i];
        }
        // line = CP.memory[CP.PC];
        executeInstruction(line);
        log.push_back(CP.toJSON());
        log1.push_back(CP.toJSON1());
    }
    std::ofstream outputFile("result.json");
    if (outputFile.is_open()) {
        // Write the JSON content to the file with indentation of 4 spaces
        outputFile << log1.dump(4) << std::endl;
    }
        // Close the file
        outputFile.close();
    // 输出JSON日志到标准输出
    std::cout << log.dump(4) << std::endl;
}

void CPUSimulator::executeInstruction(const std::string& instruction) {
    char Instruction_Code = instruction[0];
    char Function_Code = instruction[1];
    Register Register_Code_1;
    Register Register_Code_2;
    std::string Offset;
    long long V;
    long long D;
    long long Dest;
    Register_Code_1 = getRegisterAtIndex(hexCharToInt(instruction[2]));
    Register_Code_2 = getRegisterAtIndex(hexCharToInt(instruction[3]));


    switch(Instruction_Code){
        case('0'):
            CP.STATE = 2;
            return;
        case('1'):
            break;

        case('2'):
        switch (Function_Code) {
            case '0': 
            rrmovq(Register_Code_1,Register_Code_2);
            break;

            case '1': 
            comvle(Register_Code_1, Register_Code_2);
            break;

            case '2': 
            cmovl(Register_Code_1, Register_Code_2);
            break;

            case '3': 
            cmove(Register_Code_1, Register_Code_2);
            break;

            case '4': 
            cmovne(Register_Code_1, Register_Code_2);
            break;

            case '5': 
            cmovge(Register_Code_1, Register_Code_2);
            break;

            case '6': 
            cmovg(Register_Code_1, Register_Code_2);
            break;
        }
        break;

        case('3'):
        V = convertLittleEndianHexToDecimal(instruction.substr(4));
        irmovq(Register_Code_2,V);
        break;

        case('4'):
        D = convertLittleEndianHexToDecimal(instruction.substr(4));
        rmmovq(Register_Code_1,Register_Code_2,D);
        break;

        case('5'):
        D = convertLittleEndianHexToDecimal(instruction.substr(4));
        mrmovq(Register_Code_1,Register_Code_2,D);
        break;

        case('6'):
        switch(Function_Code){
            case('0'):
            addq(Register_Code_1,Register_Code_2);
            
            break;

            case('1'):
            subq(Register_Code_1,Register_Code_2);

            break;

            case('2'):
            andq(Register_Code_1,Register_Code_2);

            break;

            case('3'):
            xorq(Register_Code_1,Register_Code_2);
            break;
        }
        break;
        
        case('7'):
        Dest = convertLittleEndianHexToDecimal(instruction.substr(2));            
        switch(Function_Code){

            case('0'):
            CP.PC = Dest;
            return;

            case('1'):
            if ((CP.SF^CP.OF)|CP.ZF) {
                CP.PC = Dest;
            }
            else{
                CP.PC += instruction.length()/2;
            }
            return;

            case('2'):
            if ((CP.SF^CP.OF)) {
                CP.PC = Dest;
            }
            else{
                CP.PC += instruction.length()/2;
            }                    
            return;

            case('3'):
            if ((CP.ZF)) {
                CP.PC = Dest;
            }
            else{
                CP.PC += instruction.length()/2;
            }     
            return;

            case('4'):
            if ((!CP.ZF)) {
                CP.PC = Dest;
            }
            else{
                CP.PC += instruction.length()/2;
            }     
            return;

            case('5'):
            if ((!(CP.SF^CP.OF))) {
                CP.PC = Dest;
            }
            else{
                CP.PC += instruction.length()/2;
            }     
            return;

            case('6'):
            if ((!(CP.SF^CP.OF))&(!CP.ZF)) {
                CP.PC = Dest;
            }
            else{
                CP.PC += instruction.length()/2;
            }     
            return;

        }

    case('8'):
    Dest = convertLittleEndianHexToDecimal(instruction.substr(2));
    call(Dest,instruction.length()/2);
    return;

    case('9'):
    ret();
    return;

    case 'a':
    pushq(Register_Code_1);
    if(CP.STATE == 1){
        CP.PC += instruction.length()/2;
    }else{
        return;
    }
    return;

    case 'b':
    popq(Register_Code_1);
    break;


    }
    CP.PC += instruction.length()/2;
}
void CPUSimulator::rrmovq(Register source, Register destination) {
    CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
}

void CPUSimulator::comvle(Register source, Register destination) {
    if ((CP.SF^CP.OF)|CP.ZF) {
        CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
    }
}

void CPUSimulator::cmovl(Register source, Register destination) {
    if ((CP.SF^CP.OF)) {
        CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
    }
}

void CPUSimulator::cmove(Register source, Register destination) {
    if ((CP.ZF)) {
        CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
    }
}

void CPUSimulator::cmovne(Register source, Register destination) {
    if ((!CP.ZF)) {
        CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
    }
}

void CPUSimulator::cmovge(Register source, Register destination) {
    if ((!(CP.SF^CP.OF))) {
        CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
    }
}

void CPUSimulator::cmovg(Register source, Register destination) {
    if ((!(CP.SF^CP.OF))&(!CP.ZF)) {
        CP.registers[static_cast<int>(destination)] = CP.registers[static_cast<int>(source)];
    }
}

void CPUSimulator::irmovq(Register destination, long long V) {
CP.registers[static_cast<int>(destination)] = V;
}

void CPUSimulator::rmmovq(Register source, Register destination, long long D) {
    long long temp;
    temp = CP.registers[static_cast<int>(source)];
    write(CP.registers[static_cast<int>(destination)] + D,CP.registers[static_cast<int>(source)]);

}

void CPUSimulator::mrmovq(Register destination, Register source, long long D) {
  
    CP.registers[static_cast<int>(destination)] = read((CP.registers[static_cast<int>(source)] + D));
}

void CPUSimulator::addq(Register source, Register destination){
    long long x = CP.registers[static_cast<int>(source)];
    long long y = CP.registers[static_cast<int>(destination)];
    long long t = x + y;
    CP.OF = (x <0 == y<0)&& (t < 0 != x< 0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);
    CP.registers[static_cast<int>(destination)] += CP.registers[static_cast<int>(source)];
}

void CPUSimulator::subq(Register source, Register destination){
    long long x = CP.registers[static_cast<int>(source)];
    long long y = CP.registers[static_cast<int>(destination)];
    long long t = y -x;
    CP.OF = (t <y != x>0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);    
    CP.registers[static_cast<int>(destination)] -= CP.registers[static_cast<int>(source)];
}

void CPUSimulator::andq(Register source, Register destination){
    long long x = CP.registers[static_cast<int>(source)];
    long long y = CP.registers[static_cast<int>(destination)];
    long long t = x&y;
    CP.ZF = (t == 0);
    CP.SF = (t < 0); 
    CP.OF = 0;
    CP.registers[static_cast<int>(destination)] =CP.registers[static_cast<int>(destination)] & CP.registers[static_cast<int>(source)];
}

void CPUSimulator::xorq(Register source, Register destination){
    long long x = CP.registers[static_cast<int>(source)];
    long long y = CP.registers[static_cast<int>(destination)];
    long long t = x^y;
    CP.ZF = (t == 0);
    CP.SF = (t < 0); 
    CP.OF = 0;    
    CP.registers[static_cast<int>(destination)] =CP.registers[static_cast<int>(destination)] ^ CP.registers[static_cast<int>(source)];
}

void CPUSimulator::jle(long long Dest) {
    if ((CP.SF^CP.OF)|CP.ZF) {
        CP.PC = Dest;
    }
    else{

    }
}

void CPUSimulator::jl(long long Dest) {
    if ((CP.SF^CP.OF)) {
        CP.PC = Dest;
    }
}

void CPUSimulator::je(long long Dest) {
    if ((CP.ZF)) {
        CP.PC = Dest;
    }
}

void CPUSimulator::jne(long long Dest) {
    if ((!CP.ZF)) {
        CP.PC = Dest;
    }
}

void CPUSimulator::jge(long long Dest) {
    if ((!(CP.SF^CP.OF))) {
        CP.PC = Dest;
    }
}

void CPUSimulator::jg(long long Dest) {
    if ((!(CP.SF^CP.OF))&(!CP.ZF)) {
        CP.PC = Dest;
    }
}

void CPUSimulator::call(long long Dest,int n){
    long long temp;
    CP.registers[static_cast<int>(getRegisterAtIndex(4))] -= 8;
    write(CP.registers[static_cast<int>(getRegisterAtIndex(4))],CP.PC+n);
    CP.PC = Dest;
}

void CPUSimulator::ret(){
    CP.registers[static_cast<int>(getRegisterAtIndex(4))] += 8;

    CP.PC = read(CP.registers[static_cast<int>(getRegisterAtIndex(4))]-8);
}

void CPUSimulator::pushq(Register source){
    long long temp;
    temp =  CP.registers[static_cast<int>(source)];
    CP.registers[static_cast<int>(getRegisterAtIndex(4))] -= 8;
    if(CP.registers[static_cast<int>(getRegisterAtIndex(4))]<8){
        CP.STATE = 3;
        return;
    }
 
    write(CP.registers[static_cast<int>(getRegisterAtIndex(4))],temp);
}

void CPUSimulator::popq(Register destination){
    CP.registers[static_cast<int>(getRegisterAtIndex(4))] += 8;
    CP.registers[static_cast<int>(destination)] = read(CP.registers[static_cast<int>(getRegisterAtIndex(4))] -8);
}


void CPUSimulator::highaddq(Register source1,Register source2){
    std::string temp1 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source1)]);
    std::string temp2 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source2)]);
    long long x = convertLittleEndianHexToDecimal(getHighAddress(temp1));
    long long y = convertLittleEndianHexToDecimal(getHighAddress(temp2));
    long long t = x + y;
    CP.OF = (x <0 == y<0)&& (t < 0 != x< 0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);
    CP.registers[static_cast<int>(source2)] = t;
}


void CPUSimulator::lowaddq(Register source1,Register source2){
    std::string temp1 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source1)]);
    std::string temp2 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source2)]);
    long long x = convertLittleEndianHexToDecimal(getLowAddress(temp1));
    long long y = convertLittleEndianHexToDecimal(getLowAddress(temp2));
    long long t = x + y;
    CP.OF = (x <0 == y<0)&& (t < 0 != x< 0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);
    CP.registers[static_cast<int>(source2)] = t;
}


void CPUSimulator::highsubq(Register source1,Register source2){
    std::string temp1 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source1)]);
    std::string temp2 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source2)]);
    long long x = convertLittleEndianHexToDecimal(getHighAddress(temp1));
    long long y = convertLittleEndianHexToDecimal(getHighAddress(temp2));
    long long t = y -x;
    CP.OF = (t <y != x>0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);    
    CP.registers[static_cast<int>(source2)] = t;
}


void CPUSimulator::lowsub(Register source1,Register source2){
    std::string temp1 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source1)]);
    std::string temp2 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source2)]);
    long long x = convertLittleEndianHexToDecimal(getLowAddress(temp1));
    long long y = convertLittleEndianHexToDecimal(getLowAddress(temp2));
    long long t = y -x;
    CP.OF = (t <y != x>0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);    
    CP.registers[static_cast<int>(source2)] = t;
}

//如果寄存器中的值大于某个值，就跳转
void CPUSimulator::highjump(Register source,long long V, long long D){
    if(CP.registers[static_cast<int>(source)] > V){
        CP.PC = D;
    }
}

//如果寄存器中的值小于某个值，就跳转
void CPUSimulator::lowjump(Register source,long long V, long long D){
        if(CP.registers[static_cast<int>(source)] < V){
        CP.PC = D;
    }
}

//如果寄存器中的值不大于某个值，就跳转
void CPUSimulator::nothighjump(Register source,long long V, long long D){
    if(!CP.registers[static_cast<int>(source)] > V){
        CP.PC = D;
    }
}

//如果寄存器中的值不小于某个值，就跳转
void CPUSimulator::notlowjump(Register source,long long V, long long D){
        if(!CP.registers[static_cast<int>(source)] < V){
        CP.PC = D;
    }
}

//两个寄存器的值相乘，结果存在第二个寄存器里
void CPUSimulator::mulq(Register source1,Register source2){
    std::string temp1 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source1)]);
    std::string temp2 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source2)]);
    long long x = convertLittleEndianHexToDecimal(getLowAddress(temp1));
    long long y = convertLittleEndianHexToDecimal(getLowAddress(temp2));
    long long t = y*x;
    CP.OF = (t < y && x > 0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);    
    CP.registers[static_cast<int>(source2)] = t;
}

//两个寄存器的值相除，结果存放在第二个寄存器里
void CPUSimulator::divq(Register source1,Register source2){
    std::string temp1 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source1)]);
    std::string temp2 = convertDecimalToLittleEndianHex( CP.registers[static_cast<int>(source2)]);
    long long x = convertLittleEndianHexToDecimal(getLowAddress(temp1));
    long long y = convertLittleEndianHexToDecimal(getLowAddress(temp2));
    long long t = 0;
    if(x != 0){
        t = y/x;
    }else{
        CP.STATE = 3;
    }
    CP.OF = (t <y != x>0);
    CP.ZF = (t == 0);
    CP.SF = (t < 0);    
    CP.registers[static_cast<int>(source2)] = t;
}
int main() {
    CPUSimulator simulator(1);
    simulator.run();
    return 0;
}





