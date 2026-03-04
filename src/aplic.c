#include "aplic.h"

#include <stdint.h>
#include <stdbool.h>

void aplic_init(uintptr_t aplic_addr, uint16_t intr_src_count)
{
    /* Configuring domaincfg */
    uint32_t *domaincfg = (uint32_t *) (aplic_addr + APLIC_DOMAINCFG);
    *domaincfg |= APLIC_DOMAINCFG_IE;
    *domaincfg |= APLIC_DOMAINCFG_DM;

    /* Setting mmsiaddrcfg & mmsiaddrcfgh */
    uint32_t *mmsiaddrcfg = (uint32_t *) (aplic_addr + APLIC_MMSICFGADDR);
    *mmsiaddrcfg = MACHINE_IF_ADDR >> 12;
    uint32_t *mmsiaddrcfgh = (uint32_t *) (aplic_addr + APLIC_MMSICFGADDRH);
    *mmsiaddrcfgh = 0UL;

    /* Setting smsiaddrcfg & smsiaddrcfgh */
    uint32_t *smsiaddrcfg = (uint32_t *) (aplic_addr + APLIC_SMSICFGADDR);
    *smsiaddrcfg = SUPERVISOR_IF_ADDR >> 12;
    uint32_t *smsiaddrcfgh = (uint32_t *) (aplic_addr + APLIC_SMSICFGADDRH);
    *smsiaddrcfgh = 0UL;

    /* Configuring sourcecfg */
    /* By default all root domain sourcecfg are SM & DETACH */
    uint32_t *sourcecfg = (uint32_t *) (aplic_addr + APLIC_SOURCECFG_BASE);
    for (int i = 0; i < intr_src_count; i++)
    {
        *(sourcecfg + i) |= APLIC_SOURCECFG_SM_DETACH;
    }

    /* Enabling all interrupt sources from 1 - 31 */
    aplic_setie(M_INTR_DOMAIN, 0, 0xFFFFFFFE);
}

void aplic_send_msi(uintptr_t intr_domain, uint16_t intr_src, uint16_t hart_index, uint8_t guest_index, uint16_t eiid)
{
    if (intr_src == 0) {
        return;
    }

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;
    eiid &= 0x7FFUL;

    uint32_t *target = (uint32_t *) (intr_domain + APLIC_TARGET_BASE);
    target += (intr_src - 1);
    *target = (uint32_t) hart_index << APLIC_TARGET_HART_IDX_SHIFT;
    *target |= ((uint32_t) guest_index << APLIC_TARGET_GUEST_IDX_SHIFT);
    *target |= (uint32_t) eiid;

    uint32_t *setipnum = (uint32_t *) (M_INTR_DOMAIN + APLIC_SETIPNUM);
    *setipnum = (uint32_t) intr_src;
}

void aplic_send_Nmsi(uintptr_t intr_domain, uint16_t intr_src, uint16_t hart_index, uint8_t guest_index, uint32_t bit_mask)
{
    /* intr_src must is multiple of 32 and smaller than intr_src */
    if ((intr_src % 32 != 0) || (intr_src > 1023)) {
        return;
    }

    hart_index &= 0x3FFFUL;
    guest_index &= 0x3FUL;

    uint32_t *target = (uint32_t *) (intr_domain + APLIC_TARGET_BASE);
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

    uint32_t *setip = (uint32_t *) (M_INTR_DOMAIN + APLIC_SETIP_BASE);
    setip += (intr_src >> 5);
    *setip = bit_mask;
}

void aplic_conf_sourcecfg(uintptr_t intr_domain, uint16_t intr_src, bool D, uint16_t child_index_or_sm)
{
    uint32_t *sourcecfg = (uint32_t *) (intr_domain + APLIC_SOURCECFG_BASE);
    sourcecfg += intr_src;

    /* As we are using MSI, SM either detached or inactive */
    if (D) {
        *sourcecfg |= (uint32_t) APLIC_SOURCECFG_D;
        *sourcecfg |= (uint32_t) (child_index_or_sm & APLIC_SOURCECFG_CHILDIDX_MASK);
    } else {
        // TODO: Add check to ensure child_index_or_sm is 0 or 1
        // otherwise set to inactive
        *sourcecfg &= (uint32_t) ~APLIC_SOURCECFG_D;
        *sourcecfg |= (uint32_t) (child_index_or_sm & APLIC_SOURCECFG_SM_MASK);
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
