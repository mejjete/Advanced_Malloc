#ifndef _ADVANCED_MALLOC_
    #define _ADVANCED_MALLOC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
    #include <Windows.h>
#endif
#ifdef __linux__
    #include <unistd.h>
#endif
#include <stdbool.h>
#include <stdint.h>

typedef struct block_meta
{
    struct block_meta *next;
    struct block_meta *prev;
    size_t size; 
    volatile bool flag;
}block_meta_t;

struct a_mallinfo_t
{
    int arena;      /* non mapped region */
    int nofch;      /* number of free chunks */
    int mreg;       /* number of mmaped region */
    int mxar;       /* maximum total alocated size */
};

#define MMAP_ERROR ((void *) -1)
#define PAGE 16384
#define SIZE_STRUCT (sizeof(block_meta_t))
#define u8 uint8_t 

static void unite_block(block_meta_t *dest, block_meta_t *src);
static void spin_lock(volatile bool *lock);
static void spin_unlock(volatile bool *lock);
void LOG(bool gl);
static block_meta_t *split_block(block_meta_t *ptr, size_t size);
static void *add_list(size_t size);
static void *morescore(size_t size);
static void mark_block(u8 *ptr, size_t size, block_meta_t *next, block_meta_t *prev);

struct a_mallinfo_t a_mallinfo();
void *a_calloc(size_t nitems, size_t size);
void *a_realloc(void *ptr, size_t size);
void *a_malloc(size_t size);
void wfree(void *ptr);

#endif