// #include "lib-header/stdtype.h"

// int main(void) {
//     __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(0xDEADBEEF));
//     while (TRUE);
//     return 0;
// }
#include "lib-header/stdtype.h"
#include "filesystem/fat32.h"

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}



// syscall untuk mengubah current working directory
void chdir(const char *path) {
    syscall(10, (uint32_t) path, 0, 0);
}

// syscall untuk menampilkan isi current working directory
void ls(void) {
    syscall(11, 0, 0, 0);
}

// syscall untuk membuat folder baru
void mkdir(const char *path) {
    syscall(12, (uint32_t) path, 0, 0);
}

// syscall untuk menampilkan isi file ke layar
void cat(const char *path) {
    syscall(13, (uint32_t) path, 0, 0);
}

// syscall untuk menyalin file atau folder
void cp(const char *src_path, const char *dst_path) {
    syscall(14, (uint32_t) src_path, (uint32_t) dst_path, 0);
}

// syscall untuk menghapus file atau folder
void rm(const char *path) {
    syscall(15, (uint32_t) path, 0, 0);
}

// syscall untuk memindahkan dan merename file atau folder
void mv(const char *src_path, const char *dst_path) {
    syscall(16, (uint32_t) src_path, (uint32_t) dst_path, 0);
}

// syscall untuk mencari file atau folder dengan nama yang sama di seluruh file system
void whereis(const char *name) {
    syscall(17, (uint32_t) name, 0, 0);
}



int main(void) {
    struct ClusterBuffer cl           = {0};
    struct FAT32DriverRequest request = {
        .buf                   = &cl,
        .name                  = "ikanaide",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);
    if (retcode == 0)
        syscall(5, (uint32_t) "owo\n", 4, 0xF);

    char buf[16];
    while (TRUE) {
        syscall(4, (uint32_t) buf, 16, 0);
        syscall(5, (uint32_t) buf, 16, 0xF);
    }

    return 0;
}

