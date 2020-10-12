#include<unistd.h>
#include<sys/types.h>
#include<memory.h>
#include<string.h>

#include "file.h"

#define PSIZE 4096
#define HSIZE 24

#define true 1
#define false 0

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
    headerManager.sync = 0;
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

void file_write_header(const Header* src) {
    int flag = pwrite(fd, src, HSIZE, 0);
    if (flag == -1) {
        printf("write header error");
    }
}