#ifndef __PAGE_H__
#define __PAGE_H__

#include<cstdint>

#include "buffer.hpp"

#define PSIZE 4096
#define HSIZE 24

#define true 1
#define false 0

class Page{
public:
    Page_t* page;
    pagenum_t pnum;
    int table_id;

private:

public:
    Page();
    Page(int tid, pagenum_t pnum);

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

typedef uint64_t pagenum_t;

typedef struct Record_ {
    int64_t key;
    char value[120];
}Record;

typedef struct Precord_ {
    int64_t key;
    uint64_t pnum;
}Precord;

typedef struct Header_ {
    pagenum_t free_pnum;
    pagenum_t root_pnum;
    uint64_t numpages;
}Header;

typedef struct Leaf_ {
    pagenum_t parent_pnum;
    int isLeaf;
    int numkeys;
    char padding[120 - 16];
    pagenum_t rsib_pnum;
    Record record[31];
}Leaf;

typedef struct Internal_ {
    pagenum_t parent_pnum;
    int isLeaf;
    int numkeys;
    char padding[120 - 16];
    pagenum_t more_pnum;
    Precord precord[248];
}Internal;

typedef struct Free_ {
    pagenum_t free_pnum;
    char padding[PSIZE - 8];
}Free;

typedef union Page_t {
    Header header;
    Internal internal;
    Leaf leaf;
    Free free;
}Page_;

/*
typedef struct Page_t_ {
    Page page_;
}Page_t;

typedef struct Node_t_ {
    Page_t page;
    pagenum_t pnum;
}Node_t;
*/

#endif