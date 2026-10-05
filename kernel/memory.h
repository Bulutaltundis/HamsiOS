#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void memory_init(void);

void* kmalloc(uint32_t size);
void kfree(void* ptr);

uint32_t memory_total(void);
uint32_t memory_used(void);
uint32_t memory_free(void);

#endif
