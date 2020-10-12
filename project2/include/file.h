#include<stdint.h>

typedef uint64_t pagenum_t;

typedef struct Record_ {
    int64_t key;
    char* value[120];
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
    pagenum_t rsib_pnum;
    Record record[31];
}Leaf;

typedef struct Internal_ {
    pagenum_t parent_pnum;
    int isLeaf;
    int numkeys;
    pagenum_t more_pnum;
    Precord precord[248];
}Internal;

typedef struct Free_ {
    pagenum_t free_pnum;
}Free;

typedef union Page_ {
    Header header;
    Internal internal;
    Leaf leaf;
    Free free;
}Page;

typedef struct Page_t_ {
    Page page;
}Page_t;

typedef struct Node_t_ {
    Page_t page;
    pagenum_t pnum;
}Node_t;


typedef struct HeaderManager_ {
    Header header;
    int modified = 0;
}HeaderManager;

extern HeaderManager headerManager;

pagenum_t file_alloc_page();

void file_free_page(pagenum_t pagenum);

void file_read_page(pagenum_t pagenum, Page_t* dest);

void file_write_page(pagenum_t pagenum, const Page_t* src);