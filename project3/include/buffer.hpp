#include"page.hpp"

#include<list>

class BufferManager{
private:
    std::list<Buffer>buffManager;
    int size;
public:
    BufferManager(int buff_size);

};

class Buffer{
private:
    Page_t frame;
    pagenum_t pnum;
    int table_id;
    int is_dirty;
    int pincnt;
    Buffer* prev;
    Buffer* next;
};