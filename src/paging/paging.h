#ifndef _PAGING_H
#define _PAGING_H

<<<<<<< HEAD
#include "../lib-header/stdtype.h"

#define PAGE_ENTRY_COUNT 1024
#define PAGE_FRAME_SIZE (4*1024*1024)
=======
#include "stdtype.h"

#define PAGE_ENTRY_COUNT 1024
#define PAGE_FRAME_SIZE  (4*1024*1024)
>>>>>>> trial

// Operating system page directory, using page size PAGE_FRAME_SIZE (4 MiB)
extern struct PageDirectory _paging_kernel_page_directory;




/**
<<<<<<< HEAD
* Page Directory Entry Flag, only first 8 bit
*
* @param present_bit Indicate whether this entry is exist or not
* @param write_bit Indicate whether this entry is read-only or read-write
* @param user_supervisor Indicate whether this entry is accessible by user or supervisor
* @param write_through Indicate whether this entry is write-through or write-back
* @param cache_disable Indicate whether this entry is cacheable or not
* @param accessed Indicate whether this entry is accessed or not
* @param dirty Indicate whether this entry is dirty or not
* @param use_pagesize_4_mb Indicate whether this entry is referencing to page table or not
*
*/
struct PageDirectoryEntryFlag {
// First 8 bit flags of PDE 4MB page
uint8_t present_bit : 1;
uint8_t write_bit : 1;
uint8_t user_supervisor : 1;
uint8_t write_through : 1;
uint8_t cache_disable : 1;
uint8_t accessed : 1;
uint8_t dirty : 1;
uint8_t use_pagesize_4_mb : 1;

} __attribute__((packed));

/**
* Page Directory Entry, for page size 4 MB.
* Check Intel Manual 3a - Ch 4 Paging - Figure 4-4 PDE: 4MB page
*
* @param flag Contain 8-bit page directory entry flag
* @param global_page Is this page translation global (also cannot be flushed)
* @param ignored
* @param pat Page Attribute Table (PAT) index
* @param lower_address Lower 8 bit (39:32) of physical address
* @param reserved Reserved bit
* @param higher_address Higher 10 bit (31:22) of physical address
* ...
*/
struct PageDirectoryEntry {
struct PageDirectoryEntryFlag flag;

// Bit 9 - 31 of PDE 4MB page
uint16_t global_page : 1;
uint16_t ignored : 3;
uint16_t pat : 1;
uint16_t bits : 4;
uint16_t lower_address : 5;
uint16_t higher_address : 10;

// Note: Use uint16_t + bitfield here, Do not use uint8_t
} __attribute__((packed));

/**
* Page Directory, contain array of PageDirectoryEntry.
* Note: This data structure not only can be manipulated by kernel,
* MMU operation, TLB hit & miss also affecting this data structure (dirty, accessed bit, etc).
* Warning: Address must be aligned in 4 KB (listed on Intel Manual), use __attribute__((aligned(0x1000))),
* unaligned definition of PageDirectory will cause triple fault
*
* @param table Fixed-width array of PageDirectoryEntry with size PAGE_ENTRY_COUNT
*/
struct PageDirectory {
// TODO : Implement
struct PageDirectoryEntry table[PAGE_ENTRY_COUNT];
} __attribute__((packed));

