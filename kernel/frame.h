#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

#define FRAME_SIZE 4096

void frame_init(void);

uint32_t frame_alloc(void);
void frame_free(uint32_t address);

uint32_t frame_total(void);
uint32_t frame_used(void);
uint32_t frame_free_count(void);

#endif
