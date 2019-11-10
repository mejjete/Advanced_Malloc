//need to advance a free mehanism 
#include "header/a_malloc.h"
#include "header/out.h"                                

void *base = NULL;
unsigned unused_space = PAGE;

void LOG(bool gl)
{
    if(base == NULL)
        return;
    wprint("\n********************\n");
    wprint("Base address: [%d]\n", base);
    wprint("Unused Space: [%d] bytes\n", unused_space);
    wprint("Struct block_meta_t: [%d]\n\n", SIZE_STRUCT);
    if(gl == false)
    {
        wprint("\n********************\n");
        return;
    }
    block_meta_t *current = (block_meta_t *) base;
    while(current != NULL)
    {
        wprint("Entity:\n");
        wprint("\tUsed size: [%d] bytes\n", current->size);
        wprint("\tAdress User Pointer: [%d]\n", (u8 *)current + SIZE_STRUCT);
        if(current->flag == true)
            puts("\tIs used");
        else 
            puts("\tFREE");
        current = current->next;
    }
    wprint("********************\n");
}

struct a_mallinfo_t a_mallinfo()
{
    struct a_mallinfo_t info;
    if(base == NULL)
    {
        info.arena = info.nofch = info.mxar = info.mreg = 0;
        return info;
    }
    info.arena = unused_space;
    info.mreg = PAGE - unused_space;
    info.mxar = PAGE;
    info.nofch = 0;
    block_meta_t *ptr = (block_meta_t *)base;
    while(ptr != NULL)
    {
        info.nofch++;
        ptr = ptr->next;
    }
    return info;
}   


static block_meta_t *split_block(block_meta_t *ptr, size_t size)
{
    if(ptr == NULL)
        return NULL;
    if(ptr->size < (size + SIZE_STRUCT))
    {
        ptr->flag = true;
        unused_space -= size;
        return ptr->next;
    }
    block_meta_t *next_block = (block_meta_t *)((u8 *)ptr + SIZE_STRUCT + size);
    size_t cur_size = ptr->size;
    unused_space -= (size + SIZE_STRUCT);
    next_block->flag = false;
    next_block->next = ptr->next;
    next_block->prev = ptr;
    next_block->size = unused_space - SIZE_STRUCT;
    next_block->size = cur_size - (size + SIZE_STRUCT);

    ptr->flag = true;
    ptr->size = size;
    return next_block;
}

static void *add_list(size_t size)
{
    block_meta_t *current = (block_meta_t *) base;
    block_meta_t *prev = current;
    while(current->next != NULL)
    {
        if(current->flag == false && current->size >= size) 
        {
            block_meta_t *s = split_block(current, size);
            current->next = s;
            return ((u8 *)current + SIZE_STRUCT);
        }
        prev = current;
        current = current->next;
    }
    if(current->flag == false && current->size + SIZE_STRUCT >= size)
    {
        block_meta_t *next = split_block(current, size);
        current->next = next;
        return ((u8 *)current + SIZE_STRUCT);
    }
    u8 *next_block = ((u8 *)current) + SIZE_STRUCT + current->size;
    mark_block(next_block, size, NULL, prev);
    current->next = (block_meta_t *) next_block;
    u8 *pointer = (u8 *)next_block + SIZE_STRUCT;
    return (void *)pointer;
}

static void mark_block(u8 *ptr, size_t size, block_meta_t *next, block_meta_t *prev)
{
    block_meta_t *bpt = (block_meta_t *) ptr;
    bpt->next = next;
    bpt->prev = prev;
    bpt->size = size;
    bpt->flag = true;
    unused_space -= size + SIZE_STRUCT;
}

static void *morescore(size_t size)
{
    if(size < PAGE)
        size = PAGE;
    void *ptr =  sbrk(size);
    if(ptr == MMAP_ERROR)
        return NULL;
    return ptr;
}

void *a_malloc(size_t size)
{
    if(size >= unused_space) 
        return NULL;
    if(base == NULL)
    {
        base = morescore(size);
        if(base == MMAP_ERROR)
        {
            printf("Morescore Error!\n");
            exit(EXIT_FAILURE);
        }
        block_meta_t *bpt = (block_meta_t *) base;
        bpt->next = NULL;
        bpt->prev = NULL;
        bpt->size = PAGE - SIZE_STRUCT;
        unused_space -= SIZE_STRUCT;
        bpt->flag = false;
    }
    return add_list(size);
}

void *a_realloc(void *ptr, size_t size)
{
    block_meta_t *current = (block_meta_t *)((u8 *)ptr - SIZE_STRUCT);
    if(current == NULL && current->size >= size)
        return ptr;
    wfree(ptr);
    return add_list(size);
}

void *a_calloc(size_t nitems, size_t size)
{
    void *ptr = a_malloc(size);
    if(ptr != NULL)
    {
        memset(ptr, 0, nitems);
        return ptr;
    }   
    return NULL;
}

static void unite_block(block_meta_t *dest, block_meta_t *src)
{
    if(dest == NULL || src == NULL)
        return;
    if(src->next != NULL)
        src->next->prev = dest;
    dest->next = src->next;
    dest->size += src->size + SIZE_STRUCT;
}

void wfree(void *ptr)
{
    block_meta_t *block = (block_meta_t *)(ptr - SIZE_STRUCT);
    if(block == NULL || block->flag == false)
        return;
    unused_space += block->size;
    block->flag = false;
    block_meta_t *next;
    block_meta_t *prev = block->prev;
    while(prev != NULL && prev->flag == false)
    {
        unite_block(prev, block);
        wprint("\n");
        unused_space += SIZE_STRUCT;
        block = prev;
        prev = prev->prev;
    }
    next = block->next;
    while(next != NULL && next->flag == false)
    {
        unite_block(block, next);
        wprint("\n");
        unused_space += SIZE_STRUCT;
        block = next;
        next = next->next;
    }
} 

int main()
{
    char *ptr = (char *)a_malloc(10);
    int *i_ptr = (int *)a_malloc(sizeof(int) * 4);
    int *i2_ptr = (int *)a_malloc(sizeof(int) * 10);
    wfree(i_ptr);
    wfree(ptr);
    wfree(i2_ptr);
    LOG(true);
    return 0;
}


    /*char *ptr = (char *)a_malloc(10);
    int *i_ptr = (int *)a_malloc(sizeof(int) * 4);
    int *i2_ptr = (int *)a_malloc(sizeof(int) * 10);
    wfree(i_ptr);
    wfree(ptr);
    wfree(i2_ptr);*/