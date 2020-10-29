#ifndef __DBAPI_H__
#define __DBAPI_H__

#include <cstdint>

#include "fileAPI.hpp"
#include "buffer.hpp"
#include "bpt.hpp"
#include "page.hpp"

class DBManager{
private:
    BPT* bpt;
    BufferManager* buff;
    FileManager* file;

public:
    static DBManager& DBManager::get();

    void initialize(int buff_size);

    int init_db(int num_buf);

    int open_table(char* pathname);

    int db_insert(int table_id, int64_t key, char* value);

    int db_find(int table_id, int64_t key, char* ret_val);

    int db_delete(int table_id, int64_t key);

    int close_table(int table_id);

    int shutdown_db();

public:
    BPT& getBPT();

    BufferManger& getBuffmgr();

    FileManager& getFilemgr()

private:
    DBManager();

};

#endif 