/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>Fan Yiyang 22307110117
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
#include "bits.h"

//P1
/* 
 * tconst - return a constant value 0xFFFFFFE0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int tconst(void) {
	return ~0x1F;
}

//P2
/* 
 * bitNand - return ~(x&y) using only ~ and | 
 *   Example: bitNand(4, 5) = -5, bitNand(3, 11) = -4
 *   Legal ops: ~ |
 *   Max ops: 6
 *   Rating: 2
 */
int bitNand(int x, int y) {
  return (~x)|(~y);//根据摩根定理，~(x&y) == (~x)|(~y)
}

//P3
/*
 * ezOverflow - determine if the addition of two signed positive numbers overflows,
 *      and return 1 if overflowing happens and 0 otherwise
 *   You can assume 0 <= x, 0 <= y
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int ezOverflow(int x,int y) {
  int sum = x + y;
	return (sum >> 31)&1;//由于c语言右移是逻辑右移，需要删除前面所有1，才能使溢出时return 1
}

//P4
/* 
 * fastModulo - return x%(2^y)
 *   You can assume 0 <= x, 1 <= y <= 31
 *   Example: fastModulo(0x1234567,0xA) = 0x167
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 3
 */
int fastModulo(int x,int y) {
  int tool = (1<<y) + ~0;//创建一个工具，需要求余的位上是1，其余溢出出去；让x与工具做与运算，进而求余
	return (x&tool);
}

//P5
/*
 * findDifference - return a mask that marks the different bits of y compared to x
 *   Example: findDifference(12, 10) = findDifference(10, 12) = 6
 *   Legal ops: ~ & |
 *   Max ops: 9
 *   Rating: 3
 */
int findDifference(int x,int y) {
  int mask = (x&(~y))|((~x)&y);//即返回x和y的异或值
	return mask;
}

//P6
/*
 * absVal - return the absolute value of x
 *   Examples: absVal(-10) = 10
 *			   absVal(5) = 5
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 4
 */
int absVal(int x) {
  int tool = x >> 31;//考虑正负数在做什么处理的时候有区别
  return (x+tool)^tool;//负数转正数等于取反+1，等于-1取反；负数+（～0）即负数-1
}

//P7
/*
 * secondLowBit - return a mask that marks the position of the second least significant 1 bit.
 *   Examples: secondLowBit(0x00000110) = 0x00000100
 *			       secondLowBit(0xFEDC1A80) = 0x00000200
 *             secondLowBit(0)  = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 4
 */
int secondLowBit(int x) {
  int lowest = x&(~x+1);//找出最小的有效位
  int mask = (x^lowest)&(~(x^lowest)+1);//将最小的有效位删除，然后重复操作
	return mask;
}

//P8
/*
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 20
 *  Rating: 5
 */
int byteSwap(int x, int n, int m) {
  int bytemask = 0xFF;//匹配一个byte的掩码
  int swap_n = (x>>(n<<3))&bytemask;//找出你要交换的byte
  int swap_m = (x>>(m<<3))&bytemask;
  
  x = x ^ (swap_n<<(n<<3));//删除要交换的byte
  x = x ^ (swap_m<<(m<<3));

  x = x | (swap_m<<(n<<3));//输入交换的byte
  x = x | (swap_n<<(m<<3));
	return x;
}

//P9
/* 
 * byteCheck - Returns the number of bytes that are not equal to 0
 *   Examples: byteCheck(0xFE0A4100) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 5
 */
int byteCheck(int x) {

    int byteMask = 0xFF;//匹配一个byte的掩码
    int byte1 = (x >> 0) & byteMask;
    int byte2 = (x >> 8) & byteMask;
    int byte3 = (x >> 16) & byteMask;
    int byte4 = (x >> 24) & byteMask;
    
    // 统计不等于0的字节数
    int count = !!byte1 + !!byte2 + !!byte3 + !!byte4;//使用！！把不是0的byte数值强制转化为1
    
    return count;
}
//P10
/* 
 * fractions - return floor(x*7/16), for 0 <= x <= (1 << 28), x is an integer 
 *   Example: fractions(20) = 8
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 5
 */
int fractions(int x) {
  int result = x + (x<<2) + (x<<1);//先x*7
  result >>= 4;// result/16
  return result;
}

//P11
/* 
 * biggerOrEqual - if x >= y  then return 1, else return 0 
 *   Example: biggerOrEqual(4,5) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 6
 */
