#include "memory.h"

#include <stdint.h>

#define HEAP_START 0x00400000
#define HEAP_SIZE  0x00100000

#define BLOCK_MAGIC 0x48414D53

struct memory_block
{
    uint32_t magic;
    uint32_t size;
    uint8_t free;
    struct memory_block* next;
};

static struct memory_block* first_block = 0;

static uint32_t used_memory = 0;

void memory_init(void)
{
    first_block =
        (struct memory_block*)HEAP_START;

    first_block->magic = BLOCK_MAGIC;
    first_block->size =
        HEAP_SIZE - sizeof(struct memory_block);

    first_block->free = 1;
    first_block->next = 0;

    used_memory = 0;
}

void* kmalloc(uint32_t size)
{
    if (size == 0)
        return 0;

    struct memory_block* current =
        first_block;

    while (current)
    {
        if (current->free &&
            current->size >= size)
        {
            uint32_t remaining =
                current->size - size;

            if (remaining >
                sizeof(struct memory_block) + 8)
            {
                struct memory_block* new_block =
                    (struct memory_block*)
                    (
                        (uint8_t*)current +
                        sizeof(struct memory_block) +
                        size
                    );

                new_block->magic = BLOCK_MAGIC;
                new_block->size =
                    remaining -
                    sizeof(struct memory_block);

                new_block->free = 1;
                new_block->next = current->next;

                current->next = new_block;
                current->size = size;
            }

            current->free = 0;
            used_memory += current->size;

            return (void*)
                (
                    (uint8_t*)current +
                    sizeof(struct memory_block)
                );
        }

        current = current->next;
    }

    return 0;
}

static void merge_free_blocks(void)
{
    struct memory_block* current =
        first_block;

    while (current && current->next)
    {
        struct memory_block* next =
            current->next;

        if (current->free && next->free)
        {
            current->size +=
                sizeof(struct memory_block) +
                next->size;

            current->next = next->next;

            continue;
        }

        current = current->next;
    }
}

void kfree(void* ptr)
{
    if (!ptr)
        return;

    struct memory_block* block =
        (struct memory_block*)
        (
            (uint8_t*)ptr -
            sizeof(struct memory_block)
        );

    if (block->magic != BLOCK_MAGIC)
        return;

    if (block->free)
        return;

    block->free = 1;

    if (used_memory >= block->size)
        used_memory -= block->size;
    else
        used_memory = 0;

    merge_free_blocks();
}

uint32_t memory_total(void)
{
    return HEAP_SIZE;
}

uint32_t memory_used(void)
{
    return used_memory;
}

uint32_t memory_free(void)
{
    return HEAP_SIZE - used_memory;
}
