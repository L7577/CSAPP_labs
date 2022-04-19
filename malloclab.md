# Malloc Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Malloc Lab*](http://csapp.cs.cmu.edu/im/labs/malloclab.tar) *[Updated 9/2/14]* ([README](http://csapp.cs.cmu.edu/3e/README-malloclab), [Writeup](http://csapp.cs.cmu.edu/3e/malloclab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/malloclab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/malloclab-handout.tar))







教材中已经给出了一个简单的示例[mm.c](http://csapp.cs.cmu.edu/3e/ics3/code/vm/malloc/mm.c)基于隐式空闲链表的分配器，

但是测试表现出的性能和内存使用率并不出彩

```sh
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.007063   806
 1       yes   99%    5848  0.006416   912
 2       yes   99%    6648  0.011257   591
 3       yes  100%    5380  0.008115   663
 4       yes   66%   14400  0.000098146640
 5       yes   92%    4800  0.007517   639
 6       yes   92%    4800  0.007290   658
 7       yes   55%   12000  0.162680    74
 8       yes   51%   24000  0.342581    70
 9       yes   27%   14401  0.066802   216
10       yes   34%   14401  0.002288  6294
Total          74%  112372  0.622106   181

Perf index = 44 (util) + 12 (thru) = 56/100

```







完善`mm.c`



```c
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    return 0;
}

```





```c
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

```



```c

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
}

```





```c

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

```



