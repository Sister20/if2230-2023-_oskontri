#include "lib-header/stdtype.h"
#include "lib-header/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to GDT definition in Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
            {
                // Entry 0 : Null Descriptor
                .segment_low = 0,
                .base_low = 0,
                .base_mid = 0,
                .type_bit = 0,
                .non_system = 0,
                .dpl = 0,
                .p = 0,
                .segment_limit = 0,
                .avl = 0,
                .l = 0,
                .d_b = 0,
                .g = 0,
                .base_high = 0
                
            },
            {
                // Entry 1 : Kernel Code Segment
                .segment_low = 0xFFFF,
                .base_low = 0,
                .base_mid = 0,
                .type_bit = 0xA,
                .non_system = 1,
                .dpl = 0,
                .p = 1,
                .segment_limit = 0xF,
                .avl = 0,
                .l = 0,
                .d_b = 1,
                .g = 1,
                .base_high = 0

            },
            {
                // Entry 2 : Kernel Data Segment
                .segment_low = 0xFFFF,
                .base_low = 0,
                .base_mid = 0,
                .type_bit = 0x2,
                .non_system = 1,
                .dpl = 0,
                .p = 1,
                .segment_limit = 0xF,
                .avl = 0,
                .l = 0,
                .d_b = 1,
                .g = 1,
                .base_high = 0
            }
    }
};

/**
 * _gdt_gdtr, predefined system GDTR. 
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    // TODO : Implement, this GDTR will point to global_descriptor_table. 
    //        Use sizeof operator
    .size = sizeof(global_descriptor_table) -1,
    .address = &global_descriptor_table,

    
};

