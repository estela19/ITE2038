#ifndef __BUFFER_H__
#define __BUFFER_H__

#include<list>
#include<algorithm>
#include<memory.h>

#include "page.hpp"
#include "fileAPI.hpp"

class FileManager;
class Page;

class Buffer{

friend class BufferManager;

public:
    Page_t frame;
    pagenum_t pnum;
    int table_id;
    int is_dirty;
    int pincnt;
};

class BufferManager{

private:
    inline static FileManager* file;
    inline static std::list<Buffer> buffManager;
    inline static int size;

public:
    BufferManager(FileManager* filemanager, int buff_size);
    static int Eviction();
    static void Buff_read(pagenum_t pnum, int tid, Page* p);
//    void Buff_read(int tid, pagenum_t pnum);
    static void Buff_write(Page* p);
    pagenum_t Alloc_page(int tid);
    void Free_page(Page* p);
    void Write_Buffers(int tid);

private:
    static std::list<Buffer>::iterator& Buff_find(Page* p);
//    auto& Buff_find(int tid, pagenum_t pnum);
    static void Buff_make(int tid, pagenum_t num);
    static void setPage(Page* p, std::list<Buffer>::iterator it);
    static int isfull();
    static void usedbuffmove(std::list<Buffer>::iterator it);
};

#endif