/*   kernel/kmain.c   */

/* Kernel main entry point. */

#include <arch_init.h>
#include <limine_requests.h>

/*
 * @brief Fill the screen with a color.
 *
 * @param framebuffer The Limine framebuffer struct.
 * @param color The color to fill the screen with.
 */
void print_color(struct limine_framebuffer *framebuffer, uint32_t color) {
    uint32_t *fb = framebuffer->address;
    
    uint64_t width = framebuffer->width;
    uint64_t height = framebuffer->height;

    for (uint64_t i = 0; i < (height-1)*width; i++) {
        fb[(framebuffer->pitch / 4) + i] = color;
    }
}

/*
 * @brief Kernel entry point.
 */
void kmain(void) {
    arch_init();

    while (1) {
        for (uint8_t x = 0; x < 0xff; x++) {
            print_color(
                limine_framebuffer_request.response->framebuffers[0],
                0x000000 | (x << 16)
            );
        }
    }

    return;
}