/**
* Containing page driver states
*
* @param last_available_physical_addr Pointer to last empty physical addr (multiple of 4 MiB)
*/
struct PageDriverState {
uint8_t *last_available_physical_addr;
=======
 * Page Directory Entry Flag, only first 8 bit
 * 
 * @param present_bit       Indicate whether this entry is exist or not
 * ...
 */
struct PageDirectoryEntryFlag {
    uint8_t present_bit        : 1;
    uint8_t read_write_bit     : 1;
    uint8_t user_supervisor_bit: 1;
    uint8_t write_through_bit  : 1;
    uint8_t cache_disable_bit  : 1;
    uint8_t accessed_bit       : 1;
    uint8_t dirty_bit          : 1;
    uint8_t page_size_bit      : 1; 
} __attribute__((packed));

/**
 * Page Directory Entry, for page size 4 MB.
 * Check Intel Manual 3a - Ch 4 Paging - Figure 4-4 PDE: 4MB page
 * 
 * @param flag            Contain 8-bit page directory entry flag
 * @param global_page     Is this page translation global (also cannot be flushed)
 * ...
 * Note:
 * - Assume "Bits 39:32 of address" (higher_address) is 8-bit and Reserved is 1
 * - "Bits 31:22 of address" is called lower_address in kit
 */
struct PageDirectoryEntry {
    struct PageDirectoryEntryFlag flag;
    uint16_t global_page    : 1;
    uint16_t reserved       : 2;
    uint16_t accessed       : 1;
    uint16_t cache_disabled : 1;
    uint16_t write_through  : 1;
    uint16_t user_supervisor: 1;
    uint16_t read_write     : 1;
    uint16_t present        : 1;
    uint32_t lower_address  : 22;
    uint8_t higher_address  : 8;
} __attribute__((packed));

/**
 * Page Directory, contain array of PageDirectoryEntry.
 * Note: This data structure not only can be manipulated by kernel, 
 *   MMU operation, TLB hit & miss also affecting this data structure (dirty, accessed bit, etc).
 * Warning: Address must be aligned in 4 KB (listed on Intel Manual), use __attribute__((aligned(0x1000))), 
 *   unaligned definition of PageDirectory will cause triple fault
 * 
 * @param table Fixed-width array of PageDirectoryEntry with size PAGE_ENTRY_COUNT
 */
struct PageDirectory {
    struct PageDirectoryEntry table[PAGE_ENTRY_COUNT]  ;
} __attribute__((packed, aligned(0x1000)));

/**
 * Containing page driver states
 * 
 * @param last_available_physical_addr Pointer to last empty physical addr (multiple of 4 MiB)
 */
struct PageDriverState {
    uint8_t *last_available_physical_addr;
>>>>>>> trial
} __attribute__((packed));





/**
<<<<<<< HEAD
* update_page_directory,
* Edit _paging_kernel_page_directory with respective parameter
*
* @param physical_addr Physical address to map
* @param virtual_addr Virtual address to map
* @param flag Page entry flags
*/
void update_page_directory(void *physical_addr, void *virtual_addr, struct PageDirectoryEntryFlag flag);

/**
* flush_single_tlb,
* invalidate page that contain virtual address in parameter
*
* @param virtual_addr Virtual address to flush
*/
void flush_single_tlb(void *virtual_addr);

/**
* Allocate user memory into specified virtual memory address.
* Multiple call on same virtual address will unmap previous physical address and change it into new one.
*
* @param virtual_addr Virtual address to be mapped
* @return int8_t 0 success, -1 for failed allocation
*/
=======
 * update_page_directory_entry,
 * Edit _paging_kernel_page_directory with respective parameter
 * 
 * @param physical_addr Physical address to map
 * @param virtual_addr  Virtual address to map
 * @param flag          Page entry flags
 */
void update_page_directory_entry(void *physical_addr, void *virtual_addr, struct PageDirectoryEntryFlag flag);

/**
 * flush_single_tlb, 
 * invalidate page that contain virtual address in parameter
 * 
 * @param virtual_addr Virtual address to flush
 */
void flush_single_tlb(void *virtual_addr);

/**
 * Allocate user memory into specified virtual memory address.
 * Multiple call on same virtual address will unmap previous physical address and change it into new one.
 * 
 * @param  virtual_addr Virtual address to be mapped
 * @return int8_t       0 success, -1 for failed allocation
 */
>>>>>>> trial
int8_t allocate_single_user_page_frame(void *virtual_addr);

#endif