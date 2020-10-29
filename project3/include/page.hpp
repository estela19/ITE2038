#ifndef __PAGE_H__
#define __PAGE_H__

#include "buffer.hpp"

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

    Page_t& getPage();
    pagenum_t getPagenum();
    int getParentnum();
    int getIsleaf();
    int getNumkeys();
    pagenum_t getRsibnum();
    pagenum_t getMorepnum();
    Precord* getPrecord();
    Record* getRecord();

    void setPagenum(pagenum_t num);
    void setParentnum(pagenum_t num);
    void setIsleaf(int isleaf);
    void setNumkeys(int numkeys);
    void setRsibnum(pagenum_t num);
    void setMorepnum(pagenum_t num);
};

#endif