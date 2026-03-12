#include "aplic.h"

#include <stdint.h>
#include <stdbool.h>

#include "mmio.h"
#include "uart.h"

#define NULL 0

/* Limit of IRQ_DOMAINS is the same as IRQ_DOMAINS_PER_SOCKET in hw/riscv/virt.h */
#define IRQ_DOMAINS 5

typedef struct IrqDomain {
    char name[10]; /*< name in format <mode>_<child_idx>,<parent_idx>*/
    bool mmode;
    uintptr_t base_addr;
    int32_t parent_idx;
    uint32_t child_idx[IRQ_DOMAINS];
} IrqDomain;

IrqDomain irq_domain_table[IRQ_DOMAINS] = {
    {"ROOT",    true,   ROOT_MIRQ_DOMAIN,   -1, {1, 2}},
    {"M_0,0",   true,   L0_MIRQ_DOMAIN,     0,  {3, 4}},
    {"S_1,0",   false,  L0_SIRQ_DOMAIN,     0,  {}},
    {"M_0,1",   true,   L1_MIRQ_DOMAIN,     1,  {}},
    {"S_1,1",   false,  L1_SIRQ_DOMAIN,     1,  {}},
};

static IrqDomain *src_irq_domain(uint32_t irq_src)
{
    uint32_t srccfg, child_idx, domain_idx;

    for (domain_idx = 0; domain_idx < IRQ_DOMAINS;) {
        srccfg = mmio_read_word(irq_domain_table[domain_idx].base_addr +
                                APLIC_SOURCECFG_BASE,
                                irq_src - 1);
        if (!srccfg) {
            continue;
        }

        /**
         * If D bit is 0, and the register is not readonly zeros, then the
         * interrupt is delegated in `domain_idx` interrupt domain
         */
        if (!(srccfg & APLIC_SOURCECFG_D)) {
            break;
        }
        child_idx = srccfg & APLIC_SOURCECFG_CHILDIDX_MASK;
        domain_idx = irq_domain_table[domain_idx].child_idx[child_idx];
    }

    return (domain_idx < IRQ_DOMAINS) ? &irq_domain_table[domain_idx]
                                        : NULL;
}

void aplic_init(uint32_t irq_src_count)
{
    /* ============ ROOT DOMAIN SETTINGS ============ */
    /* Setting mmsiaddrcfg & mmsiaddrcfgh */
    mmio_write_word(irq_domain_table[0].base_addr + APLIC_MMSICFGADDR,
                    0, MACHINE_IF_ADDR >> 12);
    mmio_write_word(irq_domain_table[0].base_addr + APLIC_MMSICFGADDRH,
                    0, 0);

    /* Setting smsiaddrcfg & smsiaddrcfgh */
    mmio_write_word(irq_domain_table[0].base_addr + APLIC_SMSICFGADDR,
                    0, SUPERVISOR_IF_ADDR >> 12);
    mmio_write_word(irq_domain_table[0].base_addr + APLIC_SMSICFGADDRH,
                    0, 0);

    /* Configuring sourcecfg */
    /* By default all sourcecfg show interrupt is at root & DETACH */
    for (uint32_t i = 0; i < irq_src_count; i++)
    {
        mmio_write_word(irq_domain_table[0].base_addr + APLIC_SOURCECFG_BASE,
                        i, APLIC_SOURCECFG_SM_DETACH);
    }

    /* Enabling all specified interrupt sources for root domain ONLY */
    for (uint8_t i = 0; i < (irq_src_count >> 5); i++) {
        aplic_setie(irq_domain_table[0].base_addr, i, 0xFFFFFFFF);
    }

    /* ============ COMMON DOMAIN SETTINGS ============ */
    /* Configuring domaincfg */
    for (int i = 0; i < IRQ_DOMAINS; i++) {
        mmio_write_word(irq_domain_table[i].base_addr + APLIC_DOMAINCFG,
                        0, APLIC_DOMAINCFG_IE);
    }
}

void aplic_send_msi(uint32_t irq_src, uint32_t hart_index,
                    uint32_t guest_index, uint32_t eiid)
{
    if (irq_src == 0) {
        return;
    }

    IrqDomain *irq_domain = src_irq_domain(irq_src);
    if (irq_domain == NULL) {
        return;
    }

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;
    eiid &= 0x7FFUL;

    uint32_t target = hart_index << APLIC_TARGET_HART_IDX_SHIFT;
    target |= guest_index << APLIC_TARGET_GUEST_IDX_SHIFT;
    target |= eiid;

    mmio_write_word(irq_domain->base_addr + APLIC_TARGET_BASE,
                    irq_src - 1, target);
    mmio_write_word(irq_domain->base_addr + APLIC_SETIPNUM,
                    0, irq_src);
}

