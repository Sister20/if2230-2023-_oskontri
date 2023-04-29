// #include "lib-header/keyboard.h"
#include "keyboard.h"
#include "lib-header/portio.h"
#include "lib-header/framebuffer.h"
#include "lib-header/stdmem.h"



static struct KeyboardDriverState keyboard_state = {
  .read_extended_mode = FALSE,
  .keyboard_input_on = TRUE,
  .buffer_index = 0,
  .row = 0,
  .col[0] = 0,
  .keyboard_buffer[0] = '\0'
};


const char keyboard_scancode_1_to_ascii_map[256] = {
      0, 0x1B, '1', '2', '3', '4', '5', '6',  '7', '8', '9',  '0',  '-', '=', '\b', '\t',
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '[',  ']', '\n',   0,  'a',  's',
    'd',  'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',  'z', 'x',  'c',  'v',
    'b',  'n', 'm', ',', '.', '/',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0, '-',    0,    0,   0,  '+',    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,

      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
};

void keyboard_state_activate(void){
  keyboard_state.keyboard_input_on = TRUE;
}

void keyboard_state_deactivate(void){
  keyboard_state.keyboard_input_on = FALSE;
}

void get_keyboard_buffer(char *buf){
  for(int i = 0 ; i < KEYBOARD_BUFFER_SIZE; i++){
    buf[i] = keyboard_state.keyboard_buffer[i];
  }
}

bool is_keyboard_blocking(void){
  return keyboard_state.keyboard_input_on;
}

void keyboard_isr(uint32_t len){
  framebuffer_write(20,20, 'a' + len, 0x0, 0xFF);
  if(!keyboard_state.keyboard_input_on){
    keyboard_state.buffer_index = len;
  }

  else {
    
    uint8_t  scancode    = in(KEYBOARD_DATA_PORT);
    char     mapped_char = keyboard_scancode_1_to_ascii_map[scancode];
    // bool     lastpresskey = FALSE;
    // uint8_t lastpresskey = 0;
    // TODO : Implement scancode processing
    
              if (mapped_char == '\b'){
                if (keyboard_state.col[keyboard_state.row] > 0){
                  keyboard_state.col[keyboard_state.row]--;
                  framebuffer_set_cursor(keyboard_state.row, keyboard_state.col[keyboard_state.row] + len);
                  framebuffer_write(keyboard_state.row, keyboard_state.col[keyboard_state.row] + len, ' ', 0x0, 0x0);
                }
                else {
                  if (keyboard_state.row > 0){
                    keyboard_state.row--;
                    framebuffer_set_cursor(keyboard_state.row, keyboard_state.col[keyboard_state.row] + len);
                    framebuffer_write(keyboard_state.row, keyboard_state.col[keyboard_state.row] + len, ' ', 0x0, 0x0);
                  }
                }
              }
              else if (mapped_char == '\n'){
                keyboard_state.row++;
                keyboard_state.col[keyboard_state.row] = 0;
                framebuffer_set_cursor(keyboard_state.row, keyboard_state.col[keyboard_state.row] + len);
                keyboard_state_deactivate();
              }
              else if (mapped_char != 0){
                framebuffer_write(keyboard_state.row,keyboard_state.col[keyboard_state.row] + len,mapped_char,0x0F,0x000);
                keyboard_state.buffer_index++;
                keyboard_state.col[keyboard_state.row]++;
                if(keyboard_state.col[keyboard_state.row] == 81){

                  keyboard_state.row++;
                }
            // fb_putc(mapped_char);
                framebuffer_set_cursor(keyboard_state.row,keyboard_state.col[keyboard_state.row] + len);
              }
        }
            pic_ack(IRQ_KEYBOARD);
            // return;
          }
        
      
    // }
//   }
// }


//=================================================================

// void keyboard_isr(void)
// {
//     // Read scancode from keyboard data port
//     uint8_t scancode = inb(KEYBOARD_DATA_PORT);

//     // Check if scancode is extended
//     if (scancode == EXTENDED_SCANCODE_BYTE) {
//         keyboard_state.read_extended_mode = TRUE;
//         return;
//     }

//     // Check if scancode is in extended mode
//     if (keyboard_state.read_extended_mode) {
//         scancode |= EXTENDED_SCANCODE_BYTE;
//         keyboard_state.read_extended_mode = FALSE;
//     }

//     // Convert scancode to ASCII character
//     char ascii_char = scancode_to_ascii(scancode);

//     // Check if ASCII character is printable
//     if (ascii_char != 0) {
//         // Store printable character in keyboard buffer
//         keyboard_state.keyboard_buffer[keyboard_state.buffer_index] = ascii_char;

//         // Increment buffer index
//         keyboard_state.buffer_index++;

//         // Check if buffer is full or enter key is pressed
//         if (keyboard_state.buffer_index >= KEYBOARD_BUFFER_SIZE || ascii_char == '\n') {
//             // Disable keyboard input
//             keyboard_state.keyboard_input_on = FALSE;
//         }
//     }
// }