int biggerOrEqual(int x, int y) {
  int tool=x + (~y+1);//x -y
  int Sign = (tool>>31)&(0x1); //x -y的符号
  int mask = 1<<31;//最大位为1的32位有符号数
  int xSign = x&mask;//x的符号
  int ySign = y&mask;//y的符号
  int bitXor = xSign ^ ySign;//x和y符号相同标志位，相同为0不同为1
  bitXor = (bitXor>>31)&1;//符号相同标志位格式化为0或1
  return ((!bitXor)&(!Sign))|(bitXor&(y>>31));//返回1有两种情况：符号相同标志位为0（相同）位与x-y 的符号为0；符号相同标志位为1（不同）位与y的符号位为1
}




//P12
/*
 * hdOverflow - determine if the addition of two signed 32-bit integers overflows,
 *      and return 1 if overflowing happens and 0 otherwise
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 6
 */
int hdOverflow(int x, int y) {
  int sum = x + y;
  int x_sign = x >> 31; // 获取x的符号位
  int y_sign = y >> 31; // 获取y的符号位
  int sum_sign = sum >> 31; // 获取sum的符号位
  int overflow_condition = ((x_sign & y_sign & !sum_sign) | (!x_sign & !y_sign & sum_sign));// 溢出条件为当x和y都为正数时，sum为负数，或者当x和y都为负数时，sum为正数
  return overflow_condition;
}




//P13
/* 
 * overflowCalc - given binary representations of three 32-bit positive numbers and add them together, 
 *      return the binary representation of the part where bits are higher than 32.
 *   Examples: overflowCalc(0xffffffff,0xffffffff,0xffffffff) = 2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 7
 */
int overflowCalc(int x, int y, int z) {

    int sum = x + y;
    int overflow = ((x & y) | ((x | y) & ~sum))>> 31;
    int sum2 = sum + z;
    int overflow2 = ((sum & z) | ((sum | z) & ~sum2))>>31;
    int result = (((overflow&1) + (overflow2&1))&3);

    return result;
}

//P14
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 8
 */
int logicalShift(int x, int n) {
  int mask = ((1 << 31) >> (n + ~0)); // 创建一个掩码，用于将高于n位的位数清零
  mask = mask + !n;//处理是0的情况
  return (x >> n)&(~mask); // 右移并清除高于n位的位数
}
    

//P15
/* 
 * partialFill - given l, h, fill all even digits within the [l, h] interval with 1 (subscripts starting from 0)
 *   Can assume that 0 <= l <= 15, 16 <= h <= 31
 *   Examples: partialFill(13,16) = 0x00014000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 8
 */
int partialFill(int l, int h) {
  int tool1 = 0x55;
  int minus_l = ~l + 1;
  int minus_h = ~h + 1;
  int tool2 = (tool1 << 8)| tool1;
  int mask = (tool2<<16)|tool2; // 创建一个掩码，其中偶数位都为1，奇数位都为0
  int tool3 = 1 << 31;
  int maskL = (tool3 >>((31+minus_l)));
  int mask2 = (tool3 >> ((31+minus_h)+(~0)));
  int maskH = mask2 + !(31+minus_h);//排除为0的情况
  int rangeMask = (maskL ^ maskH); // 创建一个掩码，将 [l, h] 区间内的位都设置为1
  return (mask & rangeMask); // 结合两个掩码来填充偶数位
}

//P16
/* 
 * float_abs - Return bit-level equivalent of expression |f| (absolute value of f) for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 20
 *   Rating: 3
 */
unsigned float_abs(unsigned uf) {
  int abs = uf & 0x7FFFFFFF;
  if  (abs > 0x7F800000){
    return uf;
  }
  return abs;
}

//P17
/* 
 * float_cmp - Return 1 if uf1 > uf2, and 0 otherwise.
 *   Both of the arguments are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When ant of the arguments is NaN, return 0.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 40
 *   Rating: 5
 */
unsigned float_cmp(unsigned uf1, unsigned uf2) {
  unsigned exp_mask = 0xFF << 23;  // 掩码用于提取指数部分
  unsigned frac_mask = (1 << 23) - 1;  // 掩码用于提取尾数部分
  unsigned exp1 = uf1 & exp_mask;
  unsigned frac1 = uf1 & frac_mask;
  unsigned exp2 = uf2 & exp_mask;
  unsigned frac2 = uf2 & frac_mask;
  // 如果 uf1 或 uf2 的指数部分全为1并且尾数部分不全为0，则为 NaN。
  int is_nan1 = (exp1 == exp_mask) && frac1;
  int is_nan2 = (exp2 == exp_mask) && frac2;
  int sign;

  if (is_nan1 || is_nan2) {
    return 0;  // 如果任何一个参数是 NaN，则返回0。
  }

  if((uf1 == 0x0 && uf2 == 0x80000000) || (uf1 == 0x80000000 && uf2 == 0x0)){
    return 0;//排除正负0
  }
 
  sign = uf2 >> 31;
  // 如果 uf1 和 uf2 具有不同的符号位，则比较它们的符号位来决定大小。
  if ((uf1 ^ uf2) & 0x80000000) {
    return (uf2 >> 31);
  }
  //符号位相同就比较指数
  if(exp1 == exp2){
   if(frac1 == frac2){
    return 0 ;
    }
  return (frac1 >frac2)^sign;
  }

  if(exp1 > exp2){
    return !sign;
  }

 if(exp1 < exp2){
    return sign;
  }
  
}


