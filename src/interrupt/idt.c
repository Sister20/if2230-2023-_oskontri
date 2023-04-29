#include "idt.h"
#include "../lib-header/gdt.h"

// ISR stub for reducing code duplication
struct IDT idt={
    .table = {
        {
        .offset_low  = 0,
        .segment     = 0,
        ._reserved   = 0,
        ._r_bit_1    = 0,
        ._r_bit_2    = 0,
        .gate_32     = 0,
        ._r_bit_3    = 0,
        .privilege   = 0,
        .valid       = 0,
        .offset_high = 0
        }
    }
};
// void *isr_stub_table[ISR_STUB_TABLE_LIMIT];

// IDT table
struct IDTR _idt_idtr ={
    .limit = sizeof(struct IDT) - 1,
    .base = &idt.table[1]
};

// IDTR descriptor



/**
 * Set IDTGate with proper interrupt handler values.
 * Will directly edit global IDT variable and set values properly
 * 
 * @param int_vector       Interrupt vector to handle
 * @param handler_address  Interrupt handler address
 * @param gdt_seg_selector GDT segment selector, for kernel use GDT_KERNEL_CODE_SEGMENT_SELECTOR
 * @param privilege        Descriptor privilege level
 */
void initialize_idt(void) {
    /* TODO : 
   * Iterate all isr_stub_table,
   * Set all IDT entry with set_interrupt_gate()
   * with following values:
   * Vector: i
   * Handler Address: isr_stub_table[i]
   * Segment: GDT_KERNEL_CODE_SEGMENT_SELECTOR
   * Privilege: 0
   */
    // Set IDT entries with privilege level 0
    for (int i = 0; i < ISR_STUB_TABLE_LIMIT; i++) {
        if(i >= 0x30 && i <= 0x3F) {
            set_interrupt_gate(i, isr_stub_table[i], GDT_KERNEL_CODE_SEGMENT_SELECTOR, 0x3);
        }
        else{
            set_interrupt_gate(i, isr_stub_table[i], GDT_KERNEL_CODE_SEGMENT_SELECTOR, 0);
        }
    }

    // // Set IDT entries with privilege level 0x3
    // for (int i = 0x30; i <= 0x3F; i++) {
    //     set_interrupt_gate(i, isr_stub_table[i], GDT_KERNEL_CODE_SEGMENT_SELECTOR, 0x3);
    // }

    // Load IDTR descriptor and enable interrupts
    __asm__ volatile("lidt %0" : : "m"(_idt_idtr));
    __asm__ volatile("sti");
}

void set_interrupt_gate(uint8_t int_vector, void *handler_address, uint16_t gdt_seg_selector, uint8_t privilege) {
    struct IDTGate *idt_int_gate = &idt.table[int_vector];
    // TODO : Set handler offset, privilege & segment
    idt_int_gate->privilege   = privilege;
    idt_int_gate->offset_low  = (uint32_t)handler_address & 0xFFFF;
    idt_int_gate->offset_high = ((uint32_t)handler_address >> 16) & 0xFFFF;
    idt_int_gate->segment     = gdt_seg_selector;
    idt_int_gate->_reserved   = 0;
    

    // Target system 32-bit and flag this as valid interrupt gate
    idt_int_gate->_r_bit_1    = INTERRUPT_GATE_R_BIT_1;
    idt_int_gate->_r_bit_2    = INTERRUPT_GATE_R_BIT_2;
    idt_int_gate->_r_bit_3    = INTERRUPT_GATE_R_BIT_3;
    idt_int_gate->gate_32     = 1;
    idt_int_gate->valid   = 1;

    // // Calculate IDTGate offset
    // uint32_t offset = (uint32_t)handler_address;
    // struct IDTGate *idt_gate = &idt_table[int_vector];

    // // Setup IDTGate entry
    // idt_table[int_vector].offset_low = offset & 0xFFFF;
    // idt_table[int_vector].segment = gdt_seg_selector;
    // idt_table[int_vector]._reserved = 0;
    // idt_table[int_vector]._r_bit_1 = INTERRUPT_GATE_R_BIT_1;
    // idt_table[int_vector]._r_bit_2 = INTERRUPT_GATE_R_BIT_2;
    // idt_table[int_vector].gate_32 = 1;
    // idt_table[int_vector]._r_bit_3 = INTERRUPT_GATE_R_BIT_3;
    // idt_table[int_vector].privilege = privilege;
    // idt_table[int_vector].offset_high = (offset >> 16) & 0xFFFF;
    // idt_table[int_vector].valid = 1;
}

/**
 * Set IDT with proper values and load with lidt
 */

