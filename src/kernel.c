#include "lib-header/portio.h"
#include "lib-header/stdtype.h"
#include "lib-header/stdmem.h"
#include "lib-header/gdt.h"
#include "lib-header/framebuffer.h"
#include "interrupt/idt.h"
#include "lib-header/kernel_loader.h"
#include "interrupt/interrupt.h"
#include "keyboard/keyboard.h"
#include "filesystem/fat32.h"

void kernel_setup(void) {
    enter_protected_mode(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    create_fat32();
    keyboard_state_activate();

    struct ClusterBuffer cbuf[5];
    for (uint32_t i = 0; i < 5; i++)
        for (uint32_t j = 0; j < CLUSTER_SIZE; j++)
            cbuf[i].buf[j] = i + 'a';

    struct FAT32DriverRequest request = {
        .buf                   = cbuf,
        .name                  = "ikanaide",
        .ext                   = "uwu",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0,
    } ;

    write(request);  // Create folder "ikanaide"




    // enter_protected_mode(&_gdt_gdtr);
    // pic_remap();
    // activate_keyboard_interrupt();
    // initialize_idt();
    // framebuffer_clear();
    // framebuffer_set_cursor(0, 0);
    // // __asm__("int $0x4");
    // while (TRUE);
    // keyboard_state_activate();
    // // __asm__("int $0x4”);
    // // while (TRUE);


    // enter_protected_mode(&_gdt_gdtr);
    // framebuffer_clear();
    // framebuffer_write(9, 13,  ' ', 0, 0xF);
    // framebuffer_write(10, 14,  ' ', 0, 0xF);
    // framebuffer_write(11, 15,  ' ', 0, 0xF);
    // framebuffer_write(12, 16,  ' ', 0, 0xF);
    // framebuffer_write(12, 17,  ' ', 0, 0xF);
    // framebuffer_write(12, 18,  ' ', 0, 0xF);
    // framebuffer_write(12, 19,  ' ', 0, 0xF);
    // framebuffer_write(11, 20,  ' ', 0, 0xF);
    // framebuffer_write(10, 21,  ' ', 0, 0xF);
    // framebuffer_write(9, 22,  ' ', 0, 0xF);
    // framebuffer_write(8, 23,  ' ', 0, 0xF);
    // framebuffer_write(8, 24,  ' ', 0, 0xF);
    // framebuffer_write(9, 25,  ' ', 0, 0xF);
    // framebuffer_write(10, 26,  ' ', 0, 0xF);
    // framebuffer_write(11, 27,  ' ', 0, 0xF);
    // framebuffer_write(12, 28,  ' ', 0, 0xF);
    // framebuffer_write(12, 29,  ' ', 0, 0xF);
    // framebuffer_write(12, 30,  ' ', 0, 0xF);
    // framebuffer_write(12, 31,  ' ', 0, 0xF);
    // framebuffer_write(11, 32,  ' ', 0, 0xF);
    // framebuffer_write(10, 33,  ' ', 0, 0xF);
    // framebuffer_write(9, 34,  ' ', 0, 0xF);
    // framebuffer_write(8, 35,  ' ', 0, 0xF);
    // framebuffer_write(8, 13,  ' ', 0, 0xF);
    // framebuffer_write(7, 13,  ' ', 0, 0xF);
    // framebuffer_write(6, 13,  ' ', 0, 0xF);
    // framebuffer_write(5, 13,  ' ', 0, 0xF);
    // framebuffer_write(4, 14,  ' ', 0, 0xF);
    // for (int i = 15; i<50;i++){
    //     framebuffer_write(3, i, ' ', 0, 0xF);
    // }
    // framebuffer_write(4, 50,  ' ', 0, 0xF);
    // framebuffer_write(4, 51,  ' ', 0, 0xF);
    // framebuffer_write(4, 52,  ' ', 0, 0xF);
    // framebuffer_write(5, 53,  ' ', 0, 0xF);
    // framebuffer_write(5, 54,  ' ', 0, 0xF);
    // framebuffer_write(5, 55,  ' ', 0, 0xF);
    // framebuffer_write(6, 55,  ' ', 0, 0xF);
    // framebuffer_write(6, 54,  ' ', 0, 0xF);
    // framebuffer_write(6, 53,  ' ', 0, 0xF);
    // framebuffer_write(7, 53,  ' ', 0, 0xF);
    // framebuffer_write(8, 52,  ' ', 0, 0xF);
    // for (int i = 51; i>35;i--){
    //     framebuffer_write(8, i, ' ', 0, 0xF);
    // }
    // framebuffer_write(10, 14,  ' ', 0, 0xF);
    // framebuffer_write(11, 15,  ' ', 0, 0xF);
    // framebuffer_write(6, 24,  'O', 0xA, 0x0);
    // framebuffer_write(6, 25,  'S', 0xB, 0x0);
    // framebuffer_write(6, 26,  ' ', 0xC, 0x0);
    // framebuffer_write(6, 27,  'K', 0xD, 0x0);
    // framebuffer_write(6, 28,  'O', 0xE, 0x0);
    // framebuffer_write(6, 29,  'N', 0x1, 0x0);
    // framebuffer_write(6, 30,  'T', 0x2, 0x0);
    // framebuffer_write(6, 31,  'R', 0x3, 0x0);
    // framebuffer_write(6, 32,  'I', 0x4, 0x0);
    // framebuffer_set_cursor(6, 57);
    // framebuffer_set_cursor(5, 58);
    // while (TRUE);


}