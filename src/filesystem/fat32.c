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
         read_clusters(fat32DriveState.fat_table.cluster_map, 1, 1);
    }
}

bool is_empty_storage() {
    read_clusters(fat32DriveState.cluster_buf.buf, 0, 1);
    return memcmp(fat32DriveState.cluster_buf.buf, fs_signature, BLOCK_SIZE) != 0;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster){
    dir_table->table[0].undelete = 0;
    dir_table->table[0].create_time = 0;
    dir_table->table[0].create_date = 0;
    dir_table->table[0].access_date = 0;
    dir_table->table[0].modified_time = 0;
    dir_table->table[0].modified_date = 0;
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
    for (int i = 0; i < 64; i++){
        if (memcmp(fat32DriveState.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32DriveState.dir_table_buf.table[i].ext, request.ext, 3) == 0){
            if (request.buffer_size < fat32DriveState.dir_table_buf.table[i].filesize) {
                return 2;
            }
            else if (fat32DriveState.dir_table_buf.table[i].attribute == 1){
                return 1;
            }
            else {
                uint8_t loc = (fat32DriveState.dir_table_buf.table[i].cluster_high << 16) | fat32DriveState.dir_table_buf.table[i].cluster_low;
                read_clusters(request.buf, loc, 1);
                return 0;
            }
        }
    }
    return -1;
}





int8_t write(struct FAT32DriverRequest request) {
    uint32_t cluster_number = find_free_cluster();
    read_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
    if (cluster_number == 0) {
        return -1;
    }
    for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++){
        if (memcmp(fat32DriveState.dir_table_buf.table[i].name, request.name, 8) == 0){
            if (fat32DriveState.dir_table_buf.table[i].filesize > 0){
                return 2;
            }
            break;
        }
            
        }
    
    if (request.parent_cluster_number < 2) {
        return 2;
    }
    else{
        bool found = FALSE;
        for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++){
            if (memcmp(fat32DriveState.dir_table_buf.table[i].name, request.name, 8) == 0){
                found = TRUE;
                break;
            }
        }
        if (found){
            return 1;
        }
        struct FAT32DirectoryEntry entry={
            .name = {0},
            .ext = {0},
          .attribute = 0,
          .user_attribute = 0,
            .undelete = 0,
            .create_time = 0,
            .create_date = 0,
            .access_date = 0,
            .cluster_high = 0,
            .modified_time = 0,
            .modified_date = 0,
            .cluster_low = 0,
            .filesize = 0,
        };
                memcpy(entry.name, request.name, 8);
                entry.attribute = ATTR_SUBDIRECTORY;
                entry.user_attribute = UATTR_NOT_EMPTY;
                entry.cluster_low = (uint16_t)(request.parent_cluster_number >> 16);
                entry.cluster_high = (uint16_t)(request.parent_cluster_number & 0xFFFF);
                entry.filesize = request.buffer_size;
        for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++) {
            if (fat32DriveState.dir_table_buf.table[i].user_attribute != UATTR_NOT_EMPTY){
                fat32DriveState.dir_table_buf.table[i] = entry;
                write_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
                break;
            }
        }
        read_clusters(&fat32DriveState.fat_table, FAT_CLUSTER_NUMBER, 1);
        if(request.buffer_size > 512){
            uint8_t cluster_count = request.buffer_size / 512;
            for (uint8_t i = 0; i < cluster_count-1; i++) {
                uint32_t cluster_next_number;
                for (int j = 0; j < CLUSTER_SIZE; j++) {
                    if (fat32DriveState.fat_table.cluster_map[i] == 0x00) {
                        cluster_next_number = i;
                        break;
                    }
                }
                fat32DriveState.fat_table.cluster_map[cluster_number] = cluster_next_number;
                cluster_number = find_free_cluster();
            }
            fat32DriveState.fat_table.cluster_map[cluster_number] = FAT32_FAT_END_OF_FILE;
            write_clusters(&fat32DriveState.fat_table, FAT_CLUSTER_NUMBER, 1);
        }
        else{
            fat32DriveState.fat_table.cluster_map[cluster_number] = FAT32_FAT_END_OF_FILE;
            write_clusters(&fat32DriveState.fat_table, FAT_CLUSTER_NUMBER, 1);

        }
        if (request.buffer_size == 0){
            memset(&fat32DriveState.dir_table_buf, 0, sizeof(fat32DriveState.dir_table_buf.table));
            read_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
            init_directory_table(&fat32DriveState.dir_table_buf, request.name, request.parent_cluster_number);
            write_clusters(&fat32DriveState.dir_table_buf, cluster_number, 1);
        }
        else{
            
            memset(&fat32DriveState.cluster_buf, 0, sizeof(fat32DriveState.cluster_buf.buf));
            read_clusters(&fat32DriveState.cluster_buf, 1, 1);
            memcpy(fat32DriveState.cluster_buf.buf, request.buf, request.buffer_size);
            write_clusters(&fat32DriveState.cluster_buf, cluster_number, 1);
        }
        
        
    }
    return 0;
    }



