#include <stdint.h>
#include <stdbool.h>

/**
 * @name IMSIC Interrupt File addresses
 * @{
 */
#define MACHINE_IF_ADDR 0x24000000
#define SUPERVISOR_IF_ADDR 0x28000000
/** @} */

/**
 * @name APLIC Interrupt Domain addresses
 * @note These are subjected to change when using different domain
 * configurations
 * @{
 */
#define ROOT_MIRQ_DOMAIN (0xc000000)
#define L0_MIRQ_DOMAIN (ROOT_MIRQ_DOMAIN + 0x4000)
#define L0_SIRQ_DOMAIN (0xd000000)
#define L1_MIRQ_DOMAIN (L0_MIRQ_DOMAIN + 0x4000)
#define L1_SIRQ_DOMAIN (L0_SIRQ_DOMAIN + 0x4000)
/** @} */

/**
 * @name APLIC Configuration Macros
 * APLIC MMR offsets, default values, and masks
 * @{
 */
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
/** @} */

/**
 * @name Maximum IRQ Sources based on QEMU
 */
#define IRQ_SRC_MAX                    0x60

/**
 * @brief Initialize N irq sources for aplic
 *
 * @param irq_src_count Number of irq sources to initialize
 */
void aplic_init(uint32_t irq_src_count);

/**
 * @brief Send irq_src as MSI to delegated IMSIC
 *
 * @param irq_src Interrupt source number
 * @param hart_index Target hart id
 * @param guest_index If hypervisor implemented, the target guest interrupt
 *                    file
 * @param eiid target[i] registers external interrupt ID shown when interrupt
 *             serviced.
 */
void aplic_send_msi(uint32_t irq_src, uint32_t hart_index,
                    uint32_t guest_index, uint32_t eiid);

/**
 * @brief Send N irq_src as MSI to delegated IMSIC
 *
 * @param base_irq_src Base interrupt source number
 * @param irq_count Number of interrupt requests to send. Interrupts in range
 *                  [base_irq_src, base_irq_src + irq_count) are sent as MSIs
 * @param hart_index Target hart id
 * @param guest_index If hypervisor implemented, the target guest interrupt
 *                    file
 *
 * @note eiid will be the same as the interrupt source number.
 * @warning the function only works for MSIs in the same domain. If MSIs of
 *          overlapping domains are used, only the domain containing
 *          base_irq_src would be configured.
 */
void aplic_send_Nmsi(uint32_t base_irq_src, uint32_t irq_count,
                     uint32_t hart_index, uint32_t guest_index);

/**
 * @brief Modify sourcecfg register for irq_src in irq_domain
 *
 * @param irq_src Base interrupt source number
 * @param irq_domain Interrupt Domain address
 * @param child_index_or_sm If D is true, field is read as child_index
 *                          of the interrupt domain irq is to be delegated.
 *                          If D is false, field is read as source_mode.
 * @param D Delegate interrupt
 *
 * @note eiid will be the same as the interrupt source number.
 */
void aplic_conf_sourcecfg(uint32_t irq_src, uintptr_t irq_domain,
                          uint32_t child_index_or_sm, bool D);

/**
 * @brief Delegate N interrupts to child domain "child_index"
 *
 * @param base_irq_src Base interrupt source number
 * @param irq_count Number of interrupt requests to send. Interrupts in range
 *                  [base_irq_src, base_irq_src + irq_count) are sent as MSIs
 * @param irq_domain Interrupt Domain address
 * @param child_index Child_index of the interrupt domain irq is to be delegated
 * @param D Delegate interrupt
 *
 * @note eiid will be the same as the interrupt source number.
 */
void aplic_Nirq_delegate(uint32_t base_irq_src, uint32_t irq_count,
                         uint32_t child_index);

/**
 * @brief Enables interrupt in irq_domain based on bit_mask
 *
 * @param irq_domain Interrupt Domain address
 * @param k Index of setie[k] MMR to set
 * @param bit_mask 32-bit mask to set the 32 bit MMR setie[k]
 */
void aplic_setie(uintptr_t irq_domain, uint32_t k, uint32_t bit_mask);

/**
 * @brief Disables interrupt in irq_domain based on bit_mask
 *
 * @param irq_domain Interrupt Domain address
 * @param k Index of clrie[k] MMR to set
 * @param bit_mask 32-bit mask to set the 32 bit MMR clrie[k]
 */
void aplic_clrie(uintptr_t irq_domain, uint32_t k, uint32_t bit_mask);
