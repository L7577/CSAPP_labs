# Malloc Lab

CSAPP:http://csapp.cs.cmu.edu/3e/labs.html

[*Malloc Lab*](http://csapp.cs.cmu.edu/im/labs/malloclab.tar) *[Updated 9/2/14]* ([README](http://csapp.cs.cmu.edu/3e/README-malloclab), [Writeup](http://csapp.cs.cmu.edu/3e/malloclab.pdf), [Release Notes](http://csapp.cs.cmu.edu/3e/malloclab-release.html), [Self-Study Handout](http://csapp.cs.cmu.edu/3e/malloclab-handout.tar))

---

相关课程资料参考[15-213/14-513/15-513: Intro to Computer Systems, Spring 2022-Schedule](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/schedule.html)

 Dynamic Memory Allocation: Basic ([pptx](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/13-malloc-basic.pptx), [pdf](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/13-malloc-basic.pdf), [video 15213](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=2c811af2-bc64-4c7d-be2d-ae4b014ecf3a), [video 14513](https://ece.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=a6c21c9a-360f-48ae-9ebc-ae4b01535fe6&start=0))

Dynamic Memory Allocation: Advanced ([pptx](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/14-malloc-advanced.pptx), [pdf](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/14-malloc-advanced.pdf), [video 15213](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=3e33e9b8-1365-41d2-aed2-ae4d01520c4b), [video 14513](https://ece.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=61fd3acc-f4e7-4cff-9e7b-ae4d0151dc79&start=0)

*Recitation 8: Malloc lab (Part I)* ([slides](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/recitations/rec08_slides.pdf))

Linking ([pptx](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/15-linking.pptx), [pdf](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/15-linking.pdf), [video 15213](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=81e76000-0215-492a-85b3-ae590141c39d), [video 14513](https://ece.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=fea6c726-0f1d-4812-884d-ae590140d746))

Virtual Memory: Concepts ([pptx](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/16-vm-concepts.pptx), [pdf](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/lectures/16-vm-concepts.pdf), [video 15213](https://scs.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=f5cb944b-879b-4cba-a2e6-ae5b0137b26d), [video 14513](https://ece.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=36c29c5e-1e57-41ab-8f00-ae5b01448f70))

**Bootcamp 5: Malloc** ([slides](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/bootcamps/mallocbootcamp.pdf))

*Recitation 9: Malloc lab (Part II)* ([slides](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/recitations/rec09_slides.pdf), [tar](https://www.cs.cmu.edu/afs/cs/academic/class/15213-s22/www/activities/rec9.tar))

---



教材中已经给出了一个简单的示例[mm.c](http://csapp.cs.cmu.edu/3e/ics3/code/vm/malloc/mm.c)基于隐式空闲链表的分配器，

但是测试表现出的性能和内存使用率并不出彩

```sh
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.008190   695
 1       yes   99%    5848  0.006830   856
 2       yes   99%    6648  0.011606   573
 3       yes  100%    5380  0.008706   618
 4       yes   66%   14400  0.000103139400
 5       yes   92%    4800  0.007116   675
 6       yes   92%    4800  0.007095   676
 7       yes   55%   12000  0.158104    76
 8       yes   51%   24000  1.353026    18
 9       yes   27%   14401  0.242974    59
10       yes   34%   14401  0.007435  1937
Total          74%  112372  1.811187    62

Perf index = 44 (util) + 4 (thru) = 49/100

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


## Phase 1: 隐式空闲链表 (Implicit Free List)

教材第 9.9 节的隐式空闲链表实现。空闲块通过边界标记隐式链接，分配时采用首次适配。

```sh
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.008190   695
 1       yes   99%    5848  0.006830   856
 2       yes   99%    6648  0.011606   573
 3       yes  100%    5380  0.008706   618
 4       yes   66%   14400  0.000103139400
 5       yes   92%    4800  0.007116   675
 6       yes   92%    4800  0.007095   676
 7       yes   55%   12000  0.158104    76
 8       yes   51%   24000  1.353026    18
 9       yes   27%   14401  0.242974    59
10       yes   34%   14401  0.007435  1937
Total          74%  112372  1.811187    62

Perf index = 44 (util) + 4 (thru) = 49/100

```

吞吐量问题 trace 7/8 中大堆首次适配需扫描全部已分配块，Kops 仅 18 和 76。利用率问题 trace 9/10 中 realloc 总是 malloc+copy+free 导致严重外部碎片，利用率仅 27% 和 34%。


## Phase 2: 显式空闲链表 (Explicit Free List)

在空闲块 payload 区域嵌入前驱/后继指针，形成双向显式空闲链表。分配时仅遍历空闲块，避免扫描已分配块。

空闲块结构变化：

```
Phase 1 (隐式):
  空闲块: [header(4B)] [未使用 payload ...] [footer(4B)]

Phase 2 (显式):
  空闲块: [header(4B)] [pred(8B)] [succ(8B)] [...] [footer(4B)]
```

最小块从 16 字节增加到 32 字节（header 4 + pred 8 + succ 8 + footer 4 = 24，对齐至 32）。

新增函数：
- `insert_free_block` — LIFO 头部插入，O(1)
- `remove_free_block` — 双向链表摘除，O(1)

修改函数：
- `find_fit` — 从遍历堆（含已分配块）改为遍历 free_list_head
- `coalesce` — 合并前从链表摘除邻居，合并后插入结果
- `place` — 从链表摘除被分配块，切分后插入剩余块

```sh
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   89%    5694  0.000267 21326
 1       yes   92%    5848  0.000120 48896
 2       yes   94%    6648  0.000353 18849
 3       yes   96%    5380  0.000191 28182
 4       yes   66%   14400  0.000129111801
 5       yes   88%    4800  0.000394 12170
 6       yes   85%    4800  0.000412 11665
 7       yes   55%   12000  0.004628  2593
 8       yes   50%   24000  0.002946  8147
 9       yes   26%   14401  0.129025   112
10       yes   34%   14401  0.007265  1982
Total          70%  112372  0.145729   771

Perf index = 42 (util) + 40 (thru) = 82/100
```

对比 Phase 1: 吞吐量 +1143%（62 → 771 Kops），trace 7/8 从 18/76 提升至 2593/8147。利用率 -4%（74% → 70%），因为最小块增大导致小分配内部碎片增加。

### 改进方向

引入分离适配 (Segregated Fits)，按大小分桶减少单一链表长度。


## Phase 3: 分离适配 (Segregated Fits)

在 Phase 2 基础上将单一空闲链表拆分为 9 个大小类，每类独立维护双向空闲链表。

大小类划分：

| class | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|-------|---|---|---|---|---|---|---|---|---|----|
| 范围 | ≤32 | ≤64 | ≤128 | ≤256 | ≤512 | ≤1024 | ≤2048 | ≤4096 | >4096 |

分配时从适配的最小类开始搜索，未找到则到更大的类。释放时根据合并后大小插入对应类。

新增函数：
- `get_size_class` — 根据块大小返回对应类索引

修改：
- `free_list_head` → `free_list_heads[9]` 数组
- `find_fit` — 从对应类开始，逐类向上搜索
- `insert/remove` — 根据块大小定位对应类链表

```sh
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   97%    5694  0.000186 30695
 1       yes   97%    5848  0.000177 33002
 2       yes   98%    6648  0.000212 31403
 3       yes   99%    5380  0.000200 26954
 4       yes   66%   14400  0.000318 45297
 5       yes   89%    4800  0.000312 15365
 6       yes   86%    4800  0.000357 13434
 7       yes   55%   12000  0.000328 36641
 8       yes   50%   24000  0.000893 26879
 9       yes   30%   14401  0.238055    60
10       yes   44%   14401  0.008804  1636
Total          74%  112372  0.249841   450

Perf index = 44 (util) + 30 (thru) = 74/100
```

对比 Phase 2: 利用率恢复至 74%（+4%），trace 0~3 恢复至 97-99%（接近 Phase 1）。trace 7/8 吞吐量进一步提升（2593→36641，8147→26879）。trace 9/10 吞吐量下降，因为分离链表导致 realloc 模式的搜索效率降低，这将在 Phase 4 优化 realloc 中解决。

### 改进方向

优化 realloc — 尝试原地扩展、与下一空闲块合并扩展。


## Phase 4: 优化 realloc

将 naive realloc (malloc+copy+free) 替换为三步策略：

1. **原地收缩** — 新大小 ≤ 旧大小，切分多余空间
2. **向后合并扩展** — 下一块空闲且合并后够用，直接合并无需 copy
3. **Fallback** — 以上都不行才 malloc+copy+free

```sh
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   97%    5694  0.000817  6970
 1       yes   97%    5848  0.001093  5352
 2       yes   98%    6648  0.001106  6012
 3       yes   99%    5380  0.000942  5709
 4       yes   66%   14400  0.001601  8994
 5       yes   89%    4800  0.001383  3471
 6       yes   86%    4800  0.001647  2915
 7       yes   55%   12000  0.001370  8760
 8       yes   50%   24000  0.002757  8705
 9       yes   33%   14401  0.141962   101
10       yes   30%   14401  0.002367  6083
Total          73%  112372  0.157045   716

Perf index = 44 (util) + 40 (thru) = 84/100
```

trace 9 利用率 30%→33%，吞吐量 60→101。trace 10 吞吐量 1636→6083。

### 各阶段对比

| Phase | 方案 | util | thru | 得分 |
|-------|------|:---:|:----:|:----:|
| 1 | 隐式空闲链表 | 74% | 62 | 49 |
| 2 | 显式空闲链表 | 70% | 771 | 82 |
| 3 | 分离适配 | 74% | 450 | 74 |
| 4 | 优化 realloc | 73% | 716 | **84** |

### 改进方向

Phase 5: 去掉已分配块的 footer，用 header 中 1 bit 标记前块是否已分配，节省每块 4 字节。



