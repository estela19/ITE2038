#ifndef __TRANSACTION_MANAGER_H__
#define __TRANSACTION_MANAGER_H__

#include<unordered_map>

#include "lock_manager.hpp"

struct trx_t{
    int id;
    lock_t* entry;

    trx_t(int trx_id);
};

    int trx_begin();
    int trx_commit(int trx_id);

    extern std::unordered_map<int, trx_t*> trx_table;
    extern int trx_id;

#endif 