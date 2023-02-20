#include "framebuffer.h"
#include "portio.c"
#include "stdmem.c"

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    uint16_t attrib = (bg << 4) | (fg & 0x0F);
     volatile uint16_t * where;
     where = (volatile uint16_t *)0xB8000 + (col * 80 + row) ;
     *where = c | (attrib << 8);
}

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    uint16_t pos = r * 80 + c;
    out(CURSOR_PORT_CMD, 0x0F);
    out(CURSOR_PORT_DATA, (uint8_t) (pos & 0xFF));
    out(CURSOR_PORT_CMD, 0x0E);
    out(CURSOR_PORT_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

void framebuffer_clear(void) {
    for (uint8_t row = 0; row < 25; row++) {
        for (uint8_t col = 0; col < 80; col++) {
            framebuffer_write(row, col, ' ', 0x07, 0x00);
        }
    }
}