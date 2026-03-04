#include <stdint.h>
#include <stdbool.h>

#define MACHINE_IF_ADDR 0x24000000
#define SUPERVISOR_IF_ADDR 0x28000000

#define M_INTR_DOMAIN 0xC000000
#define S_INTR_DOMAIN 0xD000000

#define APLIC_DOMAINCFG                0x0000
#define APLIC_DOMAINCFG_RDONLY         0x80000000
#define APLIC_DOMAINCFG_IE             (1 << 8)
#define APLIC_DOMAINCFG_DM             (1 << 2)
#define APLIC_DOMAINCFG_BE             (1 << 0)

#define APLIC_SOURCECFG_BASE           0x0004
#define APLIC_SOURCECFG_D              (1 << 10)
#define APLIC_SOURCECFG_CHILDIDX_MASK  0x000003ff
#define APLIC_SOURCECFG_SM_MASK        0x00000007
#define APLIC_SOURCECFG_SM_INACTIVE    0x0
#define APLIC_SOURCECFG_SM_DETACH      0x1

#define APLIC_MMSICFGADDR              0x1bc0
#define APLIC_MMSICFGADDRH             0x1bc4
#define APLIC_SMSICFGADDR              0x1bc8
#define APLIC_SMSICFGADDRH             0x1bcc

#define APLIC_SETIP_BASE               0x1c00
#define APLIC_SETIPNUM                 0x1cdc

#define APLIC_SETIE_BASE               0x1e00

#define APLIC_CLRIE_BASE               0x1f00

#define APLIC_TARGET_BASE              0x3004
#define APLIC_TARGET_HART_IDX_SHIFT    18
#define APLIC_TARGET_HART_IDX_MASK     0x3fff
#define APLIC_TARGET_GUEST_IDX_SHIFT   12
#define APLIC_TARGET_GUEST_IDX_MASK    0x3f
#define APLIC_TARGET_IPRIO_MASK        0xff
#define APLIC_TARGET_EIID_MASK         0x7ff

void aplic_init(uintptr_t aplic_addr, uint16_t intr_src_count);

void aplic_send_msi(uintptr_t intr_domain, uint16_t intr_src, uint16_t hart_index, uint8_t guest_index, uint16_t eiid);
void aplic_send_Nmsi(uintptr_t intr_domain, uint16_t intr_src, uint16_t hart_index, uint8_t guest_index, uint32_t bit_mask);

void aplic_conf_sourcecfg(uintptr_t intr_domain, uint16_t intr_src, bool D, uint16_t child_index_or_sm);

void aplic_setie(uintptr_t intr_domain, uint8_t k, uint32_t bit_mask);
void aplic_in_clrie(uintptr_t intr_domain, uint8_t k, uint32_t bit_mask);

