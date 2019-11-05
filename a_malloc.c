#include "a_malloc.h"

void *base = NULL;
unsigned unused_space = PAGE;

void LOG()
{
    printf("LOG\n");
    printf("Base address: [%p]\n", base);
    printf("Unused Space: [%d]\n", unused_space);
    printf("Struct block_meta_t: [%ld]\n", SIZE_STRUCT);
    block_meta_t *current = (block_meta_t *) base;
    while(current != NULL)
    {
        printf("Entity:\n");
        printf("\tUsed size: [%ld]\n", current->size);
        printf("\tAdress Pointer: [%p]\n", current);
        if(current->flag == true)
            puts("\tIs used\n");
        else 
            puts("\tIsn't used\n");
        current = current->next;
    }
}

static void *add_list(size_t size)
{
    block_meta_t *current = (block_meta_t *) base;
    if(current == NULL)
    {
        mark_block((u8 *)current, size);
        return ((u8 *)current + SIZE_STRUCT);
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
    unused_space -= (size + SIZE_STRUCT);
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

int main()
{
    char *ptr = (char *)a_malloc(sizeof(char) * 15);
    ptr = "Hello";
    int *ptr_int = (int *) a_malloc(sizeof(int));
    *ptr_int = 45;
    printf("String: %s\nInteger: %d\n", ptr, *ptr_int);
    LOG();
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

/*


int futex_wait(int *futex, int val)
{
    return syscall(SYS_futex, futex, FUTEX_WAIT, val, NULL, NULL, 0);
}

int futex_wake(int *futex)
{
    return syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 0);
}

void futex_lock(int *futex)
{
    while(__sync_val_compare_and_swap(futex, 0, 1) != 0)
        futex_wait(futex, 1);
}

void futex_unlock(int *futex)
{
    __sync_bool_compare_and_swap(futex, 1, 0);
    futex_wake(futex);
}*/