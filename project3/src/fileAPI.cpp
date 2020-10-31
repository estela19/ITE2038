#include<unistd.h>
#include<sys/types.h>
#include<memory.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>

#include "fileAPI.hpp"


int FileManager::open_file(char* pathname) {
    int fd = open(pathname, O_RDWR | O_CREAT, 0777);
    match_fd(pathname, fd);
}

int FileManager::exist_file(char* pathname) {
    struct stat buf;
    int res = stat(pathname, &buf);
    return res == 0;
}

void FileManager::match_fd(char* pathname, int fd){
    tid2fp[get_tableid(pathname)] = fd;
}

int FileManager::get_tableid(char* pathname){
    auto it = path2tid.find(pathname);
    if(it == path2tid.end()){
        path2tid.insert(std::make_pair(std::string(pathname), table_count));
        return table_count++;
    }
    else{
        return it->second;
    }
}

//move to buffer method
//maybe not used
/*
pagenum_t FileManager::file_alloc_page() {
    pagenum_t fnum = headerManager.header.free_pnum;
    if (fnum == 0) {
        Page_t fpage;
        headerManager.header.free_pnum = headerManager.header.numpages;
        for (int i = 0; i < NEWPAGES; ++i) {
            if(i == NEWPAGES - 1){
                fpage.free.free_pnum = 0;
            }
            else{
                fpage.free.free_pnum = headerManager.header.numpages + 1;
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
*/

void FileManager::make_free_page(Page* header){
    Page_t tmp;
    memset(&tmp, 0, sizeof(Page_t));
    header->page->header.free_pnum = header->page->header.numpages;
    for(int i = 0; i < NEWPAGES; i++){
        if(i == NEWPAGES - 1){
            tmp.free.free_pnum = 0;
        }
        else{
            tmp.free.free_pnum = header->page->header.numpages + 1;
        }
        file_write_page(&tmp, header->table_id, header->page->header.numpages);
        header->page->header.numpages++;
    }
 //   header->is_dirty = 1;
}

void FileManager::file_read_page(Page_t* p, int tid, pagenum_t pnum) {
    int fd = get_file_pointer(tid);
    pread(fd, p, PSIZE, PSIZE * pnum);
}

void FileManager::file_write_page(Page_t* p, int tid, pagenum_t pnum) {
    int fd = get_file_pointer(tid);
    pwrite(fd, p, PSIZE, PSIZE * pnum);
    fsync(fd);
}

int FileManager::get_file_pointer(int table_id){
    return tid2fp[table_id];
}

int FileManager::get_tablenum(){
    return table_count;
}