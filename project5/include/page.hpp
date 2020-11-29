#ifndef __PAGE_H__
#define __PAGE_H__

#include "types.hpp"

class BufferManager;

class Page{
public:
    Page_t* page;
    pagenum_t pnum;
    int table_id;
    bool is_empty;

private:

public:
    Page();
    Page(int tid, pagenum_t pnum);
    ~Page();

    void SetPage(Page* c);
    void SetPage(int tid, pagenum_t pnum);

};

#endif