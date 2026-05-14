/*   device/framebuffer/framebuffer.c   */
#include <kernel/device/framebuffer/framebuffer.h>

static framebuffer_device_t *framebuffer_devices[FRAMEBUFFER_MAX_DEVICES];
static framebuffer_info_t   *framebuffer_info[FRAMEBUFFER_MAX_DEVICES];
static uint8_t               framebuffer_count = 0;

/* Default framebuffer handle. */
static framebuffer_handle_t  framebuffer_default = FRAMEBUFFER_MAX_DEVICES;

framebuffer_handle_t framebuffer_register(framebuffer_device_t *dev)
{
    if (framebuffer_count >= FRAMEBUFFER_MAX_DEVICES) {
        return FRAMEBUFFER_MAX_DEVICES; /* Invalid handle. */
    }

    framebuffer_devices[framebuffer_count] = dev;
    framebuffer_info[framebuffer_count] = (framebuffer_info_t *) dev->data;
    return framebuffer_count++;
}

void framebuffer_init(framebuffer_handle_t fb)
{
    if (fb >= framebuffer_count || framebuffer_devices[fb] == NULL) {
        return;
    }

    framebuffer_device_t *dev = framebuffer_devices[fb];
    if (dev->init != NULL) {
        dev->init(dev->data);
    }
}

void framebuffer_set_default_handle(framebuffer_handle_t framebuffer_handle)
{
    if (framebuffer_handle >= 0 && framebuffer_handle < framebuffer_count) {
        framebuffer_default = framebuffer_handle;
    }
}

bool framebuffer_has_default(void)
{
    return framebuffer_default < framebuffer_count
        && framebuffer_info[framebuffer_default] != NULL;
}

uint32_t framebuffer_get_pixel_from(
        framebuffer_handle_t fb,
        uint64_t x,
        uint64_t y)
{
    if (fb >= framebuffer_count || framebuffer_devices[fb] == NULL) {
        return 0;
    }

    framebuffer_device_t *dev = framebuffer_devices[fb];
    if (dev->get_pixel == NULL) {
        return 0;
    }

    return dev->get_pixel(dev->data, x, y);
}

uint32_t framebuffer_get_pixel(uint64_t x, uint64_t y)
{
    return framebuffer_get_pixel_from(framebuffer_default, x, y);
}

void framebuffer_set_pixel_to(
    framebuffer_handle_t fb,
    uint64_t x,
    uint64_t y,
    uint32_t color)
{
    if (fb >= framebuffer_count || framebuffer_devices[fb] == NULL) {
        return;
    }

    framebuffer_device_t *dev = framebuffer_devices[fb];
    if (dev->set_pixel != NULL) {
        dev->set_pixel(dev->data, x, y, color);
    }
}

void framebuffer_set_pixel(uint64_t x, uint64_t y, uint32_t color)
{
    framebuffer_set_pixel_to(framebuffer_default, x, y, color);
}

uint64_t framebuffer_get_width_from(framebuffer_handle_t fb)
{
    if (fb < framebuffer_count && framebuffer_info[fb] != NULL) {
        return framebuffer_info[fb]->width;
    }
    return 0;
}

uint64_t framebuffer_get_width(void)
{
    return framebuffer_get_width_from(framebuffer_default);
}

uint64_t framebuffer_get_height_from(framebuffer_handle_t fb)
{
    if (fb < framebuffer_count && framebuffer_info[fb] != NULL) {
        return framebuffer_info[fb]->height;
    }
    return 0;
}

uint64_t framebuffer_get_height(void)
{
    return framebuffer_get_height_from(framebuffer_default);
}
