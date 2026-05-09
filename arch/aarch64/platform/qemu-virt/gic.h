/*   arch/aarch64/platform/qemu-virt/gic.h   */

/* ARM Generic Interrupt Controller version 2 management API. */

#ifndef ARCH_AARCH64_GIC_H
#define ARCH_AARCH64_GIC_H

#define GICD_BASE 0x08000000UL
#define GICC_BASE 0x08010000UL

/* Distributor ports. */
#define GICD_CTLR       0x000 /* Distributor control.           */
#define GICD_TYPER      0x004 /* Interrupt controller type.     */
#define GICD_ISENABLER  0x100 /* Interrupt set-enable.          */
#define GICD_ISPENDR    0x200 /* Interrupt Set-Pending.         */
#define GICD_IPRIORITYR 0x400 /* Interrupt priority.            */
#define GICD_ITARGETSR  0x800 /* Interrupt targets (which CPU). */
#define GICD_ICFGR      0xC00 /* Interrupt configuration.       */
#define GICD_SGIR       0xF00 /* Software Generated Interrupt.  */

/* CPU interface ports. */
#define GICC_CTLR       0x000 /* CPU interface control.  */
#define GICC_PMR        0x004 /* Priority mask.          */
#define GICC_IAR        0x00C /* Interrupt acknowledge.  */
#define GICC_EOIR       0x010 /* End Of Interrupt (EOI). */

/*
 * @brief Write to the GIC Distributor.
 *
 * @param off Offset of the register to read from.
 *
 * @return The value of the parameter.
 */
uint32_t gicd_read(uint32_t off);

/*
 * @brief Write to the GIC Distributor.
 *
 * @param off Offset of the register to write to.
 * @param val Value to write.
 */
void gicd_write(uint32_t off, uint32_t val);

/*
 * @brief Write to the GIC CPU interface.
 *
 * @param off Offset of the register to read from.
 *
 * @return The value of the parameter.
 */
uint32_t gicc_read(uint32_t off);

/*
 * @brief Write to the GIC CPU interface.
 *
 * @param off Offset of the register to write to.
 * @param val Value to write.
 */
void gicc_write(uint32_t off, uint32_t val);

/*
 * @brief Initialize GIC.
 */
void gic_init(void);

/*
 * @brief Handle an interrupt (called from the IRQ handler.)
 */
void gic_handle_irq(void);

#endif /* ARCH_AARCH64_GIC_H */
