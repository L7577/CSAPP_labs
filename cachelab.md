# Cache Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Cache Lab*](http://csapp.cs.cmu.edu/im/labs/cachelab.tar) **[Updated 5/2/16]** ([README](http://csapp.cs.cmu.edu/3e/README-cachelab), [Writeup](http://csapp.cs.cmu.edu/3e/cachelab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/cachelab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/cachelab-handout.tar))



参考：

- [IV Cache Lab - 实现一个缓存系统来加速计算](http://wdxtub.com/csapp/thick-csapp-lab-4/2016/04/16/)
- [Exely-cachelab笔记](https://github.com/Exely/CSAPP-Labs/blob/master/notes/cachelab.md)
- https://github.com/TsundereChen/csapp-cache-lab
- https://zhuanlan.zhihu.com/p/79058089



做实验前阅读 http://csapp.cs.cmu.edu/3e/README-cachelab

实验说明：http://csapp.cs.cmu.edu/3e/cachelab.pdf

15-213课程资料：https://www.cs.cmu.edu/~213/recitations/rec06_slides.pdf

对应CS:APP 第6章节

用到的工具 `valgrind` `malloc` `gdb`



### Part A 

本部分是实现一个缓存模拟器，完善csim.c文件。

已经给出了一个参考程序 `csim-ref`

```shell
./csim-ref -h
Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>
Options:
  -h         Print this help message.
  -v         Optional verbose flag.
  -s <num>   Number of set index bits.
  -E <num>   Number of lines per set.
  -b <num>   Number of block offset bits.
  -t <file>  Trace file.

Examples:
  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace

```



如何实现一个模拟缓存系统，

- 读入命令行参数，
- 创建cache模拟系统
- 读入traces文件并分析
- 异常处理（缺失、如何替换）
- 输出结果（两种输出模式）



如何读取参数，可以使用`getopt` ，可参考https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html 或 使用 `man getopt`



如何创建cache模拟器

根据读入的参数

```
  -s <num>   Number of set index bits.
  -E <num>   Number of lines per set.
  -b <num>   Number of block offset bits.
  
  S = 2^s
  创建一个二维数组
  cache[S][E]
  
// 定义cacheline数据结构
typedef struct cache_line *CacheLine;
struct cache_line
{
    int valid;
    int tag;
    int time_stamp;
};
typedef  CacheLine * Cache;
```



cache大小是变化的，需要动态申请内存，使用`malloc`

```c
    int setsize = 1 << setindexbit; // S = 2^s
    Cache cache;
    cache = malloc(setsize * sizeof(Cache));
    //initialize cache[S][E];
    for(int i = 0; i < setsize; i++){
        cache[i] = malloc(lineperset * sizeof(struct cache_line));
        for(int j = 0; j < lineperset; j++){
            cache[i][j].valid = -1;
            cache[i][j].tag = -1;
            cache[i][j].time_stamp = -1;
        }
    }
```



读入trace文件，使用`fscanf` 或者 `sscanf`



计算 标志位 索引位 

```c
    int setindex = (address >> blockoffset) & ((-1U) >> (64 - setindexbit));
    int tag = address >> (setindexbit + blockoffset);
```



异常处理，访问cache缺失，写入cache空行,或者 替换？





### Part B

分块技术

http://csapp.cs.cmu.edu/3e/waside/waside-blocking.pdf









