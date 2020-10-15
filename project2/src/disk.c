#include "disk.h"
#include "file.h"
#include "bpt.h"

int table_id = 0;

int open_table(char* pathname) {
    if (exist_file(pathname)) {
        open_file(pathname);
        file_read_header();
    }
    else {
        open_file(pathname);
        headerManager.header.free_pnum = 0;
        headerManager.header.root_pnum = 0;
        headerManager.header.numpages = 1;
        file_write_header();
    }
    return table_id++;
}

int db_insert(int64_t key, char* value) {
    Node_t* root = NULL;
    if(headerManager.header.root_pnum != 0){
        root = (Node_t*)malloc(sizeof(Node_t));
        file_read_page(headerManager.header.root_pnum, &(root->page));
        root->pnum = headerManager.header.root_pnum;
    }
    int result = insert(root, key, value);
    if (headerManager.modified) {
        file_write_header();
    }
    return result;
}

int db_find(int64_t key, char* ret_val) {
    Node_t* root = NULL;
    if(headerManager.header.root_pnum != 0){
        root = (Node_t*)malloc(sizeof(Node_t));
        file_read_page(headerManager.header.root_pnum, &(root->page));
        root->pnum = headerManager.header.root_pnum;
    }
    Record* tmp = find(root, key);
    if (headerManager.modified) {
        file_write_header();
    }
    strcpy(ret_val, tmp->value);
    return 0;
}

int db_delete(int64_t key) {
    Node_t* root = NULL;
    if(headerManager.header.root_pnum != 0){
        root = (Node_t*)malloc(sizeof(Node_t));
        file_read_page(headerManager.header.root_pnum, &(root->page));
        root->pnum = headerManager.header.root_pnum;
    }
    int result = delete(root, key);
    if (headerManager.modified) {
        file_write_header();
    }
    return 0;
}