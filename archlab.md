# Architecture Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Architecture Lab*](http://csapp.cs.cmu.edu/im/labs/archlab.tar) ***[Updated 10/19/16]*** ([README](http://csapp.cs.cmu.edu/3e/README-archlab), [Writeup](http://csapp.cs.cmu.edu/3e/archlab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/archlab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/archlab-handout.tar))



参考：

- [Exely-archlab笔记](https://github.com/Exely/CSAPP-Labs/blob/master/notes/archlab.md#architecture-lab-y86-64-%E7%AC%94%E8%AE%B0)
- [CSAPP : Arch Lab 解题报告](https://zhuanlan.zhihu.com/p/36793761)



做实验前需要阅读http://csapp.cs.cmu.edu/3e/README-archlab

实验说明：http://csapp.cs.cmu.edu/3e/archlab.pdf

本节实验对应原书第四章，处理器体系结构



准备工作：

解压sim.tar ,进入sim目录， make clean  ; make

可能会编译失败，需要注释掉Makefile中部分配置项

```makefile
# Comment this out if you don't have Tcl/Tk on your system

#GUIMODE=-DHAS_GUI

# Modify the following line so that gcc can find the libtcl.so and
# libtk.so libraries on your system. You may need to use the -L option
# to tell gcc which directory to look in. Comment this out if you
# don't have Tcl/Tk.

#TKLIBS=-L/usr/lib/x86_64-linux-gnu -ltk -ltcl

# Modify the following line so that gcc can find the tcl.h and tk.h
# header files on your system. Comment this out if you don't have
# Tcl/Tk.

#TKINC=-l/usr/local/include -I/usr/include/tcl8.6

```





### Part A

工作目录 sim/misc ，根据给出的example.c中的三个函数，编写三个对应的y86-64汇编程序，并完成测试。

写一个Y86-64程序 sum.ys，用来统计链表的元素个数。

C代码如下

```c
/* 
 * Architecture Lab: Part A 
 * 
 * High level specs for the functions that the students will rewrite
 * in Y86-64 assembly language
 */

/* $begin examples */
/* linked list element */
typedef struct ELE {
    long val;
    struct ELE *next;
} *list_ptr;

/* sum_list - Sum the elements of a linked list */
long sum_list(list_ptr ls)
{
    long val = 0;
    while (ls) {
    val += ls->val;
    ls = ls->next;
    }
    return val;
}
/* rsum_list - Recursive version of sum_list */
long rsum_list(list_ptr ls)
{
    if (!ls)
    return 0;
    else {
    long val = ls->val;
    long rest = rsum_list(ls->next);
    return val + rest;
    }
}

/* copy_block - Copy src to dest and return xor checksum of src */
long copy_block(long *src, long *dest, long len)
{
    long result = 0;
    while (len > 0) {
    long val = *src++;
    *dest++ = val;
    result ^= val;
    len--;
    }
    return result;
}
/* $end examples */
```



测试链表

```
# Sample linked list
.align 8
ele1:
        .quad 0x00a
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0
```



注意，在sim/y86-code/中给出使用y86-64汇编写的asum.ys代码（用来统计数组元素个数）,当然也是原书已经给出的示例代码

先写循环方法的sumlist对用的y86-64汇编代码

```assembly
# Execution begins at address 0 
    .pos 0
    irmovq stack, %rsp      # Set up stack pointer
    call main       # Execute main program
    halt            # Terminate program 

# Array of 4 elements
    .align 8
array:  .quad 0x000d000d000d
    .quad 0x00c000c000c0
    .quad 0x0b000b000b00
    .quad 0xa000a000a000

main:   irmovq array,%rdi
    irmovq $4,%rsi
    call sum        # sum(array, 4)
    ret

# long sum(long *start, long count)
# start in %rdi, count in %rsi
sum:    irmovq $8,%r8        # Constant 8
    irmovq $1,%r9        # Constant 1
    xorq %rax,%rax       # sum = 0
    andq %rsi,%rsi       # Set CC
    jmp     test         # Goto test
loop:   mrmovq (%rdi),%r10   # Get *start
    addq %r10,%rax       # Add to sum
    addq %r8,%rdi        # start++
    subq %r9,%rsi        # count--.  Set CC
test:   jne    loop          # Stop when 0
    ret                  # Return

# Stack starts here and grows to lower addresses
    .pos 0x200
stack:

```

sum.ys

```assembly
.pos 0 #从地址0开始产生代码
    irmovq stack, %rsp
    call main
    halt

#测试链表
		.align 8
ele1:
        .quad 0x00a
        .quad ele2
ele2:
        .quad 0x0b0
        .quad ele3
ele3:
        .quad 0xc00
        .quad 0
main:
		irmovq ele1,%rdi
		call sum_list
		ret

sum_list:
		irmovq $0,%rax
		jmp test

loop:
		mrmovq 0(%rdi),%rsi
        addq %rsi,%rax
        mrmovq 8(%rdi),%rsi
        rrmovq %rsi,%rdi
        
test:
		andq %rdi,%rdi
		jne loop
		ret
		
		.pos 0x100
stack:
```



测试

```shell
./yas sum.ys  
./yis sum.yo 
Stopped in 29 steps at PC = 0x13.  Status 'HLT', CC Z=1 S=0 O=0
Changes to registers:
%rax:	0x0000000000000000	0x0000000000000cba
%rsp:	0x0000000000000000	0x0000000000000100

Changes to memory:
0x00f0:	0x0000000000000000	0x000000000000005b
0x00f8:	0x0000000000000000	0x0000000000000013
```



继续写递归版本 rsum_list，rsum.ys

```assembly
rsum_list:
	    pushq %r12
	    irmovq $0, %rax
	    andq %rdi,%rdi
	    je recursive
	    mrmovq 0(%rdi), %r12
	    mrmovq 8(%rdi), %rdi
	    call rsum_list
	    addq %r12, %rax
recursive:	   
		popq %r12
	    ret
```



测试，

```shell
./yas rsum.ys 
./yis rsum.yo 
Stopped in 42 steps at PC = 0x13.  Status 'HLT', CC Z=0 S=0 O=0
Changes to registers:
%rax:	0x0000000000000000	0x0000000000000cba
%rsp:	0x0000000000000000	0x0000000000000100

Changes to memory:
0x00b8:	0x0000000000000000	0x0000000000000c00
0x00c0:	0x0000000000000000	0x0000000000000090
0x00c8:	0x0000000000000000	0x00000000000000b0
0x00d0:	0x0000000000000000	0x0000000000000090
0x00d8:	0x0000000000000000	0x000000000000000a
0x00e0:	0x0000000000000000	0x0000000000000090
0x00f0:	0x0000000000000000	0x000000000000005b
0x00f8:	0x0000000000000000	0x0000000000000013

```



copy.ys 拷贝字节块，从一个地址到另一个地址。

```assembly

	.pos 0
	irmovq stack,%rsp
	call main
	halt

	.align 8
src:
	.quad 0x00a
	.quad 0x0b0
	.quad 0xc00

dest:
	.quad 0x111
	.quad 0x222
	.quad 0x333

main:
	irmovq src,%rdi
	irmovq dest,%rdi
	irmovq $3,%rdx
	call copy_block
	ret

copy_block:
	pushq %r12
	pushq %r13
	pushq %r14
	irmovq $1,%r13
	irmovq $8,%r14
	irmovq $0,%rax
	jmp Tloop

loop:
	mrmovq 0(%rdi),%r12
	addq %r14,%rdi
	rmmovq %r12,(%rsi)
	addq %r14,%rsi
	xorq %r12,%rax
	subq %r13,%rdx

Tloop:
	andq %rdx,%rdx
	jg loop
	popq %r14
	popq %r13
	popq %r12
	ret

	.pos 0x100
stack:
```

测试

```shell
./yas copy.ys 
./yis copy.yo 
Stopped in 45 steps at PC = 0x13.  Status 'HLT', CC Z=1 S=0 O=0
Changes to registers:
%rsp:	0x0000000000000000	0x0000000000000100
%rsi:	0x0000000000000000	0x0000000000000018
%rdi:	0x0000000000000000	0x0000000000000048

Changes to memory:
0x0000:	0x000000000100f430	0x0000000000000111
0x0008:	0x0000000048800000	0x0000000000000222
0x0010:	0x0000000000000000	0x0000000000000333
0x00f0:	0x0000000000000000	0x000000000000006f
0x00f8:	0x0000000000000000	0x0000000000000013

```





### Part B

工作目录 sim/seq

支持iaddq指令，

参见原书练习题 4.51 4.52，修改seq-full.hcl



```
#/* $begin seq-all-hcl */
####################################################################
#  HCL Description of Control for Single Cycle Y86-64 Processor SEQ   #
#  Copyright (C) Randal E. Bryant, David R. O'Hallaron, 2010       #
####################################################################

## Your task is to implement the iaddq instruction
## The file contains a declaration of the icodes
## for iaddq (IIADDQ)
## Your job is to add the rest of the logic to make it work

####################################################################
#    C Include's.  Don't alter these                               #
####################################################################

quote '#include <stdio.h>'
quote '#include "isa.h"'
quote '#include "sim.h"'
quote 'int sim_main(int argc, char *argv[]);'
quote 'word_t gen_pc(){return 0;}'
quote 'int main(int argc, char *argv[])'
quote '  {plusmode=0;return sim_main(argc,argv);}'

####################################################################
#    Declarations.  Do not change/remove/delete any of these       #
####################################################################

##### Symbolic representation of Y86-64 Instruction Codes #############
wordsig INOP 	'I_NOP'
wordsig IHALT	'I_HALT'
wordsig IRRMOVQ	'I_RRMOVQ'
wordsig IIRMOVQ	'I_IRMOVQ'
wordsig IRMMOVQ	'I_RMMOVQ'
wordsig IMRMOVQ	'I_MRMOVQ'
wordsig IOPQ	'I_ALU'
wordsig IJXX	'I_JMP'
wordsig ICALL	'I_CALL'
wordsig IRET	'I_RET'
wordsig IPUSHQ	'I_PUSHQ'
wordsig IPOPQ	'I_POPQ'
# Instruction code for iaddq instruction
wordsig IIADDQ	'I_IADDQ'

##### Symbolic represenations of Y86-64 function codes                  #####
wordsig FNONE    'F_NONE'        # Default function code

##### Symbolic representation of Y86-64 Registers referenced explicitly #####
wordsig RRSP     'REG_RSP'    	# Stack Pointer
wordsig RNONE    'REG_NONE'   	# Special value indicating "no register"

##### ALU Functions referenced explicitly                            #####
wordsig ALUADD	'A_ADD'		# ALU should add its arguments

##### Possible instruction status values                             #####
wordsig SAOK	'STAT_AOK'	# Normal execution
wordsig SADR	'STAT_ADR'	# Invalid memory address
wordsig SINS	'STAT_INS'	# Invalid instruction
wordsig SHLT	'STAT_HLT'	# Halt instruction encountered

##### Signals that can be referenced by control logic ####################

##### Fetch stage inputs		#####
wordsig pc 'pc'				# Program counter
##### Fetch stage computations		#####
wordsig imem_icode 'imem_icode'		# icode field from instruction memory
wordsig imem_ifun  'imem_ifun' 		# ifun field from instruction memory
wordsig icode	  'icode'		# Instruction control code
wordsig ifun	  'ifun'		# Instruction function
wordsig rA	  'ra'			# rA field from instruction
wordsig rB	  'rb'			# rB field from instruction
wordsig valC	  'valc'		# Constant from instruction
wordsig valP	  'valp'		# Address of following instruction
boolsig imem_error 'imem_error'		# Error signal from instruction memory
boolsig instr_valid 'instr_valid'	# Is fetched instruction valid?

##### Decode stage computations		#####
wordsig valA	'vala'			# Value from register A port
wordsig valB	'valb'			# Value from register B port

##### Execute stage computations	#####
wordsig valE	'vale'			# Value computed by ALU
boolsig Cnd	'cond'			# Branch test

##### Memory stage computations		#####
wordsig valM	'valm'			# Value read from memory
boolsig dmem_error 'dmem_error'		# Error signal from data memory


####################################################################
#    Control Signal Definitions.                                   #
####################################################################

################ Fetch Stage     ###################################

# Determine instruction code
word icode = [
	imem_error: INOP;
	1: imem_icode;		# Default: get from instruction memory
];

# Determine instruction function
word ifun = [
	imem_error: FNONE;
	1: imem_ifun;		# Default: get from instruction memory
];

#增加IIADDQ
bool instr_valid = icode in 
	{ INOP, IHALT, IRRMOVQ, IIRMOVQ, IRMMOVQ, IMRMOVQ,
	       IOPQ, IJXX, ICALL, IRET, IPUSHQ, IPOPQ, IIADDQ };

# Does fetched instruction require a regid byte?
bool need_regids =
	icode in { IRRMOVQ, IOPQ, IPUSHQ, IPOPQ, 
		     IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ };

# Does fetched instruction require a constant word?
bool need_valC =
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IJXX, ICALL, IIADDQ };

################ Decode Stage    ###################################

## What register should be used as the A source?
word srcA = [
	icode in { IRRMOVQ, IRMMOVQ, IOPQ, IPUSHQ  } : rA;
	icode in { IPOPQ, IRET } : RRSP;
	1 : RNONE; # Don't need register
];

## What register should be used as the B source?
word srcB = [
	icode in { IOPQ, IRMMOVQ, IMRMOVQ, IIADDQ  } : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't need register
];

## What register should be used as the E destination?
word dstE = [
	icode in { IRRMOVQ } && Cnd : rB;
	icode in { IIRMOVQ, IOPQ, IIADDQ } : rB;
	icode in { IPUSHQ, IPOPQ, ICALL, IRET } : RRSP;
	1 : RNONE;  # Don't write any register
];

## What register should be used as the M destination?
word dstM = [
	icode in { IMRMOVQ, IPOPQ } : rA;
	1 : RNONE;  # Don't write any register
];

################ Execute Stage   ###################################

## Select input A to ALU
word aluA = [
	icode in { IRRMOVQ, IOPQ } : valA;
	icode in { IIRMOVQ, IRMMOVQ, IMRMOVQ, IIADDQ } : valC;
	icode in { ICALL, IPUSHQ } : -8;
	icode in { IRET, IPOPQ } : 8;
	# Other instructions don't need ALU
];

## Select input B to ALU
word aluB = [
	icode in { IRMMOVQ, IMRMOVQ, IOPQ, ICALL, 
		      IPUSHQ, IRET, IPOPQ, IIADDQ } : valB;
	icode in { IRRMOVQ, IIRMOVQ } : 0;
	# Other instructions don't need ALU
];

## Set the ALU function
word alufun = [
	icode == IOPQ : ifun;
	1 : ALUADD;
];

## Should the condition codes be updated?
bool set_cc = icode in { IOPQ, IIADDQ };

################ Memory Stage    ###################################

## Set read control signal
bool mem_read = icode in { IMRMOVQ, IPOPQ, IRET };

## Set write control signal
bool mem_write = icode in { IRMMOVQ, IPUSHQ, ICALL };

## Select memory address
word mem_addr = [
	icode in { IRMMOVQ, IPUSHQ, ICALL, IMRMOVQ } : valE;
	icode in { IPOPQ, IRET } : valA;
	# Other instructions don't need address
];

## Select memory input data
word mem_data = [
	# Value from register
	icode in { IRMMOVQ, IPUSHQ } : valA;
	# Return PC
	icode == ICALL : valP;
	# Default: Don't write anything
];

## Determine instruction status
word Stat = [
	imem_error || dmem_error : SADR;
	!instr_valid: SINS;
	icode == IHALT : SHLT;
	1 : SAOK;
];

################ Program Counter Update ############################

## What address should instruction be fetched at

word new_pc = [
	# Call.  Use instruction constant
	icode == ICALL : valC;
	# Taken branch.  Use instruction constant
	icode == IJXX && Cnd : valC;
	# Completion of RET instruction.  Use value from stack
	icode == IRET : valM;
	# Default: Use incremented PC
	1 : valP;
];
#/* $end seq-all-hcl */
```



测试

```shell
make VERSION=full
# Building the seq-full.hcl version of SEQ
../misc/hcl2c -n seq-full.hcl <seq-full.hcl >seq-full.c
gcc -Wall -O2  -I../misc  -o ssim \
	seq-full.c ssim.c ../misc/isa.c  -lm
./ssim -t ../y86-code/asumi.yo 
Y86-64 Processor: seq-full.hcl
137 bytes of code read
IF: Fetched irmovq at 0x0.  ra=----, rb=%rsp, valC = 0x100
IF: Fetched call at 0xa.  ra=----, rb=----, valC = 0x38
Wrote 0x13 to address 0xf8
IF: Fetched irmovq at 0x38.  ra=----, rb=%rdi, valC = 0x18
IF: Fetched irmovq at 0x42.  ra=----, rb=%rsi, valC = 0x4
IF: Fetched call at 0x4c.  ra=----, rb=----, valC = 0x56
Wrote 0x55 to address 0xf0
IF: Fetched xorq at 0x56.  ra=%rax, rb=%rax, valC = 0x0
IF: Fetched andq at 0x58.  ra=%rsi, rb=%rsi, valC = 0x0
IF: Fetched jmp at 0x5a.  ra=----, rb=----, valC = 0x83
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched mrmovq at 0x63.  ra=%r10, rb=%rdi, valC = 0x0
IF: Fetched addq at 0x6d.  ra=%r10, rb=%rax, valC = 0x0
IF: Fetched iaddq at 0x6f.  ra=----, rb=%rdi, valC = 0x8
IF: Fetched iaddq at 0x79.  ra=----, rb=%rsi, valC = 0xffffffffffffffff
IF: Fetched jne at 0x83.  ra=----, rb=----, valC = 0x63
IF: Fetched ret at 0x8c.  ra=----, rb=----, valC = 0x0
IF: Fetched ret at 0x55.  ra=----, rb=----, valC = 0x0
IF: Fetched halt at 0x13.  ra=----, rb=----, valC = 0x0
32 instructions executed
Status = HLT
Condition Codes: Z=1 S=0 O=0
Changed Register State:
%rax:	0x0000000000000000	0x0000abcdabcdabcd
%rsp:	0x0000000000000000	0x0000000000000100
%rdi:	0x0000000000000000	0x0000000000000038
%r10:	0x0000000000000000	0x0000a000a000a000
Changed Memory State:
0x00f0:	0x0000000000000000	0x0000000000000055
0x00f8:	0x0000000000000000	0x0000000000000013
ISA Check Succeeds

```



```shell
# 使用 benchmark 测试
make testssim
../seq/ssim -t asum.yo > asum.seq
../seq/ssim -t asumr.yo > asumr.seq
../seq/ssim -t cjr.yo > cjr.seq
../seq/ssim -t j-cc.yo > j-cc.seq
../seq/ssim -t poptest.yo > poptest.seq
../seq/ssim -t pushquestion.yo > pushquestion.seq
../seq/ssim -t pushtest.yo > pushtest.seq
../seq/ssim -t prog1.yo > prog1.seq
../seq/ssim -t prog2.yo > prog2.seq
../seq/ssim -t prog3.yo > prog3.seq
../seq/ssim -t prog4.yo > prog4.seq
../seq/ssim -t prog5.yo > prog5.seq
../seq/ssim -t prog6.yo > prog6.seq
../seq/ssim -t prog7.yo > prog7.seq
../seq/ssim -t prog8.yo > prog8.seq
../seq/ssim -t ret-hazard.yo > ret-hazard.seq
grep "ISA Check" *.seq
asum.seq:ISA Check Succeeds
asumr.seq:ISA Check Succeeds
cjr.seq:ISA Check Succeeds
j-cc.seq:ISA Check Succeeds
poptest.seq:ISA Check Succeeds
prog1.seq:ISA Check Succeeds
prog2.seq:ISA Check Succeeds
prog3.seq:ISA Check Succeeds
prog4.seq:ISA Check Succeeds
prog5.seq:ISA Check Succeeds
prog6.seq:ISA Check Succeeds
prog7.seq:ISA Check Succeeds
prog8.seq:ISA Check Succeeds
pushquestion.seq:ISA Check Succeeds
pushtest.seq:ISA Check Succeeds
ret-hazard.seq:ISA Check Succeeds
rm asum.seq asumr.seq cjr.seq j-cc.seq poptest.seq pushquestion.seq pushtest.seq prog1.seq prog2.seq prog3.seq prog4.seq prog5.seq prog6.seq prog7.seq prog8.seq ret-hazard.seq

```



```sh
# 回归测试
make SIM=../seq/ssim TFLAGS=-i
./optest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 58 ISA Checks Succeed
./jtest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 96 ISA Checks Succeed
./ctest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 22 ISA Checks Succeed
./htest.pl -s ../seq/ssim -i
Simulating with ../seq/ssim
  All 756 ISA Checks Succeed
```



### Part C

待更新
