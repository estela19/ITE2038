#include<unistd.h>
#include<sys/types.h>
#include<memory.h>
#include<string.h>
#include<fcntl.h>
#include<stdio.h>

#include "file.h"


HeaderManager headerManager;

int open_file(char* pathname) {
    fd = open(pathname, O_RDWR | O_CREAT, 0777);
    return fd;
}

int exist_file(char* pathname) {
    return access(pathname, F_OK) != -1;
}

pagenum_t file_alloc_page() {
    pagenum_t fnum = headerManager.header.free_pnum;
    if (fnum == 0) {
        Page_t fpage;
        headerManager.header.free_pnum = headerManager.header.numpages;
        for (int i = 0; i < 4; ++i) {
            if(i == 3){
                fpage.page.free.free_pnum = 0;
            }
            else{
                fpage.page.free.free_pnum = headerManager.header.numpages + 1;
            }
            file_write_page(headerManager.header.numpages, &fpage);
            headerManager.header.numpages++;
        }
        fnum = headerManager.header.free_pnum;
    }
    Page_t tmp;
    file_read_page(fnum, &tmp);
    headerManager.header.free_pnum = tmp.page.free.free_pnum;
    headerManager.modified = true;
    return fnum;
}

void file_free_page(pagenum_t pagenum) {
    Page_t fpage;
    fpage.page.free.free_pnum = headerManager.header.free_pnum;
    headerManager.header.free_pnum = pagenum;
    headerManager.modified = true;
    file_write_page(pagenum, &fpage);
}

void file_read_page(pagenum_t pagenum, Page_t* dest) {
    int flag = pread(fd, dest, PSIZE, PSIZE * pagenum);
    if (flag == -1) {
        printf("read page error\n");
    }
}

void file_write_page(pagenum_t pagenum, const Page_t* src) {
    int flag = pwrite(fd, src, PSIZE, PSIZE * pagenum);
    if (flag == -1) {
        printf("write page error\n");
    }
    flag = fsync(fd);
    if (flag == -1) {
        printf("write sync error\n");
    }
}

void file_write_header() {
    int flag = pwrite(fd, &(headerManager.header), HSIZE, 0);
    if (flag == -1) {
        printf("write header error");
    }
}

void file_read_header() {
    int flag = pread(fd, &(headerManager.header), HSIZE, 0);
    if (flag == -1) {
        printf("read header error");
    }
}