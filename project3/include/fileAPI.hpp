#ifndef __FILE_H__
#define __FILE_H__

#include<cstdint>
#include<string>
#include<map>
#include<array>
#include<utility>

#include "types.hpp"
#include "buffer.hpp"

#define NEWPAGES 4

class Buffer;

class FileManager{

private:
    int table_count = 1;
    std::map<std::string, int> path2tid;
    std::array<int, 11> tid2fp;

public:
    int open_file(char* pathname);

    int exist_file(char* pathname);

    void match_fd(char* pathname, int fd);

    int get_tableid(char* pathname);

    pagenum_t file_alloc_page(int tid);

    void make_free_page(Page* header);

    void file_read_page(Page_t* p, int tid, pagenum_t pnum);

    void file_write_page(Page_t* p, int tid, pagenum_t pnum);

    int get_file_pointer(int table_id);

    int get_tablenum();
};

#endif