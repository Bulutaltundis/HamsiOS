#include "vmm.h"
#include "paging.h"
#include "frame.h"
#include "terminal.h"

static uint32_t virtual_next =
    VMALLOC_START;

static uint32_t align_up(
    uint32_t value
)
{
    return
        (value + PAGE_SIZE - 1) &
        ~(PAGE_SIZE - 1);
}

void vmm_init(void)
{
    virtual_next =
        VMALLOC_START;

    terminal_print(
        "[ OK ] Virtual memory manager initialized\n"
    );
}

uint32_t vmalloc(
    uint32_t size
)
{
    if (size == 0)
        return 0;

    uint32_t pages =
        align_up(size) / PAGE_SIZE;

    uint32_t start =
        virtual_next;

    /*
     * Adres taşmasını kontrol et.
     */
    if (pages >
        (VMALLOC_END - start) / PAGE_SIZE)
    {
        return 0;
    }

    /*
     * Önce bütün fiziksel frame'leri
     * ayır ve map et.
     */
    for (uint32_t i = 0;
         i < pages;
         i++)
    {
        uint32_t physical =
            frame_alloc();

        if (physical == 0)
        {
            /*
             * Daha önce ayırdıklarımızı geri ver.
             */
            for (
                uint32_t j = 0;
                j < i;
                j++
            )
            {
                uint32_t virtual_address =
                    start +
                    j * PAGE_SIZE;

                uint32_t mapped =
                    paging_get_physical(
                        virtual_address
                    );

                paging_unmap_page(
                    virtual_address
                );

                if (mapped != 0)
                {
                    frame_free(
                        mapped &
                        0xFFFFF000
                    );
                }
            }

            return 0;
        }

        int result =
            paging_map_page(
                start +
                i * PAGE_SIZE,
                physical,
                PAGE_WRITE
            );

        if (result != 0)
        {
            frame_free(physical);

            /*
             * Daha önceki sayfaları temizle.
             */
            for (
                uint32_t j = 0;
                j < i;
                j++
            )
            {
                uint32_t virtual_address =
                    start +
                    j * PAGE_SIZE;

                uint32_t mapped =
                    paging_get_physical(
                        virtual_address
                    );

                paging_unmap_page(
                    virtual_address
                );

                if (mapped != 0)
                {
                    frame_free(
                        mapped &
                        0xFFFFF000
                    );
                }
            }

            return 0;
        }
    }

    virtual_next =
        start +
        pages * PAGE_SIZE;

    return start;
}

void vfree(
    uint32_t address,
    uint32_t size
)
{
    if (address == 0 ||
        size == 0)
    {
        return;
    }

    uint32_t pages =
        align_up(size) / PAGE_SIZE;

    for (
        uint32_t i = 0;
        i < pages;
        i++
    )
    {
        uint32_t virtual_address =
            address +
            i * PAGE_SIZE;

        uint32_t physical =
            paging_get_physical(
                virtual_address
            );

        if (physical == 0)
            continue;

        paging_unmap_page(
            virtual_address
        );

        frame_free(
            physical &
            0xFFFFF000
        );
    }
}