//P18
/* 
 * float_pow2 - Return bit-level equivalent of expression f*(2^n) for
 *   floating point argument f and integer argument n.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   You can assume 0 <= n <= 255
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 50
 *   Rating: 6
 */
unsigned float_pow2(unsigned uf, int n) {
  unsigned power_of_2,result,exp_mask,frac_mask,exp,frac;
  int sign;
  if ( uf == 0 || uf == 0x80000000) {
      return uf;
  }
  sign = uf & 0x80000000;
  uf = uf & 0x7fffffff;
  if(uf <= 0x7fffff){
  while(uf <= 0x7fffff){
      
      if(((uf <<1) <=0xffffff)&&(n > 0)){
        uf = (uf << 1);
        n = n-1;
      }
      if(n ==0){
      return (sign|uf);
      }
    }
  }
    exp_mask = 0xFF << 23;  // 用于提取指数部分的掩码
    frac_mask = (1 << 23) - 1;  // 用于提取尾数部分的掩码
    exp = uf & exp_mask;  // 提取uf的指数部分
    frac = uf & frac_mask;  // 提取uf的尾数部分
    
    // 当 uf 是 NaN 时，直接返回 uf
    if (exp == exp_mask && frac != 0 ) {
        return (uf|sign);
    }


    // 计算 2^n 的位级表示
    power_of_2 = (n << 23) + exp;

  if (power_of_2>=0x7f800000){
    return (0x7f800000| sign);//如果溢出，强制转化为最大或者最小
  }
    // 构建新的浮点数表示
    result = sign | (power_of_2 + frac);

    return result;
}



//P19
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 40
 *   Rating: 7
 */
unsigned float_i2f(int x) {
  unsigned sign,frac;
  int exp,ninth,eighth,siven,judge;
  sign = 0;
  if (x==0)
    return 0;
  if (x<0)//先转化绝对值，因为后面需要用到符号位，所以选择用符号位作为下一个if条件
    sign=0x80000000;
  if (sign)
    frac=-x;
  else
    frac=x;
  
  exp=158;//exp的初始值
  while (!(frac&0x80000000)) {//将尾数强制左移
    exp--;
    frac<<=1;
  }
  ninth=(frac>>8)&1;
  eighth=(frac>>7)&1;
  siven=frac&0x7f;//如果第八位等于1，同时低位还有非零值则进位；如果第九位为1并且后八位为0x80则进位
  judge=(eighth&&siven)||(eighth&&!siven&&ninth);
  frac=(frac<<1)>>9;//将原尾数左移 1 位并去掉最高位，得到舍入后的尾数。
  frac+=judge;
  if (frac>=0x800000) {//检查尾数是否超过规格化浮点数范围
    frac=((frac+0x800000)>>1)-0x800000;
    exp+=1;
  }
  return (sign|(exp<<23)|frac);
}


#ifdef NOT_SUPPOSED_TO_BE_DEFINED
#   __          __  _                          
#   \ \        / / | |                         
#    \ \  /\  / /__| | ___ ___  _ __ ___   ___ 
#     \ \/  \/ / _ \ |/ __/ _ \| '_ ' _ \ / _ \
#      \  /\  /  __/ | (_| (_) | | | | | |  __/
#       \/  \/ \___|_|\___\___/|_| |_| |_|\___|
#                                              

#  ██╗  ██╗ ██████╗ ███╗   ██╗ ██████╗ ██████╗     ██████╗  █████╗ ██████╗ ████████╗
#  ██║  ██║██╔═══██╗████╗  ██║██╔═══██╗██╔══██╗    ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝
#  ███████║██║   ██║██╔██╗ ██║██║   ██║██████╔╝    ██████╔╝███████║██████╔╝   ██║   
#  ██╔══██║██║   ██║██║╚██╗██║██║   ██║██╔══██╗    ██╔═══╝ ██╔══██║██╔══██╗   ██║   
#  ██║  ██║╚██████╔╝██║ ╚████║╚██████╔╝██║  ██║    ██║     ██║  ██║██║  ██║   ██║   
#  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝    ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   
#                                                                                   
#endif

