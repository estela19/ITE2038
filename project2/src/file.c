#include<unistd.h>
#include<sys/types.h>
#include<memory.h>
#include<string.h>

#include "file.h"

#define PSIZE 4096
#define HSIZE 24

#define true 1
#define false 0

int open_file(char* pathname) {
    fd = open(pathname, O_RDWR | O_SYNC | O_CREAT, 0777);
    if (fd < 0) return -1;
    return 0;
}

pagenum_t file_alloc_page() {
    pagenum_t* fnum = headerManager.header.free_pnum;
    if (*fnum == 0) {
        Page_t fpage;
        for (int i = 0; i < 4; ++i) {
            pagenum_t fpnum = headerManager.header.numpages++;
            fpage.page.free.free_pnum = fpnum;
            pwrite(fd, fpage, PSIZE, PSIZE * fpnum);
        }
    }
    headerManager.header.numpages++;
    headerManager.modified = true;
    return *fnum;
}

void file_free_page(pagenum_t pagenum) {
    Page_t fpage;
    fpage.page.free.free_pnum = headerManager.header.free_pnum;
    headerManager.header.free_pnum = pagenum;
    headerManager.modified = true;
    int flag = file_write_page(pagenum, &fpage);
    if (flag == -1) {
        printf("free page error");
    }
}

void file_read_page(pagenum_t pagenum, Page_t* dest) {
    int flag = pread(fd, dest, PSIZE, PSIZE * pagenum);
    if (flag == -1) {
        printf("read page error");
    }
}

void file_write_page(pagenum_t pagenum, const Page_t* src) {
    int flag = pwrite(fd, src, PSIZE, PSIZE * pagenum);
    if (flag == -1) {
        printf("write page error");
    }
}

void file_write_header() {
    int flag = pwrite(fd, &(headerManager.header), HSIZE, 0);
    if (flag == -1) {
        printf("write header error");
    }
}