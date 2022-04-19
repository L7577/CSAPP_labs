# Data Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Data Lab*](http://csapp.cs.cmu.edu/im/labs/datalab.tar) ***[Updated 12/16/19]*** ([README](http://csapp.cs.cmu.edu/3e/README-datalab), [Writeup](http://csapp.cs.cmu.edu/3e/datalab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/datalab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/datalab-handout.tar)) 



参考：

- [Exely - CSAPP-Labs - Data lab 笔记](-https://github.com/Exely/CSAPP-Labs/blob/master/notes/datalab.md)

- [I Data Lab - 位操作，数据表示](http://wdxtub.com/csapp/thick-csapp-lab-1/2016/04/16/)
- [CSAPP 之 Data Lab](https://scarboroughcoral.github.io/csapp-lab-1.html)



开始实验前需仔细阅读，实验说明文档，[datalab.pdf](http://csapp.cs.cmu.edu/3e/datalab.pdf)

实现以下13个函数，需修改`bits.c`文件，并使用测试程序(`make`编译生成`btest`  及课程提供的`dlc` 、`driver.pl` )验证正确性并查看得分情况，详情须阅读[README](http://csapp.cs.cmu.edu/3e/README-datalab)

注意阅读`bits.c`文件中的注意事项

代码风格示例：

```c
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
```

> Each "Expr" is an expression using ONLY the following:
>  1. Integer constants 0 through 255 (0xFF), inclusive. You are
>      not allowed to use big constants such as 0xffffffff.
>  2. Function arguments and local variables (no global variables).
>  3. Unary integer operations ! ~
>  4. Binary integer operations & ^ | + << >>

| Name               | Description                 | Rating | Max ops |
| ------------------ | --------------------------- | ------ | ------- |
| bitXor(x,y)        | 仅使用`~`、`&`              | 1      | 14      |
| tmin()             | 返回最小补码                | 1      | 4       |
| isTmax(x)          | 判断是否为补码最大值        | 1      | 10      |
| allOddBits(x)      | 判断补码所有奇数位是否为1   | 2      | 12      |
| negate(x)          | 不使用符号`-`实验`-x`       | 2      | 5       |
| isAsciDigit(x)     | 判断x是否是ASCII码          | 3      | 15      |
| conditional        | 条件判断，x?y:z             | 3      | 16      |
| isLessOrEqual(x,y) | x<=y                        | 3      | 24      |
| logicalNeg(x)      | 不用`!`计算`!x`             | 4      | 12      |
| howMangBits(x)     | 计算表达x时所需补码最小位数 | 4      | 90      |
| floatScale2(uf)    | 计算2.0*uf                  | 4      | 30      |
| floatFloat2Int(uf) | 计算(int )f                 | 4      | 30      |
| floatPower2(x)     | 计算2.0的x次方              | 4      | 30      |



## 解答

#### bitXor(x,y)

使用按位**取反**运算 和按位**与**运算 实现 按位**异或** 运算，有两种简单的方式，一是先将同时为 0和 同时为1的情况相与，再取反，，二是将不同时为1 和 不同时为0 的情况相与。

`~(~x&~y)&~(x&y)` 7 ops

`~(~(~x&y)&~(x&~y))` 8 ops

```c
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  return ~(~(~x&y)&~(x&~y));
  //  ~(~x&~y)&~(x&y)
}
```



#### tmin()

使用位运算，返回补码最小值，，需注意tmin==~tmax，，int是32位，补码最小值就是符号位（最高位）为1，其余全是0，即是`10000000 00000000 00000000 00000000`，可将0x1 末尾的1左移31位到最高位即可。

```c
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 0x1<<31;
}
```





#### isTmax(x)

判断是否是补码最大值（int型），即`01111111 11111111 11111111 11111111`，题目要求不能使用移位运算符，所以上一个tmin()的简单算法不能参考使用了。

如果是补码最大值则返回1，但需注意不能使用if判断，所以只能返回一个表达式的值；那就只剩下一种方式了，假定是补码最大值，将其转换成全0，再用逻辑非`!`运算符取反，返回其布尔值1，若不是补码最大值，则返回布尔值就是0了。

```c
// 一种转全0的方式
f = x + 1;//计算x+1的值
x = x + f;//将x+1的值再加上原来的值，若是补码最大值，则得到全1
x = ~x;//转换为全0
```



正常情况下，`补码最大值+1` 就是 `10000000 00000000 00000000 00000000` 再加上补码最大值得到全1，也就是 `11111111 11111111 11111111 11111111`,再进行逻辑非运算，可以转换到全0；需要注意 对于 -1， 补码为 `11111111 11111111 11111111 11111111`，对其加1，得到全0`00000000 00000000 00000000 00000000`，再与-1相加也是全1，有所冲突了，如何排除呢，需要上面的基础上再做一次判断，观察-1+1，就已经是全0了，，对其逻辑非运算会得到1，但对`补码最大值+1`的逻辑非操作是0，全0+0还是全0，其他值则会得到1。再返回其逻辑非运算后的值，是补码最大值就为1，不是则为0，这下也就可以排除-1这种情况了。

```c
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  int f = x + 1;
  x = x + f;
  x = ~x;
  f = !f;    
  x = x + f;  
  return !x;
}

```





allOddBits(x)

判断所有奇数位是否都为1，注意不能使用大于0xff的数值，考虑使用0xaa，需要将前24位移动后八位进行判断，如下所示，x第1位为0。

11111111 11111111 11111111 11111101  x

00000000 11111111 11111111 11111111  x>>8

00000000 00000000 11111111 11111111  x>>16

00000000 00000000 00000000 11111111  x>>24

用0xaa与上x，判断第八位，再用0xaa与上x>>8  ，依次类推，每次判断八位，

```c
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
   int mask = 0xAA+(0xAA<<8); //低16位: 01010101 01010101
   mask = mask + (mask << 16); //高16位
   return !((mask&x)^mask);
}
/*
    1010 a
    1011 b
    1100 c
    1101 d
    1110 e
    1111 f
*/
```



#### negate(x)

不适用`-`运算符号，求-x的值。直接使用`~`运算符号，但需要注意，补码的定义中，正数和负数不是对称的，比如：0010(2) 取反后 1101(-3) ，需要再加1。

```c
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
```





#### isAsciiDigit(x)

要求判断输入的x是否是数字0~9的ASCII值 ，

```c
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {

    int sign = 0x1<<31;
    int up = ~(sign|0x39);
    int low = ~0x30;
    up = sign&(up+x)>>31;
    low = sign & (low + 1 + x) >> 31;
    return !(up|low);
}

```



#### conditional(x,y, z)

实现条件判断，类似于 ` x?y:z `

```c
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    x = !!x;
    x = ~x+1;
    return (x&y)|(~x&z);
}

```



#### isLessOrEqual(x, y)

实现小于或等于 `<=` 运算

```c
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int val = !!((x+~y)>>31);
    x = x >> 31;
    y = y >> 31;
    return (!!x|!y)&((!!x&!y)|(val));
}

```



#### logicalNeg(x)

实现逻辑非运算

```c
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  return ((x|(~x+1))>>31)+1;
}

```



#### howManyBits(x)



```c
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  int sign = x >> 31;
  int b16,b8,b4,b2,b1,b0;
  x = (sign&~x) | (~sign&x);
  b16 = !!(x>>16)<<4;
  x = x >> b16;
  b8 = !!(x>>8)<<3;
  x = x>>b8;
  b4 = !!(x>>4)<<2;
  x = x>>b4;
  b2 = !!(x>>2)<<1;
  x = x>>b2;
  b1 = !!(x>>1);
  x = x>>b1;
  b0 = x;

  return b16+b8+b4+b2+b1+b0+1;
}

```





Float 

#### floatScale2(uf)

计算2乘以一个浮点数

```c
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {

    int exp = (uf&0x7f800000)>>23;
    int sign = uf&(1<<31);
    if(exp==0)
        return uf<<1|sign;
    if(exp==255)
        return uf;
    exp++;
    if(exp==255)
        return 0x7f800000|sign;
    return (exp<<23)|(uf&0x807fffff);
}

```



#### floatFloat2Int(uf)

将浮点数转换为整数

```c
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int s_    = uf>>31;
  int exp_  = ((uf&0x7f800000)>>23)-127;
  int frac_ = (uf&0x007fffff)|0x00800000;
  if(!(uf&0x7fffffff))
      return 0;
  if(exp_ > 31)
      return 0x80000000;
  if(exp_ < 0)
      return 0;
  if(exp_ > 23) frac_ <<= (exp_-23);
  else frac_ >>= (23-exp_);
  if(!((frac_>>31)^s_))
      return frac_;
  else if(frac_>>31)
      return 0x80000000;
  else return ~frac_+1;
}

```



#### floatPower2(x)

计算2.0的x次幂

```c
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  int INF = 0xff<<23;
  int exp = x + 127;
  if(exp <= 0) return 0;
  if(exp >= 255) return INF;
  return exp << 23;
}

```







评测

```shell
./driver.pl

Correctness Results	Perf Results
Points	Rating	Errors	Points	Ops	Puzzle
1	1	0	2	8	bitXor
1	1	0	2	1	tmin
1	1	0	2	6	isTmax
2	2	0	2	7	allOddBits
2	2	0	2	2	negate
3	3	0	2	13	isAsciiDigit
3	3	0	2	8	conditional
3	3	0	2	17	isLessOrEqual
4	4	0	2	5	logicalNeg
4	4	0	2	36	howManyBits
4	4	0	2	14	floatScale2
4	4	0	2	21	floatFloat2Int
4	4	0	2	5	floatPower2

Score = 62/62 [36/36 Corr + 26/26 Perf] (143 total operators)

```

