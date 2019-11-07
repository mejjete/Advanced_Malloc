#ifndef _ADVANCED_MALLOC_
    #define _ADVANCED_MALLOC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct block_meta
{
    struct block_meta *next;
    struct block_meta *prev;
    size_t size; 
    volatile bool flag;
}block_meta_t;

#define MMAP_ERROR ((void *) -1)
#define PAGE 4096
#define SIZE_STRUCT (sizeof(block_meta_t))
#define u8 uint8_t 

static void spin_lock(volatile bool *lock);
static void spin_unlock(volatile bool *lock);
static void LOG(bool gl);
static block_meta_t *split_block(block_meta_t *ptr, size_t size);
static void *add_list(size_t size);
static void *morescore(size_t size);
static void mark_block(u8 *ptr, size_t size, block_meta_t *next, block_meta_t *prev);
void *a_malloc(size_t size);
void a_free(void *ptr);

#endif