#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "framebuffer.c"
#include "lib-header/kernel_loader.h"

void kernel_setup(void) {
    framebuffer_clear();
    framebuffer_write(3, 8,  'A', 0, 0xF);
    framebuffer_write(3, 9,  'N', 0, 0xF);
    framebuffer_write(3, 10, 'G', 0, 0xF);
    framebuffer_write(3, 11, 'G', 0, 0xF);
    framebuffer_write(3, 12, 'N', 0, 0xF);
    framebuffer_write(3, 13, 'T', 0, 0xF);
    framebuffer_write(3, 14, 'R', 0, 0xF);
    framebuffer_write(3, 15, 'I', 0, 0xF);
    framebuffer_set_cursor(3, 10);
    while (TRUE);
}