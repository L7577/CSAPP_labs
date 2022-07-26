# Cache Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Cache Lab*](http://csapp.cs.cmu.edu/im/labs/cachelab.tar) **[Updated 5/2/16]** ([README](http://csapp.cs.cmu.edu/3e/README-cachelab), [Writeup](http://csapp.cs.cmu.edu/3e/cachelab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/cachelab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/cachelab-handout.tar))

---

相关课程资料[15-213/15-513: Introduction to Computer Systems (ICS)
Summer 2022](http://www.cs.cmu.edu/~213/index.html)

The Memory Hierarchy ([activity](http://www.cs.cmu.edu/~213/activities/213_lecture10.pdf), [activity-sol](http://www.cs.cmu.edu/~213/activities/213_lecture10-sol.pdf), [pdf](http://www.cs.cmu.edu/~213/lectures/10-memory-hierarchy.pdf), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=d0f67315-458a-4d6d-b113-ae3d014a98ed))

Cache Memories ([activity](http://www.cs.cmu.edu/~213/activities/213_lecture12.pdf), [activity-sol](http://www.cs.cmu.edu/~213/activities/213_lecture12-sol.pdf), [pdf](http://www.cs.cmu.edu/~213/lectures/12-cache-memories.pdf), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=c53132d7-cecd-4d09-9fe7-ae3f0150d14e))

C Bootcamp ([tar](http://www.cs.cmu.edu/~213/activities/cbootcamp.tar.gz), [pdf](http://www.cs.cmu.edu/~213/lectures/cbootcamp-m22.pdf), [video](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=b63440c4-4d60-413d-85eb-ad4f017bf090)) 

---

参考：

- https://zhuanlan.zhihu.com/p/79058089
- [CSAPP - Cache Lab的更(最)优秀的解法](https://zhuanlan.zhihu.com/p/387662272)

做实验前阅读 http://csapp.cs.cmu.edu/3e/README-cachelab

实验说明：http://csapp.cs.cmu.edu/3e/cachelab.pdf

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



```c
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
    cache = (Cache)malloc(setsize * sizeof(Cache));
    //initialize cache[S][E];
    for(int i = 0; i < setsize; i++){
        cache[i] = (CacheLine)malloc(lineperset * sizeof(struct cache_line));
        for(int j = 0; j < lineperset; j++){
            cache[i][j].valid = -1;
            cache[i][j].tag = -1;
            cache[i][j].time_stamp = -1;
        }
    }
```

读入trace文件，使用`fscanf` 或者 `sscanf`

```c
while(fscanf(tracefile," %c %lx,%d",&operation,&address,&size)>0){
    ...
}
```

计算 标志位 索引位 

```c
    int setindex = (address >> blockoffset) & ((-1U) >> (64 - setindexbit));
    int tag = address >> (setindexbit + blockoffset);
```

异常处理，访问cache缺失，写入cache空行,或者 替换？

```c
// hit 
for (int i = 0; i < lineperset; i++){
    if ( cache[setindex][i].tag == tag && cache[setindex][i].valid == 1 ){
        cache[setindex][i].time_stamp = count;
        hit_count++;
        hit_flag = true;
        miss_flag = false;
        if(verbose){
            printf(" hit ");
        }
        break;
    }
}
// if miss , then storage
if (miss_flag){
    miss_count++;
    if (verbose)
        printf(" miss ");
    for (int i = 0; i < lineperset; i++){
        if ( cache[setindex][i].valid == -1 ){
            cache[setindex][i].valid = 1;
            cache[setindex][i].tag = tag;
            cache[setindex][i].time_stamp = count;
            eviction_flag = false;
            hit_flag = true;
            break;
        }
    }
    // if miss and the cacheset is full , then eviction , use lru .
    if (eviction_flag){
        for(int i = 0; i < lineperset; i++){
            if (cache[setindex][i].time_stamp < max_time_stamp){
                max_time_stamp = cache[setindex][i].time_stamp;
                max_time_stamp_index = i;
            }
        }
        eviction_count++;
        hit_flag = true;
        if (verbose) {
            printf ("eviction ");
        }
        // update old cache[][] tag and time_stamp
        cache[setindex][max_time_stamp_index].tag = tag;
        cache[setindex][max_time_stamp_index].time_stamp = count;
    }
}
if (fetch_count == 2 && hit_flag ){
    hit_count++;
    if(verbose)
        printf("hit ");
}  
if (verbose)
    printf("\n");
```

测试

```shell
Part A: Testing cache simulator
Running ./test-csim
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27
```

### Part B

分块技术

http://csapp.cs.cmu.edu/3e/waside/waside-blocking.pdf

完善trans.c文件

32*32

最简单的分块方式，但是只cache缺失也有343，没有达到及格要求。

```c
/*  cache : s: 5 E: 1 b: 5 
    S = 2^5=32 , E = 1;  Blockoffset 2^5=32 sizeof(int)=4  
    per line : 32 / 4 = 8 
    */

/*  32*32 bsize=8  misses: 343 */
for (kk = 0; kk < N; kk += bsize) {
    for (jj = 0; jj < M; jj += bsize) {
        for (k = kk; k < (kk + bsize); k++){
            for (j = 0; j < bsize; j++){
                int tmp = A[k][jj+j];
                B[jj+j][k] = tmp;
            }
        }
    }
}
```

增加局部变量，miss次数降到了287

```c
   // 32*32 misses: 287
    for (kk = 0; kk < N; kk += bsize) {
        for (jj = 0; jj < M; jj += bsize) {
            for (k = kk; k < (kk + bsize); k++){
                    int t1 = A[k][jj];
                    int t2 = A[k][jj+1];
                    int t3 = A[k][jj+2];
                    int t4 = A[k][jj+3];
                    int t5 = A[k][jj+4];
                    int t6 = A[k][jj+5];
                    int t7 = A[k][jj+6];
                    int t8 = A[k][jj+7];

                    B[jj][k] = t1;
                    B[jj+1][k] = t2;
                    B[jj+2][k] = t3;
                    B[jj+3][k] = t4;
                    B[jj+4][k] = t5;
                    B[jj+5][k] = t6;
                    B[jj+6][k] = t7;
                    B[jj+7][k] = t8;
            }
        }
    }
```

64*64 

待更新
