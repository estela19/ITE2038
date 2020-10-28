#include<unistd.h>
#include<sys/types.h>
#include<memory.h>
#include<string.h>
#include<fcntl.h>
#include<stdio.h>
#include<utility>

#include "file.hpp"
#include "page.hpp"

class FileManager{
private:
    table_count = 0;
    std::map<int fd, int table_num> fpointers;

public:
    static void open_file(char* pathname);

    static int exist_file(char* pathname);

    static int get_tableid(int fd);

    static pagenum_t file_alloc_page();

    static void file_free_page(pagenum_t pagenum);

    static void file_read_page(pagenum_t pagenum, Page_t* dest);

    static void file_write_page(pagenum_t pagenum, const Page_t* src);

    static void file_write_header();

    static void file_read_header();
}

static void FileManager::open_file(char* pathname) {
    int fd = open(pathname, O_RDWR | O_CREAT, 0777);
    match_fd(pathname, fd);
}

static int FileManager::exist_file(char* pathname) {
    return access(pathname, F_OK) != -1;
}

static void FileManager::match_fd(char* pathname, int fd){
    tid2fp[get_tableid(pathname)] = fd;
}

static int FileManager::get_tableid(char* pathname){
    auto it = fpointers.find(pathname);
    if(it == fpointers.end()){
        fpointers.insert(std::make_pair<std::string(pathname), table_count>);
        return table_count++;
    }
    else{
        return it->second;
    }
}

//move to buffer method
static pagenum_t FileManager::file_alloc_page() {
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

static void FileManager::make_free_page(Buffer* header){
    Page_t tmp;
    header->frame.header.free_pnum = header->frame.header.numpages;
    for(int i = 0; i < NEWPAGES; i++){
        if(i == NEWPAGES - 1){
            fpage.free.free_pnum = 0;
        }
        else{
            fpage.free.free_pnum = header->frame.header.numpages + 1;
        }
        file_write_page(tmp, header->table_id, header->pnum);
        header->frame.header.numpages++;
    }
    header->is_dirty = 1;
}

//move to buffer method 
//maybe notused
static void FileManager::file_free_page(pagenum_t pagenum, int table_id) {
    Page_t fpage;
    Page header;
    header.pnum = 0;
    header.table_id = table_id;
    file_read_page()
    header.page.free.free_pnum = 
    file_read_
    fpage.page.free.free_pnum = headerManager.header.free_pnum;
    headerManager.header.free_pnum = pagenum;
    headerManager.modified = true;
    file_write_page(pagenum, &fpage);
}

static void FileManager::file_read_page(Page_t* p, int tid, pagenum_t pnum) {
    int fd = Get_file_pointer(tid);
    int flag = pread(fd, p, PSIZE, PSIZE * pnum);
    if (flag == -1) {
        printf("read page error\n");
    }
}

static void FileManager::file_write_page(Page_t* p, int tid, pagenum_t pnum) {
    int fd = Get_file_pointer(tid);
    int flag = pwrite(fd, p, PSIZE, PSIZE * pnum);
    if (flag == -1) {
        printf("write page error\n");
    }
    flag = fsync(fd);
    if (flag == -1) {
        printf("write sync error\n");
    }
}

static void FileManager::file_write_header() {
    int flag = pwrite(fd, &(headerManager.header), HSIZE, 0);
    if (flag == -1) {
        printf("write header error");
    }
}

static void FileManager::file_read_header() {
    int flag = pread(fd, &(headerManager.header), HSIZE, 0);
    if (flag == -1) {
        printf("read header error");
    }
}

static int FileManager::Get_file_pointer(int table_id){
    return tid2fp[table_id];
}