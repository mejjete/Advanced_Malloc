//Fixed free 
#include "a_malloc.h"
#include "output/out.h"

void *base = NULL;
unsigned unused_space = PAGE;

void LOG(bool gl)
{
    if(base == NULL)
        return;
    wprint("\n********************\n");
    wprint("Base address: [%d]\n", base);
    wprint("Unused Space: [%d] bytes\n", unused_space);
    wprint("Struct block_meta_t: [%d]\n", SIZE_STRUCT);
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

static block_meta_t *split_block(block_meta_t *ptr, size_t size)
{
    if(ptr == NULL)
        return NULL;
    block_meta_t *next_block = (block_meta_t *)((u8 *)ptr + SIZE_STRUCT + size);
    size_t cur_size = ptr->size;
    unused_space -= size + SIZE_STRUCT;
    next_block->flag = false;
    next_block->next = ptr->next;
    next_block->prev = ptr;
    next_block->size = cur_size - (size + SIZE_STRUCT);
    next_block->size -= SIZE_STRUCT;

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
            mark_block((u8 *)current, size, current->next, prev);
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

static void unite_block(block_meta_t *dest, block_meta_t *src)
{
    if(dest == NULL || src == NULL)
        return;
    dest->next = src->next;
    dest->size = src->size + SIZE_STRUCT;
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

//#define FREE_1
#define FREE_2

#ifdef FREE_1
void a_free(void *ptr)
{
    block_meta_t *current = (block_meta_t *) ((u8 *)ptr - SIZE_STRUCT);
    if(ptr == NULL || unused_space == PAGE || current->flag == false)
        return;

    block_meta_t *next = current->next;
    block_meta_t *prev = current->prev;

    if(prev != NULL)
    {
        if(prev->flag == false)
        {
            prev->next = current->next;
            unused_space += current->size + SIZE_STRUCT;
            prev->size += current->size + SIZE_STRUCT;
            current->flag = false;
            return; 
        }
    }
    if(next != NULL)
    {
        if(next->flag == false)
        {
            current->next = next->next;
            unused_space += current->size + SIZE_STRUCT;
            current->size += next->size + SIZE_STRUCT;
            current->flag = false;
            return; 
        }
    }
    current->flag = false;
    unused_space += current->size + SIZE_STRUCT;
}
#endif

#ifdef FREE_2

void a_free(void *ptr)
{
    block_meta_t *current = (block_meta_t *) ((u8 *)ptr - SIZE_STRUCT);
    current->flag = false;
    unused_space += current->size + SIZE_STRUCT;

}
#endif

int main()
{
    int *p1 = (int *) a_malloc(sizeof(int));
    //int *p2 = (int *) a_malloc(sizeof(int) * 5);
    a_free(p1);
    LOG(true);
    return 0;
}

static void spin_lock(volatile bool *lock)
{
    while(!__sync_bool_compare_and_swap(lock, 0, 1))
        ;
}

static void spin_unlock(volatile bool *lock)
{
    __sync_bool_compare_and_swap(lock, 1, 0);
}