int8_t read(struct FAT32DriverRequest request) {
    for (int i = 0; i < 64; i++){
        if (memcmp(fat32DriveState.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32DriveState.dir_table_buf.table[i].ext, request.ext, 3) == 0){
            if (request.buffer_size < fat32DriveState.dir_table_buf.table[i].filesize) {
                return 2;
            }

            else if (fat32DriveState.dir_table_buf.table[i].attribute == 1){
                return 1;
            }
            else {
                uint8_t tot = fat32DriveState.dir_table_buf.table[i].filesize / CLUSTER_SIZE;
                if (tot * CLUSTER_SIZE < fat32DriveState.dir_table_buf.table[i].filesize){
                    tot += 1;
                }
                else {
                    uint8_t file_location = (fat32DriveState.dir_table_buf.table[i].cluster_high << 16) | fat32DriveState.dir_table_buf.table[i].cluster_low;
                    for (uint32_t j = 0; j < tot; j++){
                        if (j == 0){
                            read_clusters(request.buf + CLUSTER_SIZE * j, file_location, 1);
                        }
                        else {
                            read_clusters(request.buf + CLUSTER_SIZE * j, fat32DriveState.fat_table.cluster_map[file_location], 1);
                            file_location = fat32DriveState.fat_table.cluster_map[file_location];
                        }
                    }
                    return 0;
                }
            }
        }
    }
    return -1;
}



struct FAT32DirectoryEntry find_entry(uint32_t parent_cluster_number, char *name) {
    read_clusters(&fat32DriveState.dir_table_buf, parent_cluster_number, 1);
    for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++) {
        if (memcmp(fat32DriveState.dir_table_buf.table[i].name, name, 8) == 0){
            return fat32DriveState.dir_table_buf.table[i];
        }
    }
    struct FAT32DirectoryEntry temp={
        .name = {0},
        .ext = {0},
        .attribute = 0,
        .user_attribute = 0,
        .undelete = 0,
        .create_time = 0,
        .create_date = 0,
        .access_date = 0,
        .cluster_high = 0,
        .modified_time = 0,
        .modified_date = 0,
        .cluster_low = 0,
        .filesize = 0,
    };
    return temp;
}

int8_t delete(struct FAT32DriverRequest request) {
    
    read_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
    for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++) {
        if (memcmp(fat32DriveState.dir_table_buf.table[i].name, request.name, 8) == 0){
            fat32DriveState.dir_table_buf.table[i].user_attribute = UATTR_NOT_EMPTY;
            write_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
            break;
        }
    }

    struct FAT32DirectoryEntry entry = find_entry(request.parent_cluster_number, request.name);
    if (entry.user_attribute != UATTR_NOT_EMPTY) {
        return 1;
    }
    else{
            uint32_t bot = entry.cluster_low;
            uint32_t top = entry.cluster_high;
        read_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
        for (uint8_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry) ; i++) {
            if (memcmp(&fat32DriveState.dir_table_buf.table[i], &entry, UATTR_NOT_EMPTY) == 0){
                struct FAT32DirectoryEntry temp={
                    .name = {0},
                    .ext = {0},
                    .attribute = 0,
                    .user_attribute = 0,
                    .undelete = 0,
                    .create_time = 0,
                    .create_date = 0,
                    .access_date = 0,
                    .cluster_high = 0,
                    .modified_time = 0,
                    .modified_date = 0,
                    .cluster_low = 0,
                    .filesize = 0,
                };
                fat32DriveState.dir_table_buf.table[i] = temp;
                    write_clusters(&fat32DriveState.dir_table_buf, request.parent_cluster_number, 1);
                break;
            }
        }
        read_clusters(&fat32DriveState.fat_table, FAT_CLUSTER_NUMBER, 1);
        for (uint32_t i = bot; i < top; i++) {
            fat32DriveState.fat_table.cluster_map[i] = 0x00;
        }
        write_clusters(&fat32DriveState.fat_table, FAT_CLUSTER_NUMBER, 1);
        return 0;
    }
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