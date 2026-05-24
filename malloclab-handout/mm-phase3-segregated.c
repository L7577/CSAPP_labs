/*
 * Phase 3: Segregated explicit free list allocator.
 * Maintains 9 size-class free lists. First-fit search starts from
 * the smallest adequate size class and moves up.
 * Boundary tag coalescing on free. Minimum block size 32 bytes.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mm.h"
#include "memlib.h"

team_t team = {
    "ateam",
    "Harry Bovik",
    "bovik@cs.cmu.edu",
    "",
    ""
};

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

/* Read and write a pointer at address p */
#define GET_PTR(p)      (*(char **)(p))
#define PUT_PTR(p, val) (*(char **)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Given free block ptr bp, get predecessor and successor in free list */
#define PRED(bp)  GET_PTR(bp)
#define SUCC(bp)  GET_PTR((char *)(bp) + DSIZE)

/* Minimum block size: header(4) + pred(8) + succ(8) + footer(4) = 32 */
#define MIN_BLOCK  (4*DSIZE)

/* Segregated free list: number of size classes */
#define NUM_CLASSES 9
/* $end mallocmacros */

/* Size class thresholds: blocks up to this size go in the class */
static const size_t class_threshold[NUM_CLASSES] = {
    32, 64, 128, 256, 512, 1024, 2048, 4096,
    (size_t)-1  /* unlimited */
};

/* Global variables */
static char *heap_listp = 0;                    /* Pointer to prologue block */
static char *free_list_heads[NUM_CLASSES];       /* Heads of segregated free lists */

/* Function prototypes */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static int get_size_class(size_t size);
static void insert_free_block(void *bp);
static void remove_free_block(void *bp);
static void printblock(void *bp);
static void checkheap(int verbose);
static void checkblock(void *bp);

/*
 * get_size_class - Return the smallest size class that can hold size.
 */
static int get_size_class(size_t size)
{
    int i;
    for (i = 0; i < NUM_CLASSES; i++) {
        if (size <= class_threshold[i])
            return i;
    }
    return NUM_CLASSES - 1;
}

/*
 * insert_free_block - Insert free block at head of its size class list (LIFO).
 */
static void insert_free_block(void *bp)
{
    int cls = get_size_class(GET_SIZE(HDRP(bp)));
    char *head = free_list_heads[cls];

    PUT_PTR(bp, NULL);                         /* PRED(bp) = NULL */
    PUT_PTR((char *)(bp) + DSIZE, head);       /* SUCC(bp) = head */
    if (head != NULL)
        PUT_PTR(head, bp);                     /* PRED(head) = bp */
    free_list_heads[cls] = bp;
}

/*
 * remove_free_block - Remove a free block from its size class list.
 */
static void remove_free_block(void *bp)
{
    int cls = get_size_class(GET_SIZE(HDRP(bp)));
    char *pred = PRED(bp);
    char *succ = SUCC(bp);

    if (pred != NULL)
        PUT_PTR((char *)(pred) + DSIZE, succ);
    else
        free_list_heads[cls] = succ;

    if (succ != NULL)
        PUT_PTR(succ, pred);
}

/*
 * mm_init - Initialize the memory manager.
 */
/* $begin mminit */
int mm_init(void)
{
    int i;

    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                          /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE);
    /* $end mminit */

    for (i = 0; i < NUM_CLASSES; i++)
        free_list_heads[i] = NULL;

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}
/* $end mminit */

/*
 * mm_malloc - Allocate a block with at least size bytes of payload.
 */
/* $begin mmmalloc */
void *mm_malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* $end mmmalloc */
    if (heap_listp == 0){
        mm_init();
    }
    /* $begin mmmalloc */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = MIN_BLOCK;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    if (asize < MIN_BLOCK)
        asize = MIN_BLOCK;

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}
/* $end mmmalloc */

/*
 * mm_free - Free a block and coalesce with neighbors.
 */
/* $begin mmfree */
void mm_free(void *bp)
{
    /* $end mmfree */
    if (bp == 0)
        return;

    if (heap_listp == 0){
        mm_init();
    }
    /* $begin mmfree */

    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}
/* $end mmfree */

/*
 * coalesce - Boundary tag coalescing with free list management.
 *            Remove adjacent free blocks from their size class lists,
 *            merge, then insert the merged block.
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            /* Case 1 */
        insert_free_block(bp);
        return bp;
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        remove_free_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_free_block(bp);
        return bp;
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        remove_free_block(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        insert_free_block(bp);
        return bp;
    }

    else {                                     /* Case 4 */
        remove_free_block(PREV_BLKP(bp));
        remove_free_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        insert_free_block(bp);
        return bp;
    }
}

/*
 * mm_realloc - Naive implementation of realloc.
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    if(size == 0) {
        mm_free(ptr);
        return 0;
    }

    if(ptr == NULL) {
        return mm_malloc(size);
    }

    newptr = mm_malloc(size);

    if(!newptr) {
        return 0;
    }

    oldsize = GET_SIZE(HDRP(ptr));
    if(size < oldsize) oldsize = size;
    memcpy(newptr, ptr, oldsize);

    mm_free(ptr);

    return newptr;
}

/*
 * mm_checkheap - Check the heap for correctness.
 */
void mm_checkheap(int verbose)
{
    checkheap(verbose);
}

/*
 * The remaining routines are internal helper routines.
 */

/*
 * extend_heap - Extend heap with free block and coalesce.
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}
/* $end mmextendheap */

/*
 * place - Place block of asize bytes at start of free block bp.
 *         Remove bp from free list. Split if remainder >= MIN_BLOCK.
 */
/* $begin mmplace */
/* $begin mmplace-proto */
static void place(void *bp, size_t asize)
/* $end mmplace-proto */
{
    size_t csize = GET_SIZE(HDRP(bp));

    remove_free_block(bp);

    if ((csize - asize) >= MIN_BLOCK) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        insert_free_block(bp);
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}
/* $end mmplace */

/*
 * find_fit - Segregated first-fit search.
 *            Start from the smallest adequate size class and move up.
 */
/* $begin mmfirstfit */
/* $begin mmfirstfit-proto */
static void *find_fit(size_t asize)
/* $end mmfirstfit-proto */
{
    /* $end mmfirstfit */

    int cls;
    char *bp;

    for (cls = get_size_class(asize); cls < NUM_CLASSES; cls++) {
        for (bp = free_list_heads[cls]; bp != NULL; bp = SUCC(bp)) {
            if (asize <= GET_SIZE(HDRP(bp)))
                return bp;
        }
    }
    return NULL;
}
/* $end mmfirstfit */

static void printblock(void *bp)
{
    size_t hsize, halloc, fsize, falloc;

    checkheap(0);
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    if (hsize == 0) {
        printf("%p: EOL\n", bp);
        return;
    }

    printf("%p: header: [%ld:%c] footer: [%ld:%c]\n", bp,
           hsize, (halloc ? 'a' : 'f'),
           fsize, (falloc ? 'a' : 'f'));
}

static void checkblock(void *bp)
{
    if ((size_t)bp % DSIZE)
        printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Error: header does not match footer\n");
}

/*
 * checkheap - Minimal check of the heap for consistency.
 */
void checkheap(int verbose)
{
    char *bp = heap_listp;

    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp);

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (verbose)
            printblock(bp);
        checkblock(bp);
    }

    if (verbose)
        printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
        printf("Bad epilogue header\n");
}
