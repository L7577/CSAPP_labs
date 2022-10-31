# Bomb Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Bomb Lab*](http://csapp.cs.cmu.edu/im/labs/bomblab.tar) *[Updated 1/12/16]* ([README](http://csapp.cs.cmu.edu/3e/README-bomblab), [Writeup](http://csapp.cs.cmu.edu/3e/bomblab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/bomblab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/bomb.tar))

---

相关课程资料[ 15-213/15-513: Introduction to Computer Systems (ICS)Summer 2022](http://www.cs.cmu.edu/~213/index.html)

Machine Prog: Basics ([activity](http://www.cs.cmu.edu/~213/activities/213_lecture5.pdf), [activity tar](http://www.cs.cmu.edu/~213/activities/lec5.tar), [activity-sol](http://www.cs.cmu.edu/~213/activities/213_lecture5-sol.pdf), [pdf](http://www.cs.cmu.edu/~213/lectures/05-machine-basics.pdf), [code](http://www.cs.cmu.edu/~213/code/05-machine-basics), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=b98c6535-d003-4ea5-8568-ae2a0158cc32))

Machine Prog: Control ([activity](http://www.cs.cmu.edu/~213/activities/213_lecture6.pdf), [activity tar](http://www.cs.cmu.edu/~213/activities/lec6.tar), [activity-sol](http://www.cs.cmu.edu/~213/activities/213_lecture6-sol.pdf), [pdf](http://www.cs.cmu.edu/~213/lectures/06-machine-control.pdf), [code](http://www.cs.cmu.edu/~213/code/06-machine-control), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=d68a8cde-12e3-4fbf-9226-ae2f0148e73a))

Bomblab/GDB Bootcamp([pdf](http://www.cs.cmu.edu/~213/recitations/recitation03-bomblab.pdf), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=a7549e34-722f-4645-9a69-ad3d0141c6ff))

Machine Prog: Procedures ([activity](http://www.cs.cmu.edu/~213/activities/213_lecture7.pdf), [activity tar](http://www.cs.cmu.edu/~213/activities/lec7.tar), [activity-sol](http://www.cs.cmu.edu/~213/activities/213_lecture7-sol.pdf), [pdf](http://www.cs.cmu.edu/~213/lectures/07-machine-procedures.pdf), [code](http://www.cs.cmu.edu/~213/code/07-machine-procedures), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=bff75c99-37ad-42fa-98df-ae31014d20b0))

Machine Prog: Data ([activity](http://www.cs.cmu.edu/~213/activities/213_lecture8.pdf), [activity tar](http://www.cs.cmu.edu/~213/activities/lec8.tar), [activity-sol](http://www.cs.cmu.edu/~213/activities/213_lecture8-sol.pdf), [pdf](http://www.cs.cmu.edu/~213/lectures/08-machine-data.pdf), [code](http://www.cs.cmu.edu/~213/code/08-machine-data), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=d68c41a2-ae7b-4c18-af3d-ae36014a1f40))

---

做实验前须阅读实验说明文档[bomblab.pdf](http://csapp.cs.cmu.edu/3e/bomblab.pdf)

本章实验需要使用以下工具或命令

 **gdb**   ，可参考：https://sourceware.org/gdb/current/onlinedocs/gdb/

http://csapp.cs.cmu.edu/2e/docs/gdbnotes-x86-64.txt

或使用 `man gdb` ，`gdb -h`

**objdump** 

 使用`man objdump` ，查看使用说明

**需了解汇编语言**

[Intel 64 and IA-32 Architectures Software Developer's Manuals](http://www.intel.com/products/processor/manuals/)

README: http://csapp.cs.cmu.edu/3e/README-bomblab

使用gdb工具简单调试一下bomb程序

```sh
#使用gdb调试bomb
gdb bomb
...
#可以使用list 命令打印bomb.c的源代码
(gdb)l
...
...
#当然也可以在运行时增加参数 -tui,就可以边看源码边进行调试，可视化界面支持
gdb -tui bomb
```

可以从bomb.c程序中看出，一共有6个phase函数，也就是分别对应的6个炸弹，需要输入正确的字符串才能拆除也就是正常继续运行程序，负责就会爆炸 bomb~ !!!，当然最后还有提示，有一个隐藏的炸弹很可能被忽视了。

```sh
#也可以对bomb程序反汇编，观察一下
objdump -d bomb | less
```

开始拆弹

#### phase_1

```sh
gdb bomb
...
#在phase_1，phase_2函数入口处设置断点
(gdb)b phase_1 phase_2
(gdb)r
Starting program: /home/l/csapp/bomb/bomb 
\Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
123
#随便输入点什么，抵达断点
Breakpoint 3, 0x0000000000400ee0 in phase_1 ()
#查看当前前函数的汇编代码
(gdb) disas
Dump of assembler code for function phase_1:
=> 0x0000000000400ee0 <+0>:    sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:    mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:    callq  0x401338 <strings_not_equal>
   0x0000000000400eee <+14>:    test   %eax,%eax
   0x0000000000400ef0 <+16>:    je     0x400ef7 <phase_1+23>
   0x0000000000400ef2 <+18>:    callq  0x40143a <explode_bomb>
   0x0000000000400ef7 <+23>:    add    $0x8,%rsp
   0x0000000000400efb <+27>:    retq   
End of assembler dump.
```

观察一下phase_1函数到底在做什么，调用strings_not_equal函数判断输入的字符串和原有字符串是否相等，如果相等就跳转到phase_1+0x23 处，否则就调用爆炸函数explode_bomb。判断字符串相等，就需要找到函数中已经给出的字符串，到底存储在哪里呢？**0x402400** 这个地址很明显！查看`0x402400`处存储的到底是什么

```sh
(gdb) x /s 0x402400
0x402400:       "Border relations with Canada have never been better."
```

所以第一个关卡的答案就是 "Border relations with Canada have never been better."

退出gdb，新建一个文档，用于记录答案，后续就可以指定文件做运行时参数

```sh
touch solutions.txt
```

#### phase_2

继续gdb运行bomb程序，顺带验证phase1是否成功排除炸弹，可以看到程序继续运行，在phase_2函数处设置断点。

```sh
gdb bomb
...
#设置运行时参数
(gdb) set args ./solution.txt
(gdb) show args
Argument list to give program being debugged when it is st
arted is "./solution.txt".
(gdb) b phase_2
Breakpoint 1 at 0x400efc
(gdb) r
Starting program: /home/l/csapp/bomb/bomb ./solution.txt
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Phase 1 defused. How about the next one?

Breakpoint 1, 0x0000000000400efc in phase_2 ()

(gdb) disas
Dump of assembler code for function phase_2:
=> 0x0000000000400efc <+0>:    push   %rbp
   0x0000000000400efd <+1>:    push   %rbx
   0x0000000000400efe <+2>:    sub    $0x28,%rsp
   0x0000000000400f02 <+6>:    mov    %rsp,%rsi
   0x0000000000400f05 <+9>:    callq  0x40145c <read_six_numbers>
   0x0000000000400f0a <+14>:    cmpl   $0x1,(%rsp)
   0x0000000000400f0e <+18>:    je     0x400f30 <phase_2+52>
   0x0000000000400f10 <+20>:    callq  0x40143a <explode_bomb>
   0x0000000000400f15 <+25>:    jmp    0x400f30 <phase_2+52>
   0x0000000000400f17 <+27>:    mov    -0x4(%rbx),%eax
   0x0000000000400f1a <+30>:    add    %eax,%eax
   0x0000000000400f1c <+32>:    cmp    %eax,(%rbx)
   0x0000000000400f1e <+34>:    je     0x400f25 <phase_2+41>
   0x0000000000400f20 <+36>:    callq  0x40143a <explode_bomb>
   0x0000000000400f25 <+41>:    add    $0x4,%rbx
   0x0000000000400f29 <+45>:    cmp    %rbp,%rbx
   0x0000000000400f2c <+48>:    jne    0x400f17 <phase_2+27>
   0x0000000000400f2e <+50>:    jmp    0x400f3c <phase_2+64>
   0x0000000000400f30 <+52>:    lea    0x4(%rsp),%rbx
   0x0000000000400f35 <+57>:    lea    0x18(%rsp),%rbp
   0x0000000000400f3a <+62>:    jmp    0x400f17 <phase_2+27>
   0x0000000000400f3c <+64>:    add    $0x28,%rsp
   0x0000000000400f40 <+68>:    pop    %rbx
   0x0000000000400f41 <+69>:    pop    %rbp
   0x0000000000400f42 <+70>:    retq   
End of assembler dump.
```

查看phase_2函数对应的汇编代码，可以看到调用了一个函数 `read_six_numbers`，很明显这是要读取6个数字，，并且使用了sscanf函数读取

```assembly
Dump of assembler code for function read_six_numbers:
=> 0x000000000040145c <+0>:    sub    $0x18,%rsp   #开辟栈空间
   0x0000000000401460 <+4>:    mov    %rsi,%rdx
   0x0000000000401463 <+7>:    lea    0x4(%rsi),%rcx
   0x0000000000401467 <+11>:    lea    0x14(%rsi),%rax
   0x000000000040146b <+15>:    mov    %rax,0x8(%rsp)
   0x0000000000401470 <+20>:    lea    0x10(%rsi),%rax
   0x0000000000401474 <+24>:    mov    %rax,(%rsp)
   0x0000000000401478 <+28>:    lea    0xc(%rsi),%r9
   0x000000000040147c <+32>:    lea    0x8(%rsi),%r8
   0x0000000000401480 <+36>:    mov    $0x4025c3,%esi
   0x0000000000401485 <+41>:    mov    $0x0,%eax
   0x000000000040148a <+46>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x000000000040148f <+51>:    cmp    $0x5,%eax
   0x0000000000401492 <+54>:    jg     0x401499 <read_six_numbers+61>
   0x0000000000401494 <+56>:    callq  0x40143a <explode_bomb>
   0x0000000000401499 <+61>:    add    $0x18,%rsp #回收栈空间
   0x000000000040149d <+65>:    retq   
End of assembler dump.
```

正常的执行顺序，不走进雷区。

```assembly
   0x0000000000400f0a <+14>:    cmpl   $0x1,(%rsp)   #计算  栈顶值-1的结果
   0x0000000000400f0e <+18>:    je     0x400f30 <phase_2+52> #相等  跳转到 +52 
#  0x0000000000400f10 <+20>:    callq  0x40143a <explode_bomb> #栈顶不为1，爆炸
   0x0000000000400f15 <+25>:    jmp    0x400f30 <phase_2+52> #跳转到 +52处


   0x0000000000400f30 <+52>:    lea    0x4(%rsp),%rbx   # 第二个值给rbx寄存器
   0x0000000000400f35 <+57>:    lea    0x18(%rsp),%rbp  # 最后一个数值给rbp寄存器
   0x0000000000400f3a <+62>:    jmp    0x400f17 <phase_2+27> #跳转到 +27

   0x0000000000400f17 <+27>:    mov    -0x4(%rbx),%eax  # %eax=-0x4(%rbx) 第一个数给eax
   0x0000000000400f1a <+30>:    add    %eax,%eax   #%eax自加
   0x0000000000400f1c <+32>:    cmp    %eax,(%rbx) #比较 第一个数的2倍是否与下一个数相等 
   0x0000000000400f1e <+34>:    je     0x400f25 <phase_2+41>  #相等则跳转到 +41
#  0x0000000000400f20 <+36>:    callq  0x40143a <explode_bomb> #不相等，爆炸

   0x0000000000400f25 <+41>:    add    $0x4,%rbx #取下一个数值
   0x0000000000400f29 <+45>:    cmp    %rbp,%rbx #判断是否是最后一个数
   0x0000000000400f2c <+48>:    jne    0x400f17 <phase_2+27> #不是就跳转到 +27

   0x0000000000400f2e <+50>:    jmp    0x400f3c <phase_2+64>

   0x0000000000400f3c <+64>:    add    $0x28,%rsp
   0x0000000000400f40 <+68>:    pop    %rbx
   0x0000000000400f41 <+69>:    pop    %rbp
```

*上面注释中第一个数和第二个数是相对关系，类比成当前一个数与下一个数*

分析发现第一个数一定为1，且下一个数是前一个数的2倍，所以这6个数为: `1 2 4 8 16 32`

继续拆弹～

#### phase_3

```assembly
#phase_3的汇编代码
Dump of assembler code for function phase_3:
   0x0000000000400f43 <+0>:    sub    $0x18,%rsp
   0x0000000000400f47 <+4>:    lea    0xc(%rsp),%rcx
   0x0000000000400f4c <+9>:    lea    0x8(%rsp),%rdx
   0x0000000000400f51 <+14>:    mov    $0x4025cf,%esi
   0x0000000000400f56 <+19>:    mov    $0x0,%eax
   0x0000000000400f5b <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000400f60 <+29>:    cmp    $0x1,%eax
   0x0000000000400f63 <+32>:    jg     0x400f6a <phase_3+39>
   0x0000000000400f65 <+34>:    callq  0x40143a <explode_bomb>
   0x0000000000400f6a <+39>:    cmpl   $0x7,0x8(%rsp)
   0x0000000000400f6f <+44>:    ja     0x400fad <phase_3+106>
   0x0000000000400f71 <+46>:    mov    0x8(%rsp),%eax
   0x0000000000400f75 <+50>:    jmpq   *0x402470(,%rax,8)
   0x0000000000400f7c <+57>:    mov    $0xcf,%eax
   0x0000000000400f81 <+62>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f83 <+64>:    mov    $0x2c3,%eax
   0x0000000000400f88 <+69>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f8a <+71>:    mov    $0x100,%eax
   0x0000000000400f8f <+76>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f91 <+78>:    mov    $0x185,%eax
   0x0000000000400f96 <+83>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f98 <+85>:    mov    $0xce,%eax
   0x0000000000400f9d <+90>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f9f <+92>:    mov    $0x2aa,%eax
   0x0000000000400fa4 <+97>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fa6 <+99>:    mov    $0x147,%eax
   0x0000000000400fab <+104>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fad <+106>:    callq  0x40143a <explode_bomb>
   0x0000000000400fb2 <+111>:    mov    $0x0,%eax
   0x0000000000400fb7 <+116>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fb9 <+118>:    mov    $0x137,%eax
   0x0000000000400fbe <+123>:    cmp    0xc(%rsp),%eax
   0x0000000000400fc2 <+127>:    je     0x400fc9 <phase_3+134>
   0x0000000000400fc4 <+129>:    callq  0x40143a <explode_bomb>
   0x0000000000400fc9 <+134>:    add    $0x18,%rsp
   0x0000000000400fcd <+138>:    retq   
End of assembler dump.
```

关键指令如下

```assembly
   0x0000000000400f51 <+14>:    mov    $0x4025cf,%esi
   0x0000000000400f5b <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000400f60 <+29>:    cmp    $0x1,%eax #计算eax -1
   0x0000000000400f63 <+32>:    jg     0x400f6a <phase_3+39> #大于1则跳转
#可以看出eax中存的是sscanf的返回值
(gdb) x/s 0x4025cf  #打印值并观察
0x4025cf:    "%d %d"
```

可以发现这一关卡是需要输入两个整数。

```assembly
   0x0000000000400f6a <+39>:    cmpl   $0x7,0x8(%rsp)  #计算  第1个数-7
   0x0000000000400f6f <+44>:    ja     0x400fad <phase_3+106>  #若是第一个数大于7就爆炸
   0x0000000000400f71 <+46>:    mov    0x8(%rsp),%eax
   0x0000000000400f75 <+50>:    jmpq   *0x402470(,%rax,8) #以0x402470为起始地址，%rax为偏移跳转
   0x0000000000400f7c <+57>:    mov    $0xcf,%eax
   0x0000000000400f81 <+62>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f83 <+64>:    mov    $0x2c3,%eax
   0x0000000000400f88 <+69>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f8a <+71>:    mov    $0x100,%eax
   0x0000000000400f8f <+76>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f91 <+78>:    mov    $0x185,%eax
   0x0000000000400f96 <+83>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f98 <+85>:    mov    $0xce,%eax
   0x0000000000400f9d <+90>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400f9f <+92>:    mov    $0x2aa,%eax
   0x0000000000400fa4 <+97>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fa6 <+99>:    mov    $0x147,%eax
   0x0000000000400fab <+104>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fad <+106>:    callq  0x40143a <explode_bomb>
   0x0000000000400fb2 <+111>:    mov    $0x0,%eax
   0x0000000000400fb7 <+116>:    jmp    0x400fbe <phase_3+123>
   0x0000000000400fb9 <+118>:    mov    $0x137,%eax
   0x0000000000400fbe <+123>:    cmp    0xc(%rsp),%eax
   0x0000000000400fc2 <+127>:    je     0x400fc9 <phase_3+134>
   0x0000000000400fc4 <+129>:    callq  0x40143a <explode_bomb>
```

若是不会算jump间接跳转地址，可以通过打断点，多次调试，比如设置第一参数为0是，观察会跳转到`mov    $0xcf,%eax`，0xcf的值为207，所以一个正确答案为 `0 207`  ,再就是设置第一个参数为1时，会跳转到`mov    $0x137,%eax`，0x137的十进制数为311,又一个正确答案为 `1 311`

如何计算间接跳转地址：参考[What is the jmpq command doing in this example](https://stackoverflow.com/questions/26543029/what-is-the-jmpq-command-doing-in-this-example)

```assembly
jmpq   *0x402470(,%rax,8)
# 8* %rax + *0x402470
```

可在gdb调试中，使用x/8gx 0x402470打印出地址表。

```sh
(gdb) x/8gx 0x402470
0x402470:    0x0000000000400f7c    0x0000000000400fb9
0x402480:    0x0000000000400f83    0x0000000000400f8a
0x402490:    0x0000000000400f91    0x0000000000400f98
0x4024a0:    0x0000000000400f9f    0x0000000000400fa6

# 8个地址，也就是对应通过 0 - 7 八个数字，作为偏移量，计算后的跳转地址
```

如果是第一个数是0，就跳转到 `0x400f7c`，通过计算间接跳转地址`8 * 0 + *0x402470 = *0x402470`可知。其他情况亦可照此计算。注意是带`*`运算符的，在`0x402470`处存着`0x400f7c`。

*当然这个地址表是早就定好的！*

#### phase_4

继续第四关

```assembly
Dump of assembler code for function phase_4:
   0x000000000040100c <+0>:    sub    $0x18,%rsp
   0x0000000000401010 <+4>:    lea    0xc(%rsp),%rcx
   0x0000000000401015 <+9>:    lea    0x8(%rsp),%rdx
   0x000000000040101a <+14>:    mov    $0x4025cf,%esi

(gdb) x/s 0x4025cf
0x4025cf:    "%d %d"

   0x000000000040101f <+19>:    mov    $0x0,%eax
   0x0000000000401024 <+24>:    callq  0x400bf0 <__isoc99_sscanf@plt>
   0x0000000000401029 <+29>:    cmp    $0x2,%eax  #判断是否读入两个数字
   0x000000000040102c <+32>:    jne    0x401035 <phase_4+41> #不是两个数就爆炸
   0x000000000040102e <+34>:    cmpl   $0xe,0x8(%rsp) #判断第一个数是否小于14
   0x0000000000401033 <+39>:    jbe    0x40103a <phase_4+46> #跳转到+46
   0x0000000000401035 <+41>:    callq  0x40143a <explode_bomb> #不小于14就爆炸
   0x000000000040103a <+46>:    mov    $0xe,%edx #%edx = 14
   0x000000000040103f <+51>:    mov    $0x0,%esi #%esi = 0
   0x0000000000401044 <+56>:    mov    0x8(%rsp),%edi #%edi = 第一个数
   0x0000000000401048 <+60>:    callq  0x400fce <func4> #调用函数 func4 
   0x000000000040104d <+65>:    test   %eax,%eax   #测试返回值是否为0
   0x000000000040104f <+67>:    jne    0x401058 <phase_4+76>
   0x0000000000401051 <+69>:    cmpl   $0x0,0xc(%rsp)  #判断第二个数是否为0
   0x0000000000401056 <+74>:    je     0x40105d <phase_4+81>
   0x0000000000401058 <+76>:    callq  0x40143a <explode_bomb> #不是0就爆炸
   0x000000000040105d <+81>:    add    $0x18,%rsp
   0x0000000000401061 <+85>:    retq   
```

func4函数对应的汇编代码

```assembly
Dump of assembler code for function func4:
   0x0000000000400fce <+0>:    sub    $0x8,%rsp  # 栈顶 - 8 也就是取第一个数
   0x0000000000400fd2 <+4>:    mov    %edx,%eax # %eax = %edx   
   0x0000000000400fd4 <+6>:    sub    %esi,%eax # %eax = %eax - esi
   0x0000000000400fd6 <+8>:    mov    %eax,%ecx # %ecx = %eax
   0x0000000000400fd8 <+10>:    shr    $0x1f,%ecx # 逻辑右移31位
   0x0000000000400fdb <+13>:    add    %ecx,%eax #　%ecx = %ecx + %eax
   0x0000000000400fdd <+15>:    sar    %eax   # 算术右移一位，相当于除2
   0x0000000000400fdf <+17>:    lea    (%rax,%rsi,1),%ecx # %ecx= %rsi+%rax
   0x0000000000400fe2 <+20>:    cmp    %edi,%ecx  #比较 %edi 和 %ecx
   0x0000000000400fe4 <+22>:    jle    0x400ff2 <func4+36> # 相等就跳转到 +36
   0x0000000000400fe6 <+24>:    lea    -0x1(%rcx),%edx  # %edx = %rcx - 1
   0x0000000000400fe9 <+27>:    callq  0x400fce <func4>
   0x0000000000400fee <+32>:    add    %eax,%eax  # %eax自加
   0x0000000000400ff0 <+34>:    jmp    0x401007 <func4+57>
   0x0000000000400ff2 <+36>:    mov    $0x0,%eax # %eax = 0
   0x0000000000400ff7 <+41>:    cmp    %edi,%ecx # 比较大小 判断第一个数是否为0
   0x0000000000400ff9 <+43>:    jge    0x401007 <func4+57> # 大于等于0 则跳转到 +57
   0x0000000000400ffb <+45>:    lea    0x1(%rcx),%esi # %esi = %rcx + 1
   0x0000000000400ffe <+48>:    callq  0x400fce <func4>
   0x0000000000401003 <+53>:    lea    0x1(%rax,%rax,1),%eax # %eax = 1 + %rax + %rax * 1
   0x0000000000401007 <+57>:    add    $0x8,%rsp 
   0x000000000040100b <+61>:    retq   #返回
End of assembler dump.
```

可以看到 func4函数，自己调用了自己，就是个递归函数，边界条件是什么呢，就是 `cmp    %edi,%ecx`就是第一个数,存储在`%edi`，等于`%ecx`的值，才可以退出递归，并且第一个数要大于等于0。

在 `0x400fe2`处打断点，输入参数为 `0 0`

```sh
(gdb)b *0x400fe2
# 第一次运行到判断语句，cmp %edi % ecx查看寄存器状态

(gdb) i r
rax            0x7                 7
rbx            0x7fffffffdde8      140737488346600
rcx            0x7                 7
rdx            0xe                 14
rsi            0x0                 0
rdi            0x0                 0
rbp            0x0                 0x0
rsp            0x7fffffffdcc0      0x7fffffffdcc0
r8             0xffffffff          4294967295
r9             0x0                 0
r10            0x7ffff7f5dac0      140737353472704
r11            0x0                 0
r12            0x400c90            4197520
r13            0x7fffffffdde0      140737488346592
r14            0x0                 0
r15            0x0                 0
rip            0x400fe2            0x400fe2 <func4+20>
eflags         0x202               [ IF ]
cs             0x33                51
ss             0x2b                43
ds             0x0                 0
es             0x0                 0
fs             0x0                 0
gs             0x0                 0
(gdb) p $edi
$5 = 0
(gdb) p $ecx
$6 = 7

# 可以看到%edi = 0,这是输入的第一个数，
#　第一次判断时，%ecx = 7  
# 不满足递归退出条件，会继续调用func4函数

#继续运行
(gdb) c
Continuing.

Breakpoint 5, 0x0000000000400fe2 in func4 ()
(gdb) i r
rax            0x3    3
rbx            0x7fffffffdf58    140737488346968
rcx            0x3    3
rdx            0x6    6
rsi            0x0    0
rdi            0x1    1
rbp            0x402210    0x402210 <__libc_csu_init>
rsp            0x7fffffffde30    0x7fffffffde30
r8             0x0    0
r9             0x0    0
r10            0x0    0
r11            0x7ffff7b846e0    140737349437152
r12            0x400c90    4197520
r13            0x7fffffffdf50    140737488346960
r14            0x0    0
r15            0x0    0
rip            0x400fe2    0x400fe2 <func4+20>
eflags         0x206    [ PF IF ]
cs             0x33    51
ss             0x2b    43
ds             0x0    0
es             0x0    0
fs             0x0    0
gs             0x0    0

# 第二次判断时，%ecx = 3
(gdb) p $ecx
$40 = 3

# 还是不满足退出条件，继续运行
(gdb) c
Continuing.

Breakpoint 5, 0x0000000000400fe2 in func4 ()
(gdb) i r
rax            0x1    1
rbx            0x7fffffffdf58    140737488346968
rcx            0x1    1
rdx            0x2    2
rsi            0x0    0
rdi            0x1    1
rbp            0x402210    0x402210 <__libc_csu_init>
rsp            0x7fffffffde20    0x7fffffffde20
r8             0x0    0
r9             0x0    0
r10            0x0    0
r11            0x7ffff7b846e0    140737349437152
r12            0x400c90    4197520
r13            0x7fffffffdf50    140737488346960
r14            0x0    0
r15            0x0    0
rip            0x400fe2    0x400fe2 <func4+20>
eflags         0x202    [ IF ]
cs             0x33    51
ss             0x2b    43
ds             0x0    0
es             0x0    0
fs             0x0    0
gs             0x0    0
(gdb) p $ecx
$41 = 1

(gdb) i r
rax            0x0                 0
rbx            0x7fffffffdde8      140737488346600
rcx            0x0                 0
rdx            0x0                 0
rsi            0x0                 0
rdi            0x0                 0
rbp            0x0                 0x0
rsp            0x7fffffffdc90      0x7fffffffdc90
r8             0xffffffff          4294967295
r9             0x0                 0
r10            0x7ffff7f5dac0      140737353472704
r11            0x0                 0
r12            0x400c90            4197520
r13            0x7fffffffdde0      140737488346592
r14            0x0                 0
r15            0x0                 0
rip            0x400fe2            0x400fe2 <func4+20>
eflags         0x246               [ PF ZF IF ]
cs             0x33                51
ss             0x2b                43
ds             0x0                 0
es             0x0                 0
fs             0x0                 0
gs             0x0                 0
(gdb) p $edi
$11 = 0
(gdb) p $ecx
$12 = 0

# 此时ecx等于0 ，与输入的第一个数相等，满足递归条件，退出
```

多次打断点调试发现`%ecx`中分别存入了 7 3 1 0，所以第一个数可以是 0 1 3 7

第二个数  `cmpl   $0x0,0xc(%rsp)`,也只能为0，才不会爆炸

所以答案可以为  `0 0`  、`1 0` 、`3 0`、 `7 0` 

#### phase_5

继续拆第五个炸弹

```assembly
Dump of assembler code for function phase_5:
   0x0000000000401062 <+0>:    push   %rbx
   0x0000000000401063 <+1>:    sub    $0x20,%rsp
   0x0000000000401067 <+5>:    mov    %rdi,%rbx
   0x000000000040106a <+8>:    mov    %fs:0x28,%rax
   0x0000000000401073 <+17>:    mov    %rax,0x18(%rsp)
   0x0000000000401078 <+22>:    xor    %eax,%eax
   0x000000000040107a <+24>:    callq  0x40131b <string_length># 读入一串字符
   0x000000000040107f <+29>:    cmp    $0x6,%eax            # 判断长度是否为6
   0x0000000000401082 <+32>:    je     0x4010d2 <phase_5+112> # 跳转到　+112
   0x0000000000401084 <+34>:    callq  0x40143a <explode_bomb>
   0x0000000000401089 <+39>:    jmp    0x4010d2 <phase_5+112> 
   0x000000000040108b <+41>:    movzbl (%rbx,%rax,1),%ecx # %ecx = %rbx + %rax*1
   0x000000000040108f <+45>:    mov    %cl,(%rsp) # (%rsp) = %cl
   0x0000000000401092 <+48>:    mov    (%rsp),%rdx # %rdx = (%rsp)
   0x0000000000401096 <+52>:    and    $0xf,%edx  # &运算
   0x0000000000401099 <+55>:    movzbl 0x4024b0(%rdx),%edx 　#地址映射
   0x00000000004010a0 <+62>:    mov    %dl,0x10(%rsp,%rax,1) #存入栈中
   0x00000000004010a4 <+66>:    add    $0x1,%rax # %rax + 1
   0x00000000004010a8 <+70>:    cmp    $0x6,%rax # 比较等于6
   0x00000000004010ac <+74>:    jne    0x40108b <phase_5+41> # 小于则 跳转到 +41
   0x00000000004010ae <+76>:    movb   $0x0,0x16(%rsp) 
   0x00000000004010b3 <+81>:    mov    $0x40245e,%esi #esi = $0x40245e 即是 "flyers"
   0x00000000004010b8 <+86>:    lea    0x10(%rsp),%rdi
   0x00000000004010bd <+91>:    callq  0x401338 <strings_not_equal>
   0x00000000004010c2 <+96>:    test   %eax,%eax
   0x00000000004010c4 <+98>:    je     0x4010d9 <phase_5+119>
   0x00000000004010c6 <+100>:    callq  0x40143a <explode_bomb>
   0x00000000004010cb <+105>:    nopl   0x0(%rax,%rax,1)
   0x00000000004010d0 <+110>:    jmp    0x4010d9 <phase_5+119>
   0x00000000004010d2 <+112>:    mov    $0x0,%eax # %eax=0
   0x00000000004010d7 <+117>:    jmp    0x40108b <phase_5+41> #　跳转到　+41
   0x00000000004010d9 <+119>:    mov    0x18(%rsp),%rax
   0x00000000004010de <+124>:    xor    %fs:0x28,%rax
   0x00000000004010e7 <+133>:    je     0x4010ee <phase_5+140>
   0x00000000004010e9 <+135>:    callq  0x400b30 <__stack_chk_fail@plt>
   0x00000000004010ee <+140>:    add    $0x20,%rsp
   0x00000000004010f2 <+144>:    pop    %rbx
   0x00000000004010f3 <+145>:    retq   
End of assembler dump.
```

可以看到phase_5 先是读入了一串长度为6的字符串，然后进入循环中，依次用`0xf`对每个字符串做& 运算，并将结果保存到%rdx，然后将结果映射到一个地址上，字符与`0x40245e`处相同。

```sh
(gdb) x/s 0x40245e
0x40245e:    "flyers"

(gdb) x/16ubc 0x4024b0
0x4024b0 <array.3449>:    109 'm'    97 'a'    100 'd'    117 'u'    105 'i'    101 'e'    114 'r'    115 's'
0x4024b8 <array.3449+8>:    110 'n'    102 'f'    111 'o'    116 't'    118 'v'    98 'b'    121 'y'    108 'l'
```

也就是说，我们输入的字符串，在于`0xf` `&`运算后，再经过地址转换，取出的字符串，应该是"`flyers`"。

那么输入字符串的第一个字符，与 0xf 与运算之后的结果就是9，即是取到0x4024b0处开始存储的第9个字符'f'，所以第一个字符可以是9的倍数，可查对应ASCII码表。

同理第二个字符，其ASCII码值应该是f（对应字符'l'）的倍数。第三个字符可以比第二个字符值小于1。

第四五六个字符是连续的，对应ASCII 码值 5 6 7 的倍数皆可以。

一个可行的答案是`)/.%&'`

#### phase_6

继续拆除第六个炸弹,这一关的代码是比较长的。

```assembly
Dump of assembler code for function phase_6:
   0x00000000004010f4 <+0>:    push   %r14
   0x00000000004010f6 <+2>:    push   %r13
   0x00000000004010f8 <+4>:    push   %r12
   0x00000000004010fa <+6>:    push   %rbp
   0x00000000004010fb <+7>:    push   %rbx
   0x00000000004010fc <+8>:    sub    $0x50,%rsp
   0x0000000000401100 <+12>:    mov    %rsp,%r13
   0x0000000000401103 <+15>:    mov    %rsp,%rsi
   0x0000000000401106 <+18>:    callq  0x40145c <read_six_numbers># 读入6个数字

# 开启循环
   0x000000000040110b <+23>:    mov    %rsp,%r14  # %r14 = %rsp
   0x000000000040110e <+26>:    mov    $0x0,%r12d # %r12d = 0
   0x0000000000401114 <+32>:    mov    %r13,%rbp  # %rbp = %r13
   0x0000000000401117 <+35>:    mov    0x0(%r13),%eax
   0x000000000040111b <+39>:    sub    $0x1,%eax  # %eax - 1
   0x000000000040111e <+42>:    cmp    $0x5,%eax  # 比较是否等于5
   0x0000000000401121 <+45>:    jbe    0x401128 <phase_6+52> # 相等就跳转到 +52
   0x0000000000401123 <+47>:    callq  0x40143a <explode_bomb>
   0x0000000000401128 <+52>:    add    $0x1,%r12d  #　%r12d+1
   0x000000000040112c <+56>:    cmp    $0x6,%r12d  # 比较是否等于6，6次循环，每个数都要小于等于6
   0x0000000000401130 <+60>:    je     0x401153 <phase_6+95> #相等就跳转 +95，进入下一个循环

   0x0000000000401132 <+62>:    mov    %r12d,%ebx # %ebx = %r12d
   0x0000000000401135 <+65>:    movslq %ebx,%rax  # %rax = %ebx
   0x0000000000401138 <+68>:    mov    (%rsp,%rax,4),%eax #取rsp开头的第rax个数，放到eax中
   0x000000000040113b <+71>:    cmp    %eax,0x0(%rbp) # 比较eax与栈顶的值
   0x000000000040113e <+74>:    jne    0x401145 <phase_6+81>  # 不相等就跳转到 +81
   0x0000000000401140 <+76>:    callq  0x40143a <explode_bomb>
   0x0000000000401145 <+81>:    add    $0x1,%ebx #　%ebx+1
   0x0000000000401148 <+84>:    cmp    $0x5,%ebx # 比较
   0x000000000040114b <+87>:    jle    0x401135 <phase_6+65> # 小于等于就跳转到 +65
   0x000000000040114d <+89>:    add    $0x4,%r13  # %r13 + 4 # 不相等就加4
   0x0000000000401151 <+93>:    jmp    0x401114 <phase_6+32> # 跳转到+32

# 可以看出6个数字都是正整数，存在栈中  从%rsp 到 %rsp+0x14


# 又是一个循环
   0x0000000000401153 <+95>:    lea    0x18(%rsp),%rsi # %rsi = ((%rsp)+0x18)
   0x0000000000401158 <+100>:    mov    %r14,%rax
   0x000000000040115b <+103>:    mov    $0x7,%ecx # %ecx = 7
   0x0000000000401160 <+108>:    mov    %ecx,%edx # %edx = %ecx
   0x0000000000401162 <+110>:    sub    (%rax),%edx  # %edx-(%rax)
   0x0000000000401164 <+112>:    mov    %edx,(%rax)  #(%rax)=%edx
   0x0000000000401166 <+114>:    add    $0x4,%rax # %rax + 4
   0x000000000040116a <+118>:    cmp    %rsi,%rax # 比较
   0x000000000040116d <+121>:    jne    0x401160 <phase_6+108> # 不相等就跳转+108 继续循环

# 这个循环是，用这6个数做减数，被减数是7

   # 又一个循环
   0x000000000040116f <+123>:    mov    $0x0,%esi　# %esi = 0
   0x0000000000401174 <+128>:    jmp    0x401197 <phase_6+163>
   0x0000000000401176 <+130>:    mov    0x8(%rdx),%rdx # 取到下一个数字，指向下一个地址
   0x000000000040117a <+134>:    add    $0x1,%eax # %eax + 1
   0x000000000040117d <+137>:    cmp    %ecx,%eax # 比较 ecx是否为eax+1
   0x000000000040117f <+139>:    jne    0x401176 <phase_6+130>　# 不相等就跳转到 +130
   0x0000000000401181 <+141>:    jmp    0x401188 <phase_6+148>  # 跳转到 +148
   0x0000000000401183 <+143>:    mov    $0x6032d0,%edx  #%edx = $0x6032d0
   0x0000000000401188 <+148>:    mov    %rdx,0x20(%rsp,%rsi,2) # 将%rdx的值存到栈中
   0x000000000040118d <+153>:    add    $0x4,%rsi  # %rsi + 4
   0x0000000000401191 <+157>:    cmp    $0x18,%rsi # 比较 %rsi 与 24
   0x0000000000401195 <+161>:    je     0x4011ab <phase_6+183>
   0x0000000000401197 <+163>:    mov    (%rsp,%rsi,1),%ecx # ecx = %rsp + %rsi*1
   0x000000000040119a <+166>:    cmp    $0x1,%ecx  # 比较%ecx是否为1
   0x000000000040119d <+169>:    jle    0x401183 <phase_6+143> # 若是相等则跳转到 +143
   0x000000000040119f <+171>:    mov    $0x1,%eax  # %eax = 1
   0x00000000004011a4 <+176>:    mov    $0x6032d0,%edx # %edx = $0x6032d0
   0x00000000004011a9 <+181>:    jmp    0x401176 <phase_6+130>

# 判断栈中的数字是否为1，若是就将0x6032d0放在栈中，若不是，则循环，将该地址再次进行计算后的地址存在栈中，
继而可得到五个地址，0x6032e0  0x6032f0 0x603300 0x603310 0x603320 分别存放到 栈 %rsp+0x20 到 %rsp+0x48

   0x00000000004011ab <+183>:    mov    0x20(%rsp),%rbx
   0x00000000004011b0 <+188>:    lea    0x28(%rsp),%rax
   0x00000000004011b5 <+193>:    lea    0x50(%rsp),%rsi
   0x00000000004011ba <+198>:    mov    %rbx,%rcx
   0x00000000004011bd <+201>:    mov    (%rax),%rdx
   0x00000000004011c0 <+204>:    mov    %rdx,0x8(%rcx)
   0x00000000004011c4 <+208>:    add    $0x8,%rax
   0x00000000004011c8 <+212>:    cmp    %rsi,%rax
   0x00000000004011cb <+215>:    je     0x4011d2 <phase_6+222>
   0x00000000004011cd <+217>:    mov    %rdx,%rcx
   0x00000000004011d0 <+220>:    jmp    0x4011bd <phase_6+201>
   0x00000000004011d2 <+222>:    movq   $0x0,0x8(%rdx) 

# 栈顶的值为地址 +8 寻址到的值为栈中第二个值，依次类推，栈中第六个值为地址+8后寻址到的是0

   # 循环判断
   0x00000000004011da <+230>:    mov    $0x5,%ebp # ebx = 5
   0x00000000004011df <+235>:    mov    0x8(%rbx),%rax
   0x00000000004011e3 <+239>:    mov    (%rax),%eax
   0x00000000004011e5 <+241>:    cmp    %eax,(%rbx) # (%rbx) - eax
   0x00000000004011e7 <+243>:    jge    0x4011ee <phase_6+250> # 小于等于时跳到 +250
   0x00000000004011e9 <+245>:    callq  0x40143a <explode_bomb>
   0x00000000004011ee <+250>:    mov    0x8(%rbx),%rbx # %rbx = ((%rbx)+8)
   0x00000000004011f2 <+254>:    sub    $0x1,%ebp # %ebp - 1
   0x00000000004011f5 <+257>:    jne    0x4011df <phase_6+235> 

# 对前面一段循环操作结果的判断，做5次训话，%rbx存着初始栈顶的值，对这个值+8后寻址得到的值，要小于这个值自身作为地址寻到的值，否则爆炸。

   0x00000000004011f7 <+259>:    add    $0x50,%rsp
   0x00000000004011fb <+263>:    pop    %rbx
   0x00000000004011fc <+264>:    pop    %rbp
   0x00000000004011fd <+265>:    pop    %r12
   0x00000000004011ff <+267>:    pop    %r13
   0x0000000000401201 <+269>:    pop    %r14
   0x0000000000401203 <+271>:    retq   
End of assembler dump.
```

待更新
