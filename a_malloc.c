//Add a new feature that can split a block that have a biggest area then it's needed
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
            puts("\tIsn't used");
        current = current->next;
    }
    wprint("********************\n");
}

static block_meta_t *split_block(block_meta_t *ptr, size_t size)
{
    wprint("Splitting block\n");
    if(ptr == NULL)
        return NULL;
    if(size + SIZE_STRUCT > unused_space)
        return NULL;
    block_meta_t *next_block = (block_meta_t *)((u8 *)ptr + SIZE_STRUCT + size);
    next_block->flag = false;
    next_block->next = ptr->next;
    next_block->prev = ptr;
    next_block->size = ptr->size - size;

    ptr->flag = true;
    ptr->size = size;
    unused_space -= size + SIZE_STRUCT + SIZE_STRUCT;
    return next_block;
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
    if(current->flag == false && current->size >= size)
    {
        block_meta_t *next = split_block(current, size);
        current->next = next ;
        return (u8 *)current + SIZE_STRUCT;
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
    }
    return add_list(size);
}

void free(void *ptr)
{
    if(ptr == NULL || unused_space == PAGE)
        return;

    block_meta_t *current = (block_meta_t *) ((u8 *)ptr - SIZE_STRUCT);
    block_meta_t *prev = current->prev;
    if(current->flag == false)
        return;
    if(prev != NULL)
    {
        if(prev->flag == false)
        {
            prev->next = current->next;
            prev->size += current->size + SIZE_STRUCT;
            unused_space += current->size + SIZE_STRUCT;
            return; 
        }
    }
    current->flag = false;
    unused_space += current->size + SIZE_STRUCT;
}

int main()
{
    base = sbrk(PAGE);
    block_meta_t *bpt = (block_meta_t *) base;
    bpt->next = NULL;
    bpt->prev = NULL;
    bpt->size = PAGE - SIZE_STRUCT;
    bpt->flag = false;
    block_meta_t *next = split_block(bpt, 1024);
    bpt->next = next;

    /*int *ptr = (int *) a_malloc(sizeof(int));
    wprint("FIRST POINTER: [%d]\n", ptr);
    int *arr = (int *) a_malloc(sizeof(int) * 10);
    char *string = (char *) a_malloc(sizeof(char) * 50);
    free(ptr);
    free(arr);
    free(string);
    void *v = a_malloc(sizeof(int));
    free(v);
    v = a_malloc(sizeof(int));*/
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
