#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <list>
#include <algorithm>
#include <memory.h>
#include <mutex>

#include "page.hpp"
#include "fileAPI.hpp"

class FileManager;
class Page;

class Buffer{
public:
    Page_t frame;
    pagenum_t pnum;
    int table_id;
    int is_dirty;
    int pincnt;
    std::mutex m;
};

class BufferManager{
private:
    inline static FileManager* file;
    inline static std::list<Buffer> buffManager;
    inline static int size;
    inline static std::mutex m;

public:
    BufferManager(FileManager* filemanager, int buff_size);
    static int Eviction();
    static void Buff_read(pagenum_t pnum, int tid, Page* p);
    static void Buff_write(Page* p);
    pagenum_t Alloc_page(int tid);
    void Free_page(Page* p);
    void Write_Buffers(int tid);
    static std::list<Buffer>& getBuffmgr();

private:
    static std::list<Buffer>::iterator Buff_find(Page* p);
    static std::list<Buffer>::iterator Buff_make(int tid, pagenum_t num);
    static void setPage(Page* p, std::list<Buffer>::iterator it);
    static int isfull();
    static std::list<Buffer>::iterator usedbuffmove(std::list<Buffer>::iterator it);
};

#endif