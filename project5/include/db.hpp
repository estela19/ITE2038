#ifndef __DB_H__
#define __DB_H__

#include <cstdint>

#include "dbapi.hpp"


int init_db(int num_buf);

int open_table(char* pathname);

int db_insert(int table_id, int64_t key, char* value);

int db_find(int table_id, int64_t key, char* ret_val, int trx_id);

int db_update(int table_id, int64_t key, char* values, int trx_id);

int db_delete(int table_id, int64_t key);

int close_table(int table_id);

int shutdown_db();

#endif