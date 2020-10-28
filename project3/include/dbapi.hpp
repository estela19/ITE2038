#ifndef __DBAPI_H__
#define __DBAPI_H__

#include <cstdint>

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

#endif 