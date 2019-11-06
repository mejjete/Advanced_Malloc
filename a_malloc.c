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
            puts("\tIsn't used");
        current = current->next;
    }
    wprint("********************\n");
}

static void *add_list(size_t size)
{
    block_meta_t *current = (block_meta_t *) base;
    block_meta_t *prev = current;
    if(unused_space == PAGE)
    {
        mark_block((u8 *)current, size, NULL, NULL);
        return ((u8* )current + SIZE_STRUCT);
    }
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
    u8 *next_block = ((u8 *)current) + SIZE_STRUCT + current->size;
    mark_block(next_block, size, NULL, prev);
    current->next = (block_meta_t *) next_block;
    u8 *pointer = ((u8 *)next_block + SIZE_STRUCT);
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
    }
    return add_list(size);
}

void free(void *ptr)
{
    if(ptr == NULL || unused_space == PAGE)
        return;

    block_meta_t *current = (block_meta_t *) ((u8 *)ptr - SIZE_STRUCT);
    block_meta_t *prev = current->prev;
    wprint("FREE FUNCTION\n");
    wprint("POINTER: %d\n", ptr);
    if(current->flag == false)
        return;
    if(prev != NULL)
    {
        if(prev->flag == false)
        {
            prev->next = current->next;
            prev->size += current->size;
            unused_space += current->size + SIZE_STRUCT; 
        }
    }
    current->flag = false;
    unused_space += current->size + SIZE_STRUCT;
}

int main()
{
    int *ptr = (int *) a_malloc(sizeof(int));
    int *arr = (int *) a_malloc(sizeof(int) * 10);
    free(arr);
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
