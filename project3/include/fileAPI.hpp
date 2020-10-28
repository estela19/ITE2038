#ifndef __FILE_H__
#define __FILE_H__

#include<stdint.h>
#include<string.h>
#include<map>
#include<array>

#define NEWPAGES 4

class FileManager{
private:
    int table_count = 1;
    std::map<std::string path, int table_num> path2tid;
    std::array<int fd, 11> tid2fp;

public:
    static int open_file(char* pathname);

    static int exist_file(char* pathname);

    static void match_fd(char* pathname, int fd);

    static int get_tableid(char* pathname);

    static pagenum_t file_alloc_page(int tid);

    static void file_free_page(int tid, pagenum_t pagenum);

    static void file_read_page(Page_t* p, int tid, pagenum_t pnum);

    static void file_write_page(Page_t* p, int tid, pagenum_t pnum);

    static void file_write_header();

    static void file_read_header();

    static int Get_file_pointer(int table_id);
}

#endif