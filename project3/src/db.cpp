#include "db.hpp"

int init_db(int num_buf){
    DBManager::get().initialize(num_buf);
}

int open_table(char* pathname){
    DBManager::get().open_table(pathname);
}

int db_insert(int table_id, int64_t key, char* value){
    DBManager::get().db_insert(table_id, key, value);
}

int db_find(int table_id, int64_t key, char* ret_val){
    DBManager::get().db_find(table_id, key, ret_val);
}

int db_delete(int table_id, int64_t key){
    DBManager::get().db_delete(table_id, key);
}

int close_table(int table_id){
    DBManager::get().close_table(table_id);
}

int shutdown_db(){
    DBManager::get().shutdown_db();
    DBManager::Destroy();
}