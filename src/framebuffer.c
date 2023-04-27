
#include "lib-header/stdmem.h"
#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/framebuffer.h"

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    uint16_t offset = (row * 80 + col) * 2;
    memset(MEMORY_FRAMEBUFFER + offset, c, 1);
    memset(MEMORY_FRAMEBUFFER + offset + 1, (bg << 4) | fg, 1);
}

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    uint16_t position = r * 80 + c;

    out(CURSOR_PORT_CMD, 0x0F);
    out(CURSOR_PORT_DATA, (uint8_t) (position & 0xFF));
    out(CURSOR_PORT_CMD, 0x0E);
    out(CURSOR_PORT_DATA, (uint8_t) ((position >> 8) & 0xFF));
}

void framebuffer_clear(void) {
    memset(MEMORY_FRAMEBUFFER, 0x00, 80 * 25 * 2);
    for (uint8_t row = 0; row < 25; row++) {
        for (uint8_t col = 0; col < 80; col++) {
            framebuffer_write(row, col, ' ', 0x07, 0x00);
        }
    }
}

void puts(char *str, uint32_t fg, uint32_t bg) {
    uint8_t current_row = 0, current_col = 0;
    while (*str) {
        if (*str == '\n') {
            current_row++;
            current_col = 0;
            str++;
        } else {
            framebuffer_write(current_row, current_col, *str++, (uint8_t) fg, (uint8_t) bg);
            current_col++;
        }
        if (current_col == 79) {
            current_row++;
            current_col = 0;
        }
        if (current_row == 24) {
            if (current_col < 79) {
                current_col++;
            } else {
                current_row = 0;
                current_col = 0;
            }
        }
        framebuffer_set_cursor(current_row, current_col);
    }
}
