#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define VMALLOC_START 0x40000000
#define VMALLOC_END   0x7FC00000

void vmm_init(void);

uint32_t vmalloc(uint32_t size);

void vfree(
    uint32_t address,
    uint32_t size
);

#endif