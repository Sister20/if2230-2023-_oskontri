// #include "lib-header/stdtype.h"

// int main(void) {
//     __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(0xDEADBEEF));
//     while (TRUE);
//     return 0;
// }
#include "lib-header/stdtype.h"
#include "filesystem/fat32.h"

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *buf1 = (const uint8_t*) s1;
    const uint8_t *buf2 = (const uint8_t*) s2;
    for (size_t i = 0; i < n; i++) {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}


void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void puts(char *buf, uint32_t len, uint32_t color) {
    syscall(5, (uint32_t) buf, len, color);
}
void command (uint32_t buf){
    int32_t retcode;
    

    if (memcmp((char *) buf, "mkdir", 5)){
        const char *name = (const char *)(buf + 6);
        struct  FAT32DriverRequest request = {
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = 0,
        };
        memcpy(request.name, name, sizeof(request.name) - 1);
        syscall(1, (uint32_t)&request, (uint32_t)&retcode, 0);
        // if (retcode == 0){
        //     puts("Create Directory success", 23, 0x2);
        // }
        // else {
        //     puts("Create Directory failed", 22, 0x4);
        // }
    }

    

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
    int32_t retcode=0;
    syscall(0, (uint32_t) &request, (uint32_t) &retcode, 0);
    if (retcode == 0)
        syscall(5, (uint32_t) "owo\n", 4, 0xF);

    char buf[16];
    while (TRUE) {
        puts("oskontri@OS", 11, 0xA);
        syscall(4, (uint32_t) buf, 16, 0);
        // command((uint32_t)buf);
    }

    return 0;
}


