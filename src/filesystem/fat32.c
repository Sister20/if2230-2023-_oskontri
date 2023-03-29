#include "lib-header/stdtype.h"
#include "lib-header/fat32.h"
#include "lib-header/stdmem.h"

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '3', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

void initialize_filesystem_fat32(void) {
  if (is_empty_storage()) {
    create_fat32();
  } else {
    // Read FileAllocationTable and other states and put them into FAT32DriverState
    read_file_allocation_table(1, fat32_state.fat);
    fat32_state.current_dir_cluster = CLUSTER_ROOT_DIR;
  }
}

uint32_t cluster_to_lba(uint32_t cluster) {
    return (cluster - CLUSTER_2_VALUE) * SECTORS_PER_CLUSTER + data_start_lba;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster) {
    // Set name
    memset(dir_table->name, ' ', sizeof(dir_table->name));
    strncpy(dir_table->name, name, strlen(name));
    
    // Set attribute to directory
    dir_table->attributes = ATTR_DIRECTORY;
    
    // Set first cluster to 0 (empty directory)
    dir_table->first_cluster = 0;
    
    // Set parent directory cluster number
    dir_table->parent_dir_cluster = parent_dir_cluster;
    
    // Set other fields to 0
    dir_table->file_size = 0;
    dir_table->creation_time = 0;
    dir_table->last_access_date = 0;
    dir_table->last_modified_time = 0;
}

bool is_empty_storage(void) {
    uint8_t buffer[SECTOR_SIZE];

    // Read boot sector from storage
    if (!storage_read(0, buffer)) {
        return false;
    }

    // Compare boot sector with fs_signature
    if (memcmp(buffer, fs_signature, sizeof(fs_signature)) != 0) {
        return true;
    } else {
        return false;
    }
}


void create_fat32(void) {
    // Write file system signature to boot sector
    memcpy(&boot_sector.fs_signature, &fs_signature, sizeof(fs_signature));
    write_block(&boot_sector, BOOT_SECTOR_ADDRESS, 1);

    // Initialize first two clusters in FAT table
    fat_table[0] = CLUSTER_0_VALUE;
    fat_table[1] = CLUSTER_1_VALUE;
    for (int i = 2; i < FAT_TABLE_SIZE; i++) {
        fat_table[i] = UNUSED_CLUSTER;
    }

    // Write FAT table to disk
    write_block(fat_table, FAT_TABLE_ADDRESS, FAT_TABLE_SIZE / BLOCK_SIZE);

    // Initialize root directory table
    directory_table[0].attributes = ATTR_DIRECTORY;
    directory_table[0].first_cluster = ROOT_CLUSTER;
    strncpy(directory_table[0].name, ".", NAME_LENGTH);
    strncpy(directory_table[1].name, "..", NAME_LENGTH);

    // Write root directory table to disk
    write_block(directory_table, CLUSTER_TO_ADDRESS(ROOT_CLUSTER), 1);
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    uint32_t lba = cluster_to_lba(cluster_number);
    uint32_t block_count = cluster_count * boot_sector.sectors_per_cluster;

    write_blocks(ptr, lba, block_count);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count) {
    uint32_t lba = cluster_to_lba(cluster_number);
    uint8_t block_count = cluster_count * CLUSTER_SIZE / BLOCK_SIZE;
    read_blocks(ptr, lba, block_count);
}

int8_t read_directory(struct FAT32DriverRequest request) {
    if (request.parent_cluster_number == CLUSTER_0_VALUE) {
        return 1; // not a folder
    }

    struct FAT32DirectoryTable *dir_table = (struct FAT32DirectoryTable *) request.buf;

    uint32_t parent_lba = cluster_to_lba(request.parent_cluster_number);
    uint8_t cluster_count = ceil((double) request.buffer_size / (double) sizeof(struct FAT32DirectoryTable));

    struct ClusterBuffer buffer;
    buffer.data = (uint8_t *) dir_table;
    buffer.count = cluster_count;

    read_clusters(&buffer, request.parent_cluster_number, cluster_count);

    for (int i = 0; i < cluster_count * ENTRIES_PER_CLUSTER; i++) {
        if (dir_table[i].name[0] == DIRECTORY_ENTRY_FREE || dir_table[i].name[0] == DIRECTORY_ENTRY_END) {
            break;
        }

        if (dir_table[i].attributes == DIRECTORY_ATTRIBUTE_LONG_NAME) {
            continue;
        }

        if (dir_table[i].attributes != DIRECTORY_ATTRIBUTE_DIRECTORY) {
            return 1; // not a folder
        }

        if (strcmp(dir_table[i].name, request.name) == 0) {
            return 0; // success
        }
    }

    return 2; // not found
}

int8_t read(struct FAT32DriverRequest request) {
    // Check if the given path is a file
    struct FAT32DirectoryTable dir_table;
    int8_t res = read_directory(request, &dir_table);
    if (res != 0) {
        return res;
    }
    if ((dir_table.attr & ATTR_DIRECTORY) == ATTR_DIRECTORY) {
        return 1; // Not a file
    }

    // Get the file size and calculate the number of clusters needed
    uint32_t file_size = dir_table.file_size;
    uint32_t cluster_count = (file_size + (FAT32_DRIVER_STATE.cluster_size - 1)) / FAT32_DRIVER_STATE.cluster_size;
    if (cluster_count > request.buffer_size / FAT32_DRIVER_STATE.cluster_size) {
        return 2; // Not enough buffer
    }

    // Read the file's clusters and store them in the buffer
    uint32_t cluster_number = dir_table.first_cluster;
    uint32_t bytes_read = 0;
    while (cluster_count > 0) {
        uint32_t clusters_to_read = MIN(cluster_count, MAX_READ_CLUSTERS);
        read_clusters(request.buf + bytes_read, cluster_number, clusters_to_read);
        bytes_read += clusters_to_read * FAT32_DRIVER_STATE.cluster_size;
        cluster_number = get_next_cluster(cluster_number);
        cluster_count -= clusters_to_read;
    }

    return 0; // Success
}

int8_t delete(struct FAT32DriverRequest request) {
    if (request.parent_cluster_number == 0) {
        return 2; // invalid parent cluster
    }

    // check if directory is empty
    struct FAT32DirectoryTable dir_table;
    memset(&dir_table, 0, sizeof(struct FAT32DirectoryTable));
    int8_t status = read_directory((struct FAT32DriverRequest) {
        .buf = &dir_table,
        .name = "",
        .ext = "",
        .parent_cluster_number = request.cluster_number,
        .buffer_size = sizeof(struct FAT32DirectoryTable)
    });
    if (status != 0) {
        return status; // not found or not a folder
    }
    if (dir_table.child_cluster_number != 0) {
        return 2; // folder is not empty
    }

    // mark directory as free
    struct FAT32DirectoryEntry dir_entry;
    memset(&dir_entry, 0, sizeof(struct FAT32DirectoryEntry));
    status = find_directory_entry(request, &dir_entry, true);
    if (status != 0) {
        return status; // not found or not a file
    }
    dir_entry.attribute = FAT32_ATTR_LONG_NAME;
    if (write_directory_entry(request, &dir_entry) != 0) {
        return -1; // unknown error
    }

    // free cluster chain
    uint32_t cluster_number = dir_entry.first_cluster_number;
    while (cluster_number != CLUSTER_LAST) {
        uint32_t next_cluster_number = get_fat_entry(cluster_number);
        set_fat_entry(cluster_number, CLUSTER_FREE);
        cluster_number = next_cluster_number;
    }

    return 0; // success
}