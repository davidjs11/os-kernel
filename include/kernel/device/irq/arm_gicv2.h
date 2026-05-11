/*   include/kernel/device/irq/arm_gicv2.h   */

/* ARM's Generic Interrupt Controller (GIC) version 2 driver header. */

#ifndef KERNEL_API_DEVICE_IRQ_ARM_GICV2_H
#define KERNEL_API_DEVICE_IRQ_ARM_GICV2_H

void arm_gicv2_config(uint64_t gicd_base, uint64_t gicc_base);

extern irq_device_t irq_device_arm_gicv2;
void arm_gicv2_trigger(uint32_t intid);

#endif /* KERNEL_API_DEVICE_IRQ_ARM_GICV2_H */
