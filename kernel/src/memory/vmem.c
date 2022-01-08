#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE) // number of page tables to cover the vmem
#define NR_P (SCR_SIZE / PAGE_SIZE + 1) // number of pages to cover the vmem

PTE vptable[NR_PTE * NR_P] align_to_page; // vmem page tables

PDE *get_updir();


void create_video_mapping()
{

	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
    PDE *pdir = (PDE *)va_to_pa(get_updir()) + (VMEM_ADDR >> 22);
	PTE *ptable = (PTE *)va_to_pa(vptable);
    PTE *pt_origin = (PTE *)(pdir->page_frame << 12);
	uint32_t pdir_idx, ptable_idx, pframe_idx;

	/* fill PDEs and PTEs */
	pframe_idx = VMEM_ADDR >> 12;
	for (pdir_idx = 0; pdir_idx < NR_PT; pdir_idx++)
	{
		for (ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx++)
		{
            if (ptable_idx < (VMEM_ADDR >> 12) || ptable_idx >= (VMEM_ADDR >> 12) + NR_P) {
                ptable->val = pt_origin->val;
            }
        	else {
                ptable->val = make_pte(pframe_idx << 12);
                pframe_idx++;
            }
            pt_origin++;
			ptable++;
		}
		pdir[pdir_idx].val = make_pde(ptable);
		pdir[pdir_idx + KOFFSET / PT_SIZE].val = make_pde(ptable);
	}
}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
