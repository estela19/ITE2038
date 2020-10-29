#include "dbapi.hpp"

#include<cstdio>

class DBManager{
public:
    static int init_db(int num_buf);

    static int open_table(char* pathname);

    static int db_insert(int table_id, int64_t key, char* value);

    static int db_find(int table_id, int64_t key, char* ret_val);

    static int db_delete(int table_id, int64_t key);

    static int close_table(int table_id);

    static int shutdown_db();

};

static int DBManager::init_db(int num_buf){

}

static int DBManager::open_table(char* pathname) {
    if (exist_file(pathname)) {
        open_file(pathname);
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

static int DBManager::db_insert(int table_id, int64_t key, char* value) {
    Node_t root;
    if(headerManager.header.root_pnum != 0){
        file_read_page(headerManager.header.root_pnum, &(root.page));
        root.pnum = headerManager.header.root_pnum;
    }
    int result = insert(root, key, value);
    if (headerManager.modified) {
        file_write_header();
    }
    return result;
}

static int DBManager::db_find(int table_id, int64_t key, char* ret_val) {
    Node_t root;
    if(headerManager.header.root_pnum != 0){
        root = (Node_t*)malloc(sizeof(Node_t));
        file_read_page(headerManager.header.root_pnum, &(root->page));
        root->pnum = headerManager.header.root_pnum;
    }
    Record* tmp = find(root, key);
    if (headerManager.modified) {
        file_write_header();
    }
    if(tmp == NULL){
        return -1;
    }
    else{
        strcpy(ret_val, tmp->value);
        return 0;
    }
}

static int DBManager::db_delete(int table_id, int64_t key) {
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
    return result;
}

static int DBManager::close_table(int table_id){

}

static int DBManager::shutdown_db(){

}