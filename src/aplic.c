#include "aplic.h"

#include <stdint.h>
#include <stdbool.h>

#define NULL 0
/* As QEMU only allows a maximum of 16 child domains */
#define INTR_DOMAINS 2
uintptr_t intr_domain_table[INTR_DOMAINS] = {ROOT_MINTR_DOMAIN, C0_SINTR_DOMAIN};

static uintptr_t src_intr_domain(uint16_t intr_src)
{
    uint32_t *sourcecfg;
    bool D;
    uint8_t domain_idx = 0;
    do {
        sourcecfg = (uint32_t *) (intr_domain_table[domain_idx] + APLIC_SOURCECFG_BASE);
        sourcecfg += intr_src - 1;
        D = !!(*sourcecfg & APLIC_SOURCECFG_D);
        // TODO: Add logic to parse which child to accesses
    } while (D && (++domain_idx < INTR_DOMAINS));

    return (domain_idx < INTR_DOMAINS) ? intr_domain_table[domain_idx] : NULL;
}

void aplic_init(uint16_t intr_src_count)
{
    /* ============ ROOT DOMAIN SETTINGS ============ */
    /* Setting mmsiaddrcfg & mmsiaddrcfgh */
    uint32_t *mmsiaddrcfg = (uint32_t *) (intr_domain_table[0] + APLIC_MMSICFGADDR);
    uint32_t *mmsiaddrcfgh = (uint32_t *) (intr_domain_table[0]  + APLIC_MMSICFGADDRH);
    *mmsiaddrcfg = MACHINE_IF_ADDR >> 12;
    *mmsiaddrcfgh = 0UL;

    /* Setting smsiaddrcfg & smsiaddrcfgh */
    uint32_t *smsiaddrcfg = (uint32_t *) (intr_domain_table[0] + APLIC_SMSICFGADDR);
    uint32_t *smsiaddrcfgh = (uint32_t *) (intr_domain_table[0] + APLIC_SMSICFGADDRH);
    *smsiaddrcfg = SUPERVISOR_IF_ADDR >> 12;
    *smsiaddrcfgh = 0UL;

    /* Configuring sourcecfg */
    /* By default all sourcecfg show interrupt is at root & DETACH */
    uint32_t *sourcecfg = (uint32_t *) (intr_domain_table[0] + APLIC_SOURCECFG_BASE);
    for (int i = 0; i < intr_src_count; i++)
    {
        *(sourcecfg + i) |= APLIC_SOURCECFG_SM_DETACH;
    }

    /* Enabling all interrupt sources from 1 - 31 for root domain ONLY */
    aplic_setie(intr_domain_table[0], 0, 0xFFFFFFFE);

    /* ============ COMMON DOMAIN SETTINGS ============ */
    /* Configuring domaincfg */
    uint32_t *domaincfg;
    for (int i = 0; i < INTR_DOMAINS; i++) {
        domaincfg = (uint32_t *) (intr_domain_table[i] + APLIC_DOMAINCFG);
        *domaincfg |= APLIC_DOMAINCFG_IE;
    }
}

void aplic_send_msi(uint16_t intr_src, uint16_t hart_index, uint8_t guest_index, uint16_t eiid)
{
    if (intr_src == 0) {
        return;
    }

    uintptr_t intr_domain_addr = src_intr_domain(intr_src);
    if (intr_domain_addr == NULL) {
        return;
    }

    uint32_t *target = (uint32_t *) (intr_domain_addr + APLIC_TARGET_BASE);
    uint32_t *setipnum = (uint32_t *) (intr_domain_addr + APLIC_SETIPNUM);

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;
    eiid &= 0x7FFUL;


    target += (intr_src - 1);
    *target = (uint32_t) hart_index << APLIC_TARGET_HART_IDX_SHIFT;
    *target |= ((uint32_t) guest_index << APLIC_TARGET_GUEST_IDX_SHIFT);
    *target |= (uint32_t) eiid;

    *setipnum = (uint32_t) intr_src;
}

void aplic_send_Nmsi(uint16_t intr_src, uint16_t hart_index, uint8_t guest_index, uint32_t bit_mask)
{
    /* intr_src must is multiple of 32 and smaller than intr_src */
    if ((intr_src % 32 != 0) || (intr_src > 1023)) {
        return;
    }

    uintptr_t intr_domain_addr = src_intr_domain(intr_src);
    if (intr_domain_addr == NULL) {
        return;
    }

    uint32_t *target = (uint32_t *) (intr_domain_addr + APLIC_TARGET_BASE);
    uint32_t *setip = (uint32_t *) (intr_domain_addr + APLIC_SETIP_BASE);

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;

    target += (intr_src - 1);
    for (int i = 0; i < 32; i++) {
        if (bit_mask & (0x1UL << i)) {
            *target = (uint32_t) hart_index << APLIC_TARGET_HART_IDX_SHIFT;
            *target |= ((uint32_t) guest_index << APLIC_TARGET_GUEST_IDX_SHIFT);
            // Setting EID to value of intr_src + 1
            *target |= (uint32_t) (intr_src + i + 1);
        }
        target++;
    }

    setip += (intr_src >> 5);
    *setip = bit_mask;
}

void aplic_conf_sourcecfg(uintptr_t intr_domain, uint16_t intr_src, bool D, uint16_t child_index_or_sm)
{
    intr_src--;

    uint32_t *sourcecfg = (uint32_t *) (intr_domain + APLIC_SOURCECFG_BASE);
    sourcecfg += intr_src;
    *sourcecfg = 0;

    /* As we are using MSI, SM either detached or inactive */
    if (D) {
        *sourcecfg |= (uint32_t) APLIC_SOURCECFG_D;
        *sourcecfg |= (uint32_t) (child_index_or_sm & APLIC_SOURCECFG_CHILDIDX_MASK);
    } else {
        // TODO: Add check to ensure child_index_or_sm is 0 or 1
        // otherwise set to inactive
        *sourcecfg |= (uint32_t) (child_index_or_sm & APLIC_SOURCECFG_SM_MASK);
        /* Updating setie for new source intr_domain */
        aplic_setie(intr_domain, (uint8_t) (intr_src / 32), 0xFFFFFFFF);
    }
}

void aplic_setie(uintptr_t intr_domain, uint8_t k, uint32_t bit_mask)
{
    uint32_t *setie = (uint32_t *) (intr_domain + APLIC_SETIE_BASE);
    setie += k;
    *setie = bit_mask;
}

void aplic_in_clrie(uintptr_t intr_domain, uint8_t k, uint32_t bit_mask)
{
    uint32_t *clrie = (uint32_t *) (intr_domain + APLIC_CLRIE_BASE);
    clrie += k;
    *clrie = bit_mask;
}
