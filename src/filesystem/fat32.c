#include "lib-header/stdtype.h"
#include "fat32.h"
#include "lib-header/stdmem.h"

struct FAT32DriverState fat32DriveState;


const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '3', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

void initialize_filesystem_fat32() {
    if (is_empty_storage()) {
        // Initialize filesystem
        create_fat32();
    }
    else{
       // Load filesystem 
    }
}

bool is_empty_storage() {
    read_blocks(fat32DriveState.cluster_buf.buf, 0, 1);
    return memcmp(fat32DriveState.cluster_buf.buf, fs_signature, BLOCK_SIZE) == 0;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster){
    memcpy(dir_table->table[0].name, name, 8);
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    dir_table->table[0].cluster_low = (uint16_t)(parent_dir_cluster >> 16);\
    dir_table->table[0].cluster_high = (uint16_t)(parent_dir_cluster & 0xFFFF);
    dir_table->table[0].filesize = 0;
}

void create_fat32() {
    memcpy(fat32DriveState.cluster_buf.buf, fs_signature, BLOCK_SIZE);
    fat32DriveState.fat_table = (struct FAT32FileAllocationTable) {
        .cluster_map = {
            [0] = CLUSTER_0_VALUE,
            [1] = CLUSTER_1_VALUE,
            [2] = FAT32_FAT_END_OF_FILE,
        }
    };
    init_directory_table(&fat32DriveState.dir_table_buf,"Root",2);
    write_clusters(fat32DriveState.cluster_buf.buf, 0, 1);
    write_clusters(fat32DriveState.fat_table.cluster_map, 1, 1);
    write_clusters(fat32DriveState.dir_table_buf.table, 2, 1);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    uint32_t lba = cluster_to_lba(cluster_number);
    uint8_t block_count = cluster_count * CLUSTER_BLOCK_COUNT;
    read_blocks(ptr, lba, block_count);
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    uint32_t lba = cluster_to_lba(cluster_number);
    write_blocks(ptr, lba, cluster_count);
}

uint32_t cluster_to_lba(uint32_t cluster) {
    return cluster * CLUSTER_BLOCK_COUNT;
}

int8_t read_directory(struct FAT32DriverRequest request) {
    read_clusters(request.buf ,request.parent_cluster_number, 1);
    return 0;
}

int8_t write(struct FAT32DriverRequest request) {
    uint32_t cluster_number = find_free_cluster();
    read_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
    if (cluster_number == 0) {
        return -1;
    }
    if (request.parent_cluster_number == 0) {
        return 2;
    }
    else{
        for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++) {
            if (&fat32DriveState.dir_table_buf.table[i] == FAT32_FAT_EMPTY_ENTRY){
                memcpy(fat32DriveState.dir_table_buf.table[i].name, request.name, 8);
                fat32DriveState.dir_table_buf.table[i].attribute = ATTR_SUBDIRECTORY;
                fat32DriveState.dir_table_buf.table[i].user_attribute = UATTR_NOT_EMPTY;
                fat32DriveState.dir_table_buf.table[i].cluster_low = (uint16_t)(request.parent_cluster_number >> 16);
                fat32DriveState.dir_table_buf.table[i].cluster_high = (uint16_t)(request.parent_cluster_number & 0xFFFF);
                fat32DriveState.dir_table_buf.table[i].filesize = request.buffer_size;
            }
                
            }
    }
    write_clusters(&request, cluster_number, 1);
    return 0;
    }


int8_t read(struct FAT32DriverRequest request) {
    read_clusters(request.buf, request.parent_cluster_number, 1);
    return 0;
}


uint32_t find_free_cluster() {
    for (int i = 0; i < CLUSTER_SIZE; i++) {
        if (fat32DriveState.fat_table.cluster_map[i] == 0x00) {
            return i;
        }
    }
    return 0;
}

bool in_directory_table(struct FAT32DirectoryTable *dir_table, char *name) {
    for (int i = 0; i < CLUSTER_MAP_SIZE; i++) {
        if (memcmp(dir_table->table[i].name, name, 8) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}