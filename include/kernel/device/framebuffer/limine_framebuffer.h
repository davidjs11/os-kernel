/*   include/kernel/device/video/limine_framebuffer.h   */

/* Limine framebuffer driver header. */

#ifndef KERNEL_API_DEVICE_VIDEO_LIMINE_FRAMEBUFFER_H
#define KERNEL_API_DEVICE_VIDEO_LIMINE_FRAMEBUFFER_H

/*
 * @brief Register a PL011 device given its base address in MMIO.
 *
 * @param pl011_base Base address of the PL011 to register.
 *
 * @return A pointer to the serial device struct.
 */
framebuffer_device_t *limine_framebuffer_register(void *framebuffer_addr);

#endif /* KERNEL_API_DEVICE_VIDEO_LIMINE_FRAMEBUFFER_H */
