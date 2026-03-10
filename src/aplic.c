#include "aplic.h"

#include <stdint.h>
#include <stdbool.h>

#include "mmio.h"

#define NULL 0

/* Limit of IRQ_DOMAINS is the same as IRQ_DOMAINS_PER_SOCKET in hw/riscv/virt.h */
#define IRQ_DOMAINS 2

uintptr_t irq_domain_table[IRQ_DOMAINS] = {
    ROOT_MIRQ_DOMAIN,
    C1_SIRQ_DOMAIN
};

/* #define IRQ_DOMAINS 5

uintptr_t irq_domain_table[IRQ_DOMAINS] = {
    ROOT_MIRQ_DOMAIN,
    C0_MIRQ_DOMAIN,
    C1_SIRQ_DOMAIN,
    C2_SIRQ_DOMAIN,
    C3_SIRQ_DOMAIN,
};
*/

static uintptr_t src_irq_domain(uint32_t irq_src)
{
    uint32_t srccfg;
    bool D;
    uint8_t domain_idx = 0;
    do {
        srccfg = mmio_read_word(irq_domain_table[domain_idx] +
                                APLIC_SOURCECFG_BASE,
                                irq_src - 1);
        D = !!(srccfg & APLIC_SOURCECFG_D);
    } while (D && (++domain_idx < IRQ_DOMAINS));

    return (domain_idx < IRQ_DOMAINS) ? irq_domain_table[domain_idx] : NULL;
}

void aplic_init(uint32_t irq_src_count)
{
    /* ============ ROOT DOMAIN SETTINGS ============ */
    /* Setting mmsiaddrcfg & mmsiaddrcfgh */
    mmio_write_word(irq_domain_table[0] + APLIC_MMSICFGADDR, 0,
                    MACHINE_IF_ADDR >> 12);
    mmio_write_word(irq_domain_table[0] + APLIC_MMSICFGADDRH, 0, 0);

    /* Setting smsiaddrcfg & smsiaddrcfgh */
    mmio_write_word(irq_domain_table[0] + APLIC_SMSICFGADDR, 0,
                    SUPERVISOR_IF_ADDR >> 12);
    mmio_write_word(irq_domain_table[0] + APLIC_SMSICFGADDRH, 0, 0);

    /* Configuring sourcecfg */
    /* By default all sourcecfg show interrupt is at root & DETACH */
    for (uint32_t i = 0; i < irq_src_count; i++)
    {
        mmio_write_word(irq_domain_table[0] + APLIC_SOURCECFG_BASE,
                        i, APLIC_SOURCECFG_SM_DETACH);
    }

    /* Enabling all specified interrupt sources for root domain ONLY */
    for (uint8_t i = 0; i < (irq_src_count >> 5); i++) {
        aplic_setie(irq_domain_table[0], i, 0xFFFFFFFF);
    }

    /* ============ COMMON DOMAIN SETTINGS ============ */
    /* Configuring domaincfg */
    for (int i = 0; i < IRQ_DOMAINS; i++) {
        mmio_write_word(irq_domain_table[i] + APLIC_DOMAINCFG, 0,
                        APLIC_DOMAINCFG_IE);
    }
}

void aplic_send_msi(uint32_t irq_src, uint32_t hart_index,
                    uint32_t guest_index, uint32_t eiid)
{
    if (irq_src == 0) {
        return;
    }

    uintptr_t irq_domain = src_irq_domain(irq_src);
    if (irq_domain == NULL) {
        return;
    }

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;
    eiid &= 0x7FFUL;

    uint32_t target = hart_index << APLIC_TARGET_HART_IDX_SHIFT;
    target |= guest_index << APLIC_TARGET_GUEST_IDX_SHIFT;
    target |= eiid;

    mmio_write_word(irq_domain + APLIC_TARGET_BASE, irq_src - 1, target);
    mmio_write_word(irq_domain + APLIC_SETIPNUM, 0, irq_src);
}

