#include "frame.h"
#include "multiboot.h"

#define MAX_FRAMES 262144

static uint8_t frame_bitmap[MAX_FRAMES / 8];

static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

static void bitmap_set(uint32_t frame)
{
    frame_bitmap[frame / 8] |=
        (1 << (frame % 8));
}

static void bitmap_clear(uint32_t frame)
{
    frame_bitmap[frame / 8] &=
        ~(1 << (frame % 8));
}

static int bitmap_test(uint32_t frame)
{
    return
        frame_bitmap[frame / 8] &
        (1 << (frame % 8));
}

void frame_init(void)
{
    total_frames =
        multiboot_total_memory() /
        FRAME_SIZE;

    if (total_frames > MAX_FRAMES)
        total_frames = MAX_FRAMES;

    used_frames = 0;

    for (uint32_t i = 0;
         i < MAX_FRAMES / 8;
         i++)
    {
        frame_bitmap[i] = 0;
    }
}

uint32_t frame_alloc(void)
{
    /*
     * İlk 1 MB fiziksel belleği kullanma.
     */
    uint32_t first_frame =
        0x100000 / FRAME_SIZE;

    for (uint32_t frame = first_frame;
         frame < total_frames;
         frame++)
    {
        if (!bitmap_test(frame))
        {
            bitmap_set(frame);
            used_frames++;

            return frame * FRAME_SIZE;
        }
    }

    return 0;
}

void frame_free(uint32_t address)
{
    if (address % FRAME_SIZE != 0)
        return;

    uint32_t frame =
        address / FRAME_SIZE;

    if (frame >= total_frames)
        return;

    if (!bitmap_test(frame))
        return;

    bitmap_clear(frame);

    if (used_frames > 0)
        used_frames--;
}

uint32_t frame_total(void)
{
    return total_frames;
}

uint32_t frame_used(void)
{
    return used_frames;
}

uint32_t frame_free_count(void)
{
    return total_frames - used_frames;
}
