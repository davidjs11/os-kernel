/*   include/kernel/device/fb/fb.h   */

/* Framebuffer driver API. */

#ifndef KERNEL_API_DEVICE_FRAMEBUFFER_H
#define KERNEL_API_DEVICE_FRAMEBUFFER_H

#include <kernel/types.h>

#define FRAMEBUFFER_MAX_DEVICES 4

/* 
 * Each framebuffer device will have a handle (identifier) used by different
 * parts of the kernel. This will enable output to different framebffers.
 */
typedef uint8_t framebuffer_handle_t;

typedef struct framebuffer_info {
    void     *addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
} framebuffer_info_t;

typedef struct framebuffer_device {
    void      (*init)(void *data);
    uint32_t  (*get_pixel)(void *data, uint64_t x, uint64_t y);
    void      (*set_pixel)(void *data, uint64_t x, uint64_t y, uint32_t color);
    void      *data;
} framebuffer_device_t;

/*
 * @brief Register a new framebuffer.
 *
 * @param dev Framebuffer device to register.
 *
 * @return A handle for the new registered framebuffer.
 */
framebuffer_handle_t framebuffer_register(framebuffer_device_t *dev);

/*
 * @brief Initialize a framebuffer.
 *
 * @param fb Framebuffer handle of the framebuffer to initialize.
 */
void framebuffer_init(framebuffer_handle_t fb);

/*
 * @brief Set a certain framebuffer as the default.
 *
 * @param framebuffer_handle Handle of the framebuffer to set as default.
 */
void framebuffer_set_default_handle(framebuffer_handle_t framebuffer_handle);

/*
 * @brief Get the color value of a certain pixel in a framebuffer.
 *
 * @param fb Framebuffer handle to get the pixel color value from.
 * @param x  X coordinate.
 * @param y  Y coordinate.
 *
 * @return The color value of the pixel.
 */
uint32_t framebuffer_get_pixel_from(
        framebuffer_handle_t fb,
        uint64_t x,
        uint64_t y);

/*
 * @brief Get the color value of a certain pixel from the default framebuffer.
 *
 * @param x  X coordinate.
 * @param y  Y coordinate.
 *
 * @return The color value of the pixel.
 */
uint32_t framebuffer_get_pixel(uint64_t x, uint64_t y);

/*
 * @brief Set the color value of a certain pixel in a framebuffer.
 *
 * @param fb     Framebuffer handle to get the pixel color value from.
 * @param x      X coordinate.
 * @param y      Y coordinate.
 * @param color  Color value to set on the pixel.
 */
void framebuffer_set_pixel_to(
    framebuffer_handle_t fb,
    uint64_t x,
    uint64_t y,
    uint32_t color);

/*
 * @brief Set the color value of a certain pixel in the default framebuffer.
 *
 * @param x      X coordinate.
 * @param y      Y coordinate.
 * @param color  Color value to set on the pixel.
 */
void framebuffer_set_pixel(uint64_t x, uint64_t y, uint32_t color);

/*
 * @brief Get the width of a framebuffer.
 *
 * @param fb Framebuffer handle of the framebuffer to get the width from.
 *
 * @return Width of the framebuffer.
 */
uint64_t framebuffer_get_width_from(framebuffer_handle_t fb);

/*
 * @brief Get the width of the default framebuffer.
 *
 * @return Width of the default framebuffer.
 */
uint64_t framebuffer_get_width(void);

/*
 * @brief Get the height of a framebuffer.
 *
 * @param fb Framebuffer handle of the framebuffer to get the height from.
 *
 * @return Height of the framebuffer.
 */
uint64_t framebuffer_get_height_from(framebuffer_handle_t fb);

/*
 * @brief Get the height of the default framebuffer.
 *
 * @return Height of the default framebuffer.
 */
uint64_t framebuffer_get_height(void);

#endif /* KERNEL_API_DEVICE_FRAMEBUFFER_H */