void aplic_send_Nmsi(uint32_t base_irq_src, uint32_t irq_count,
                     uint32_t hart_index, uint32_t guest_index)
{
    if (base_irq_src >= IRQ_SRC_MAX || !base_irq_src) {
        return;
    }

    uintptr_t irq_domain = src_irq_domain(base_irq_src);
    if (irq_domain == NULL) {
        return;
    }

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;
    uint32_t target, setip_k, lsb_irq, bit_mask;
    uint32_t max_irq_src = base_irq_src + irq_count;

    for (uint32_t i = base_irq_src; (i < max_irq_src) && (i < IRQ_SRC_MAX); i++) {
        target = hart_index << APLIC_TARGET_HART_IDX_SHIFT;
        target |= guest_index << APLIC_TARGET_GUEST_IDX_SHIFT;
        /* Setting EID to value of the interrupt generated */
        target |= (uint32_t) (i);
        mmio_write_word(irq_domain + APLIC_TARGET_BASE, i - 1, target);
    }

    do {
        setip_k = base_irq_src >> 5;
        lsb_irq = base_irq_src % 32;
        bit_mask = (uint32_t) (-1L << lsb_irq);
        if (lsb_irq + irq_count < 32) {
            bit_mask &= (uint32_t) ((1UL << (lsb_irq + irq_count)) - 1);
        }

        mmio_write_word(irq_domain + APLIC_SETIP_BASE, setip_k, bit_mask);
        base_irq_src += 32 - lsb_irq;
    } while (base_irq_src < max_irq_src);
}

void aplic_conf_sourcecfg(uint32_t irq_src, uintptr_t irq_domain,
                          uint32_t child_index_or_sm, bool D)
{
    if (!irq_src) {
        return;
    }

    uint32_t sourcecfg = 0;

    /* As we are using MSI, SM either detached or inactive */
    if (D) {
        sourcecfg |= APLIC_SOURCECFG_D;
        sourcecfg |= child_index_or_sm & APLIC_SOURCECFG_CHILDIDX_MASK;
    } else {
        sourcecfg |= child_index_or_sm & APLIC_SOURCECFG_SM_MASK;
    }

    mmio_write_word(irq_domain + APLIC_SOURCECFG_BASE, irq_src - 1, sourcecfg);

    /* Updating setie for new source irq_domain */
    if (!D) {
        aplic_setie(irq_domain, irq_src >> 5, 0xFFFFFFFF);
    }
}

void aplic_Nirq_delegate(uint32_t base_irq_src, uint32_t irq_count,
                         uintptr_t irq_domain, uint32_t child_index)
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
    uintptr_t parent_srccfg_addr = src_irq_domain(base_irq_src) +
                                   APLIC_SOURCECFG_BASE;
    if (!parent_srccfg_addr) {
        return;
    }

    uint32_t child_srccfg, parent_srccfg;

    for (uint32_t i = base_irq_src; i < max_irq_src; i++) {
        parent_srccfg = APLIC_SOURCECFG_D;
        parent_srccfg |= child_index & APLIC_SOURCECFG_CHILDIDX_MASK;

        child_srccfg = APLIC_SOURCECFG_SM_DETACH;

        mmio_write_word(parent_srccfg_addr, i - 1, parent_srccfg);
        mmio_write_word(irq_domain + APLIC_SOURCECFG_BASE, i - 1,
                        child_srccfg);
    }

    /**
     * Updating setie for new source irq_domain
     * Only sources with valid sourcecfg registers get configured in setie.
     */
    while ((base_irq_src < max_irq_src) && (base_irq_src < IRQ_SRC_MAX)) {
        aplic_setie(irq_domain, base_irq_src >> 5, 0xFFFFFFFF);
        base_irq_src += 32 - (base_irq_src % 32);
    }
}

void aplic_setie(uintptr_t irq_domain, uint32_t k, uint32_t bit_mask)
{
    mmio_write_word(irq_domain + APLIC_SETIE_BASE, k, bit_mask);
}

void aplic_in_clrie(uintptr_t irq_domain, uint32_t k, uint32_t bit_mask)
{
    mmio_write_word(irq_domain + APLIC_CLRIE_BASE, k, bit_mask);
}