//P20
/*
 * oddParity - return the odd parity bit of x, that is, 
 *      when the number of 1s in the binary representation of x is even, then the return 1, otherwise return 0.
 *   Examples:oddParity(5) = 1, oddParity(7) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 56
 *   Challenge: You will get 1 extra point if you use less than or equal to 34 ops
 *   Rating: 2
 */
int oddParity(int x) {
  int result;
    x =~(x^(x >> 16));//因为如果偶数个数是偶数，那么奇数个数也是偶数；所以将所有数字同或
    x =~(x^(x >> 8));//即可得到偶数个数
    x =~(x^(x >> 4));
    x =~(x^(x >> 2));
    x =~(x^(x >> 1));
    result = (x & 1);
    return result;
}

//P21
/*
 * bitReverse - Reverse bits in an 32-bit integer
 *   Examples: bitReverse(0x80000004) = 0x20000001
 *             bitReverse(0x7FFFFFFF) = 0xFFFFFFFE
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 56
 *   Challenge: You will get 1 extra point if you use less than or equal to 34 ops
 *   Rating: 2
 */
int bitReverse(int x) {
  int tool1,mask,mask1,mask2,mask3,mask4;
  tool1 = 0xFF;
  mask = (tool1<<8)|tool1;;//0x0000FFFF
  mask1 = (tool1<<16)|tool1;//0x00FF00FF;
  mask2 = (mask1 <<4)^mask1;//0x0F0F0F0F;
  mask3 = (mask2 <<2)^mask2;//0x33333333;
  mask4 = (mask3<<1)^mask3;//0x55555555

  x = ((x >> 16) & mask) | (x<< 16);
  x = ((x >> 8) & mask1) | ((x & mask1) << 8);
  x = ((x >> 4) & mask2) | ((x & mask2) << 4);
  x = ((x >> 2) & mask3) | ((x & mask3) << 2);
  x = ((x >> 1) & mask4) | ((x & mask4) << 1);
  return x;
  }

//P22
/*
 * mod7 - calculate x mod 7 without using %.
 *   Example: mod7(99) = 1
 *            mod7(-101) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 100
 *   Challenge: You will get 2 extra point if you use less than or equal to 56 ops
 *   Rating: 2
 */
int mod7(int x) {
    int tmp1, tmp2,result,result1,mask,abs_x,tool,tool2,minus_7,judge;
    int tmp11, tmp12, tmp21, tmp22, tmp31, tmp32, tmp41, tmp42, tmp51, tmp52;
    int tmp61, tmp62, tmp71, tmp72, tmp81, tmp82, tmp91, tmp92;

    
    mask = (x >> 31);
    abs_x = ((x^(x>>31))+(mask&1));
    tool =!(x ^ (1<<31));//如果为0x8000000返回1,其余数返回0
    // 第一次迭代
    tmp1 = abs_x >> 3;
    tmp2 = abs_x & 0x7;
    tmp1 = tmp1 + tmp2;

    // 第二次迭代
    tmp11 = tmp1 >> 3;
    tmp12 = tmp1 & 0x7;
    tmp11 = tmp11 + tmp12;

    // 第三次迭代
    tmp21 = tmp11 >> 3;
    tmp22 = tmp11 & 0x7;
    tmp21 = tmp21 + tmp22;

    // 第四次迭代
    tmp31 = tmp21 >> 3;
    tmp32 = tmp21 & 0x7;
    tmp31 = tmp31 + tmp32;

    // 第五次迭代
    tmp41 = tmp31 >> 3;
    tmp42 = tmp31 & 0x7;
    tmp41 = tmp41 + tmp42;

    // 第六次迭代
    tmp51 = tmp41 >> 3;
    tmp52 = tmp41 & 0x7;
    tmp51 = tmp51 + tmp52;

    // 第七次迭代
    tmp61 = tmp51 >> 3;
    tmp62 = tmp51 & 0x7;
    tmp61 = tmp61 + tmp62;

    // 第八次迭代
    tmp71 = tmp61 >> 3;
    tmp72 = tmp61 & 0x7;
    tmp71 = tmp71 + tmp72;

    // 第九次迭代
    tmp81 = tmp71 >> 3;
    tmp82 = tmp71 & 0x7;
    tmp81 = tmp81 + tmp82;

    // 第十次迭代
    tmp91 = tmp81 >> 3;
    tmp92 = tmp81 & 0x7;
    tmp91 = tmp91 + tmp92;  

    //最后一次循环
    result  = tmp91 >> 3;
    result1 = tmp91 & 0x7;
    result = result + result1 ;//若为0x80000000补上1

    tool2 =!(result ^ (7));//如果为7返回1,其余数返回0
    minus_7 = ~7+1;
    result = (result + (minus_7 & ((tool2<<31)>>31)));

    judge = (((mask +result )^mask)+((0x3)&((tool<<31)>>31)));
    return judge;
}