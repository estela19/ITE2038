#ifndef __TYPES_H__
#define __TYPES_H__

#include<cstdint>
#include<stdint.h>

#define PSIZE 4096
#define HSIZE 24

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

#endif