/*   device/framebuffer/limine_framebuffer.c   */

/* Limine framebuffer driver implementation. */

#include <kernel/device/framebuffer/framebuffer.h>
#include <kernel/libc/string.h>

#define LIMINE_FRAMEBUFFER_MAX_INSTANCES 1

typedef struct uart16550_config {
    uint16_t port;
} uart16550_config_t;

/* Extracted from limine.h */
struct limine_video_mode {
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

/* Extracted from limine.h */
typedef struct limine_framebuffer {
    void     *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
    uint8_t  unused[7];
    uint64_t edid_size;
    void     *edid;

    /* Response revision 1 */
    uint64_t mode_count;
    struct limine_video_mode **modes;
} limine_framebuffer_t;

static framebuffer_info_t   limine_framebuffer_info;
static framebuffer_device_t limine_framebuffer_device;

void limine_framebuffer_init(void *data)
{
    framebuffer_info_t *fb = (framebuffer_info_t *) data;

    /* Clear screen to black on init. */
    uint64_t total_rows = fb->height;
    uint64_t row_bytes  = fb->pitch;

    uint8_t *row = (uint8_t *) fb->addr;
    for (uint64_t y = 0; y < total_rows; y++) {
        memset(row, 0, row_bytes);
        row += row_bytes;
    }
}

uint32_t limine_framebuffer_get_pixel(void *data, uint64_t x, uint64_t y)
{
    framebuffer_info_t *fb = (framebuffer_info_t *) data;

    uint32_t *pixel = (uint32_t *)(
        (uint8_t *) fb->addr + y * fb->pitch + x * (fb->bpp / 8)
    );
    return *pixel;
}

void limine_framebuffer_set_pixel(
        void *data,
        uint64_t x,
        uint64_t y,
        uint32_t color)
{
    framebuffer_info_t *fb = (framebuffer_info_t *) data;

    /* Pitch is in bytes; Each pixel is bpp/8 bits. */
    uint32_t *pixel = (uint32_t *)(
        (uint8_t *) fb->addr + y * fb->pitch + x * (fb->bpp / 8)
    );
    *pixel = color;
}

framebuffer_device_t *limine_framebuffer_register(void *framebuffer_addr)
{
    limine_framebuffer_t *fb = (limine_framebuffer_t *) framebuffer_addr;
    limine_framebuffer_info = (framebuffer_info_t) {
        .addr   = fb->address,
        .width  = fb->width,
        .height = fb->height,
        .pitch  = fb->pitch,
        .bpp    = fb->bpp,
    };

    limine_framebuffer_device = (framebuffer_device_t) {
        .init       = limine_framebuffer_init,
        .get_pixel  = limine_framebuffer_get_pixel,
        .set_pixel  = limine_framebuffer_set_pixel,
        .data       = &limine_framebuffer_info
    };
    return &limine_framebuffer_device;
}
