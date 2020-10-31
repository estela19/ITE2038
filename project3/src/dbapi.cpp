#include "dbapi.hpp"

DBManager& DBManager::get(){
    static DBManager instance;
    return instance;
}

void DBManager::initialize(int num_buf){
    file = new FileManager();
    buff = new BufferManager(file, num_buf);
    bpt = new BPT(buff);
}

void DBManager::Destroy(){
    delete bpt;
    delete buff;
    delete file;
}

int DBManager::open_table(char* pathname) {
    if (file->exist_file(pathname)) {
        file->open_file(pathname);
    }
    else {
        int fd = file->open_file(pathname);
        Page_t header;
        header.header.root_pnum = 0;
        header.header.free_pnum = 0;
        header.header.numpages = 1;
        int a = file->get_tableid(pathname);
        file->file_write_page(&header, a, 0);
    }
    return file->get_tableid(pathname);
}

int DBManager::db_insert(int table_id, int64_t key, char* value) {
    Page root;
    Page header(table_id, 0);
    if(header.page->header.root_pnum != 0){
        root.pnum = header.page->header.root_pnum;
        root.table_id = table_id;
        buff->Buff_read(header.page->header.root_pnum, table_id, &root);
    }
    bpt->Settid(table_id);
    int result = bpt->Insert(&root, key, value);
    return result;
}

int DBManager::db_find(int table_id, int64_t key, char* ret_val) {
    Page root;
    Page header(table_id, 0);
    if(header.page->header.root_pnum != 0){
        root.pnum = header.page->header.root_pnum;
        root.table_id = table_id;
        buff->Buff_read(header.page->header.root_pnum, table_id, &root);
    }
    bpt->Settid(table_id);
    Record tmp; 
    if(bpt->Find(&root, &tmp, key) != 0){
        return -1;
    }
    else{
        strncpy(ret_val, tmp.value, 120);
        return 0;
    }
}

int DBManager::db_delete(int table_id, int64_t key) {
    Page root;
    Page header(table_id, 0);
    if(header.page->header.root_pnum != 0){
        root.pnum = header.page->header.root_pnum;
        root.table_id = table_id;
        buff->Buff_read(header.page->header.root_pnum, table_id, &root);
    }
    bpt->Settid(table_id);
    int result = bpt->Delete(&root, key);
    return result;
}

int DBManager::close_table(int table_id){
    buff->Write_Buffers(table_id);
    file->close_file(file->convert_tid2fp(table_id));
}

int DBManager::shutdown_db(){
    if(buff == nullptr) return -1;
    else{
        int num = file->get_tablenum();
        for(int i = 1; i <= num; i++){
            close_table(i);
        }
        return 0;
    }
}

DBManager::DBManager(){
    file = nullptr;
    buff = nullptr;
    bpt = nullptr;
}

BPT& DBManager::getBPT(){
    return *bpt;
}

BufferManager& DBManager::getBuffmgr(){
    return *buff;
}

FileManager& DBManager::getFilemgr(){
    return *file;
}