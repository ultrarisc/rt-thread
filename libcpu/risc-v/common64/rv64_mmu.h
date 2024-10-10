/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-30     lizhirui     first version
 * 2023-10-12     Shell        Add permission control API
 * 2024-10-09     ZhangJing    Rename the file and add MMU attribute definitions of RV64
 */

#ifndef __RV64_MMU_H__
#define __RV64_MMU_H__

#include "riscv.h"
#include <mm_aspace.h>
#include <stddef.h>

/* RAM, Flash, or ROM */
#define NORMAL_MEM 0
/* normal nocache memory mapping type */
#define NORMAL_NOCACHE_MEM 1
/* MMIO region */
#define DEVICE_MEM 2

typedef size_t rt_pte_t;

struct mem_desc
{
    rt_size_t vaddr_start;
    rt_size_t vaddr_end;
    rt_ubase_t paddr_start;
    rt_size_t attr;
    struct rt_varea varea;
};

#define GET_PF_ID(addr)     ((addr) >> PAGE_OFFSET_BIT)
#define GET_PF_OFFSET(addr) __MASKVALUE(addr, PAGE_OFFSET_MASK)
#define GET_L1(addr)        __PARTBIT(addr, VPN2_SHIFT, VPN2_BIT)
#define GET_L2(addr)        __PARTBIT(addr, VPN1_SHIFT, VPN1_BIT)
#define GET_L3(addr)        __PARTBIT(addr, VPN0_SHIFT, VPN0_BIT)
#define GET_PPN(pte)                                                           \
    (__PARTBIT(pte, PTE_PPN_SHIFT, PHYSICAL_ADDRESS_WIDTH_BITS - PTE_PPN_SHIFT))
#define GET_PADDR(pte)            (GET_PPN(pte) << PAGE_OFFSET_BIT)
#define VPN_TO_PPN(vaddr, pv_off) (((rt_uintptr_t)(vaddr)) + (pv_off))
#define PPN_TO_VPN(paddr, pv_off) (((rt_uintptr_t)(paddr)) - (pv_off))
#define COMBINEVADDR(l1_off, l2_off, l3_off)                                   \
    (((l1_off) << VPN2_SHIFT) | ((l2_off) << VPN1_SHIFT) |                     \
     ((l3_off) << VPN0_SHIFT))
#define COMBINEPTE(paddr, attr)                                                \
    ((((paddr) >> PAGE_OFFSET_BIT) << PTE_PPN_SHIFT) | (attr))

#define MMU_MAP_ERROR_VANOTALIGN -1
#define MMU_MAP_ERROR_PANOTALIGN -2
#define MMU_MAP_ERROR_NOPAGE     -3
#define MMU_MAP_ERROR_CONFLICT   -4

#undef PAGE_SIZE

#define PAGE_OFFSET_SHIFT 0
#define PAGE_OFFSET_BIT   12
#define PAGE_SIZE         __SIZE(PAGE_OFFSET_BIT)
#define PAGE_OFFSET_MASK  __MASK(PAGE_OFFSET_BIT)
#define VPN0_SHIFT        (PAGE_OFFSET_SHIFT + PAGE_OFFSET_BIT)
#define VPN0_BIT          9
#define VPN1_SHIFT        (VPN0_SHIFT + VPN0_BIT)
#define VPN1_BIT          9
#define VPN2_SHIFT        (VPN1_SHIFT + VPN1_BIT)
#define VPN2_BIT          9

#define PPN0_SHIFT (PAGE_OFFSET_SHIFT + PAGE_OFFSET_BIT)
#define PPN0_BIT   9
#define PPN1_SHIFT (PPN0_SHIFT + PPN0_BIT)
#define PPN1_BIT   9
#define PPN2_SHIFT (PPN1_SHIFT + PPN1_BIT)
#define PPN2_BIT   26
#define PPN_BITS   (PPN0_BIT + PPN1_BIT + PPN2_BIT)

#define L1_PAGE_SIZE __SIZE(PAGE_OFFSET_BIT + VPN0_BIT + VPN1_BIT)
#define L2_PAGE_SIZE __SIZE(PAGE_OFFSET_BIT + VPN0_BIT)
#define L3_PAGE_SIZE __SIZE(PAGE_OFFSET_BIT)

#define ARCH_ADDRESS_WIDTH_BITS 64

#define PHYSICAL_ADDRESS_WIDTH_BITS 56

#define PAGE_ATTR_NEXT_LEVEL  (0)
#define PAGE_ATTR_RWX         (PTE_X | PTE_W | PTE_R)
#define PAGE_ATTR_READONLY    (PTE_R)
#define PAGE_ATTR_XN          (PTE_W | PTE_R)
#define PAGE_ATTR_READEXECUTE (PTE_X | PTE_R)

#define PAGE_ATTR_USER   (PTE_U)
#define PAGE_ATTR_SYSTEM (0)

#define PAGE_DEFAULT_ATTR_LEAF (PAGE_ATTR_RWX | PAGE_ATTR_USER | PTE_V | PTE_G)
#define PAGE_DEFAULT_ATTR_NEXT (PAGE_ATTR_NEXT_LEVEL | PTE_V | PTE_G)

#define PAGE_IS_LEAF(pte) __MASKVALUE(pte, PAGE_ATTR_RWX)

#define PTE_USED(pte) __MASKVALUE(pte, PTE_V)

/**
 * encoding of SATP (Supervisor Address Translation and Protection register)
 */