void aplic_send_Nmsi(uint32_t base_irq_src, uint32_t irq_count,
                     uint32_t hart_index, uint32_t guest_index)
{
    if (base_irq_src >= IRQ_SRC_MAX || !base_irq_src) {
        return;
    }

    IrqDomain *irq_domain = src_irq_domain(base_irq_src);
    if (irq_domain == NULL) {
        return;
    }

    printf("\n~~~~~~~~~~~~~~~~\n"
           "Writing to Interrupt Domain \"%s\""
           "\n~~~~~~~~~~~~~~~~\n",
           irq_domain->name);

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;
    uint32_t target, setip_k, lsb_irq, bit_mask;
    uint32_t max_irq_src = base_irq_src + irq_count;

    for (uint32_t i = base_irq_src; (i < max_irq_src) && (i < IRQ_SRC_MAX); i++) {
        target = hart_index << APLIC_TARGET_HART_IDX_SHIFT;
        target |= guest_index << APLIC_TARGET_GUEST_IDX_SHIFT;
        /* Setting EID to value of the interrupt generated */
        target |= (uint32_t) (i);
        mmio_write_word(irq_domain->base_addr + APLIC_TARGET_BASE,
                        i - 1, target);
    }

    do {
        setip_k = base_irq_src >> 5;
        lsb_irq = base_irq_src % 32;
        bit_mask = (uint32_t) (-1L << lsb_irq);
        if (lsb_irq + irq_count < 32) {
            bit_mask &= (uint32_t) ((1UL << (lsb_irq + irq_count)) - 1);
        }

        mmio_write_word(irq_domain->base_addr + APLIC_SETIP_BASE,
                        setip_k, bit_mask);
        base_irq_src += 32 - lsb_irq;
    } while (base_irq_src < max_irq_src);
}

void aplic_conf_sourcecfg(uint32_t irq_src, uintptr_t irq_domain,
                          uint32_t child_index_or_sm, bool D)
{
    if (!irq_src) {
        return;
    }

    uint32_t sourcecfg;

    /* As we are using MSI, SM either detached or inactive */
    if (D) {
        sourcecfg = APLIC_SOURCECFG_D;
        sourcecfg |= child_index_or_sm & APLIC_SOURCECFG_CHILDIDX_MASK;
    } else {
        sourcecfg = child_index_or_sm & APLIC_SOURCECFG_SM_MASK;
    }

    mmio_write_word(irq_domain + APLIC_SOURCECFG_BASE, irq_src - 1, sourcecfg);

    /* Updating setie for new source irq_domain */
    if (!D) {
        aplic_setie(irq_domain, irq_src >> 5, 0xFFFFFFFF);
    }
}

void aplic_Nirq_delegate(uint32_t base_irq_src, uint32_t irq_count,
                         uint32_t child_idx)
{
    if (!base_irq_src) {
        return;
    }

    /**
     * This only works for immediate irq_domains (i.e. direct parent and child)
     *
     * sourcecfg MMRs have 1st interrupt at address 0, and irq_src > 0 hence
     * requied to base_irq_src - 1 to find required sourcecfg MMR address
     */
    uint32_t max_irq_src = base_irq_src + irq_count;
    IrqDomain *parent = src_irq_domain(base_irq_src);
    if (parent == NULL) {
        return;
    }

    uintptr_t parent_srccfg_addr, child_addr, child_srccfg_addr;
    parent_srccfg_addr = parent->base_addr + APLIC_SOURCECFG_BASE;
    if (!parent_srccfg_addr) {
        return;
    }

    child_addr = irq_domain_table[parent->child_idx[child_idx]].base_addr;
    child_srccfg_addr = child_addr + APLIC_SOURCECFG_BASE;

    uint32_t child_srccfg, parent_srccfg;

    for (uint32_t i = base_irq_src; i < max_irq_src; i++) {
        parent_srccfg = APLIC_SOURCECFG_D;
        parent_srccfg |= child_idx & APLIC_SOURCECFG_CHILDIDX_MASK;

        child_srccfg = APLIC_SOURCECFG_SM_DETACH;

        mmio_write_word(parent_srccfg_addr, i - 1, parent_srccfg);
        mmio_write_word(child_srccfg_addr, i - 1, child_srccfg);
    }

    /**
     * Updating setie for new source irq_domain
     * Only sources with valid sourcecfg registers get configured in setie.
     */
    while ((base_irq_src < max_irq_src) && (base_irq_src < IRQ_SRC_MAX)) {
        aplic_setie(child_addr, base_irq_src >> 5, 0xFFFFFFFF);
        base_irq_src += 32 - (base_irq_src % 32);
    }
}

void aplic_setie(uintptr_t irq_domain, uint32_t k, uint32_t bit_mask)
{
    mmio_write_word(irq_domain + APLIC_SETIE_BASE, k, bit_mask);
}

void aplic_clrie(uintptr_t irq_domain, uint32_t k, uint32_t bit_mask)
{
    mmio_write_word(irq_domain + APLIC_CLRIE_BASE, k, bit_mask);
}
