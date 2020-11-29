#include "db.hpp"

int init_db(int num_buf){
    DBManager::get().initialize(num_buf);
    return 0;
}

int open_table(char* pathname){
    return DBManager::get().open_table(pathname);
}

int db_insert(int table_id, int64_t key, char* value){
    return DBManager::get().db_insert(table_id, key, value);
}

int db_find(int table_id, int64_t key, char* ret_val, int trx_id){
    return DBManager::get().db_find(table_id, key, ret_val, trx_id);
}

int db_update(int table_id, int64_t key, char* values, int trx_id){
    return DBManager::get().db_update(table_id, key, values, trx_id);
}

int db_delete(int table_id, int64_t key){
    return DBManager::get().db_delete(table_id, key);
}

int close_table(int table_id){
    return DBManager::get().close_table(table_id);
}

//todo return value
int shutdown_db(){
    DBManager::get().shutdown_db();
    DBManager::get().Destroy();
}