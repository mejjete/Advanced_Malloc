#include "a_malloc.h"

void *base = NULL;
unsigned unused_space = PAGE;

void LOG(bool gl)
{
    if(base == NULL)
        return;
    printf("\n********************\n");
    printf("Base address: [%p]\n", base);
    printf("Unused Space: [%d] bytes\n", unused_space);
    printf("Struct block_meta_t: [%ld]\n", SIZE_STRUCT);
    if(gl == false)
    {
        printf("\n********************\n");
        return;
    }
    block_meta_t *current = (block_meta_t *) base;
    while(current != NULL)
    {
        printf("Entity:\n");
        printf("\tUsed size: [%ld] bytes\n", current->size);
        printf("\tAdress User Pointer: [%p]\n", (u8 *)current + SIZE_STRUCT);
        if(current->flag == true)
            puts("\tIs used");
        else 
            puts("\tIsn't used");
        current = current->next;
    }
    printf("********************\n");
}

static void *add_list(size_t size)
{
    block_meta_t *current = (block_meta_t *) base;
    if(unused_space == PAGE)
    {
        mark_block((u8 *)current, size);
        return (u8* )base + SIZE_STRUCT;
    }
    while(current->next != NULL)
        current = current->next;
    u8 *next_block = ((u8 *)current) + SIZE_STRUCT + current->size;
    mark_block(next_block, size);
    current->next = (block_meta_t *) next_block;
    u8 *pointer = ((u8 *)current + SIZE_STRUCT);
    return (void *)pointer;
}

static void mark_block(u8 *ptr, size_t size)
{
    block_meta_t *bpt = (block_meta_t *) ptr;
    bpt->next = NULL;
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
    }
    return add_list(size);
}

void free(void *ptr)
{
    if(unused_space == PAGE || ptr == NULL || base == NULL)
        return;
    block_meta_t *current = (block_meta_t *)base;
    block_meta_t *prev = NULL;
    while(((u8 *)current + SIZE_STRUCT) != ptr && current->next != NULL)
    {
        prev = current;
        current = current->next;
    }
    printf("value ptr: [%p]\n", (u8 *)current + SIZE_STRUCT);
    printf("block ptr: [%p]\n", (u8 *)current);
    current->flag = false;
    unused_space += current->size + SIZE_STRUCT;
    if(prev->flag != true)
    {
        current->flag = false;
        prev->next = current->next;
        prev->size += current->size;
        unused_space += current->size + SIZE_STRUCT;
    }
}

int main()
{
    int *ptr = (int *) a_malloc(sizeof(int));
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

//14123008
//14123032