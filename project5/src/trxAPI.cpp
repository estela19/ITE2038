#include "trxAPI.hpp"

trx_t::trx_t(int trx_id){
    id = trx_id;
    entry = nullptr;
}

int trx_id = 1;
std::unordered_map<int, trx_t*> trx_table;

int trx_begin(){
    trx_t* tmp = new trx_t(trx_id);
    trx_table.insert(std::make_pair(trx_id, tmp));
    return trx_id++;
}

int trx_commit(int trx_id){
    auto it = trx_table.find(trx_id);

    if(it == trx_table.end()){
        return 0;
    }
    else{
        lock_t* tmp = it->second->entry;
        while(tmp){
            LockManager::get().lock_release(tmp);
            tmp = tmp->next_trx;
        }
        return it->second->id;
    }
}