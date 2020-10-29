#include"page.hpp"

#include<list>

class BufferManager{
private:
    std::list<Buffer>buffManager;
    int size;
public:
    BufferManager(int buff_size);
    int Eviction();
    void Buff_read(pagenum_t pnum, int tid, Page* p);
    void Buff_read(int tid, pagenum_t pnum);
    void Buff_write(Page* p);
    pagenum_t Alloc_page(int tid);
    void Free_page(Page* p);

private:
    auto& Buff_find(Page* p);
    auto& Buff_find(int tid, pagenum_t pnum);
    void Buff_make(int tid, pagenum_t num);
    void setPage(Page* p, auto& it);
    int isfull();
    void usedbuffmove(auto& it);
};

class Buffer{
private:
    Page_t frame;
    pagenum_t pnum;
    int table_id;
    int is_dirty;
    int pincnt;
};