#define SATP_MODE_OFFSET 60
#define SATP_MODE_BARE   0
#define SATP_MODE_SV39   8
#define SATP_MODE_SV48   9
#define SATP_MODE_SV57   10
#define SATP_MODE_SV64   11

#define ARCH_VADDR_WIDTH 39
#define SATP_MODE        SATP_MODE_SV39

#define MMU_MAP_K_DEVICE  (PTE_G | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_K_RWCB    (PTE_G | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_K_RW      (PTE_G | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_U_RWCB    (PTE_U | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_U_ROCB 	  (PTE_U | PTE_R | PTE_V)
#define MMU_MAP_U_RWCB_XN (PTE_U | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_U_RW      (PTE_U | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_EARLY     (PAGE_ATTR_RWX | PTE_G | PTE_V)

#define PTE_XWR_MASK 0xe

#define ARCH_PAGE_SIZE   PAGE_SIZE
#define ARCH_PAGE_MASK   (ARCH_PAGE_SIZE - 1)
#define ARCH_PAGE_SHIFT  PAGE_OFFSET_BIT
#define ARCH_INDEX_WIDTH 9
#define ARCH_INDEX_SIZE  (1ul << ARCH_INDEX_WIDTH)
#define ARCH_INDEX_MASK  (ARCH_INDEX_SIZE - 1)

#define ARCH_MAP_FAILED ((void *)-1)

#define RT_HW_MMU_PROT_READ    1
#define RT_HW_MMU_PROT_WRITE   2
#define RT_HW_MMU_PROT_EXECUTE 4
#define RT_HW_MMU_PROT_KERNEL  8
#define RT_HW_MMU_PROT_USER    16
#define RT_HW_MMU_PROT_CACHE   32

struct rt_aspace;

/**
 * @brief Remove permission from attribution
 *
 * @param attr architecture specified mmu attribution
 * @param prot protect that will be removed
 * @return size_t returned attribution
 */
rt_inline size_t rt_hw_mmu_attr_rm_perm(size_t attr, rt_base_t prot)
{
    switch (prot)
    {
        /* remove write permission for user */
        case RT_HW_MMU_PROT_WRITE | RT_HW_MMU_PROT_USER:
            attr &= ~PTE_W;
            break;
        /* remove write permission for kernel */
        case RT_HW_MMU_PROT_WRITE | RT_HW_MMU_PROT_KERNEL:
            attr &= ~PTE_W;
            break;
        default:
            RT_ASSERT(0);
    }
    return attr;
}

/**
 * @brief Add permission from attribution
 *
 * @param attr architecture specified mmu attribution
 * @param prot protect that will be added
 * @return size_t returned attribution
 */
rt_inline size_t rt_hw_mmu_attr_add_perm(size_t attr, rt_base_t prot)
{
    switch (prot)
    {
        /* add write permission for user */
        case RT_HW_MMU_PROT_WRITE | RT_HW_MMU_PROT_USER:
            attr |= (PTE_R | PTE_W | PTE_U);
            break;
        default:
            RT_ASSERT(0);
    }
    return attr;
}

/**
 * @brief Test permission from attribution
 *
 * @param attr architecture specified mmu attribution
 * @param prot protect that will be test
 * @return rt_bool_t RT_TRUE if the prot is allowed, otherwise RT_FALSE
 */
rt_inline rt_bool_t rt_hw_mmu_attr_test_perm(size_t attr, rt_base_t prot)
{
    rt_bool_t rc = 0;
    switch (prot & ~RT_HW_MMU_PROT_USER)
    {
        /* test write permission for user */
        case RT_HW_MMU_PROT_WRITE:
            rc = ((attr & PTE_W) && (attr & PTE_R));
            break;
        case RT_HW_MMU_PROT_READ:
            rc = !!(attr & PTE_R);
            break;
        case RT_HW_MMU_PROT_EXECUTE:
            rc = !!(attr & PTE_X);
            break;
        default:
            RT_ASSERT(0);
    }

    if (rc && (prot & RT_HW_MMU_PROT_USER))
    {
        rc = !!(attr & PTE_U);
    }
    return rc;
}

void *rt_hw_mmu_tbl_get(void);
int rt_hw_mmu_map_init(rt_aspace_t aspace, void *v_address, rt_ubase_t size,
                       rt_ubase_t *vtable, rt_ubase_t pv_off);
void rt_hw_mmu_setup(rt_aspace_t aspace, struct mem_desc *mdesc, int desc_nr);
void rt_hw_mmu_kernel_map_init(rt_aspace_t aspace, rt_ubase_t vaddr_start,
                               rt_ubase_t size);
void *rt_hw_mmu_map(rt_aspace_t aspace, void *v_addr, void *p_addr, size_t size,
                    size_t attr);
void rt_hw_mmu_unmap(rt_aspace_t aspace, void *v_addr, size_t size);
void rt_hw_aspace_switch(rt_aspace_t aspace);
void *rt_hw_mmu_v2p(rt_aspace_t aspace, void *vaddr);

int rt_hw_mmu_control(struct rt_aspace *aspace, void *vaddr, size_t size,
                      enum rt_mmu_cntl cmd);

void *rt_hw_mmu_pgtbl_create(void);
void rt_hw_mmu_pgtbl_delete(void *pgtbl);

void mmu_set_pagetable(rt_ubase_t addr);
void mmu_enable_user_page_access(void);
void mmu_disable_user_page_access(void);
void rt_hw_asid_init(void);
void rt_hw_asid_switch_pgtbl(struct rt_aspace *aspace, rt_ubase_t pgtbl);

#endif
