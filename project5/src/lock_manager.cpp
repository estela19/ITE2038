#include <mutex>

#include "lock_manager.hpp"


LockManager& LockManager::get(){
	static LockManager instance;
	return instance;
}

lock_t* LockManager::lock_acquire(int table_id, int64_t key, int trx_id, lockmode mode)
{
	std::unique_lock<std::mutex> lock(m);

	auto it = lock_table.find(std::make_pair(table_id, key));
	lock_t* tmp = new lock_t;
	hash_t* entry;

	tmp->lock_mode = mode;
	tmp->trx_id = trx_id;

	if(it == lock_table.end()){
		hash_t* hash = new hash_t;
		hash->table_id = table_id;
		hash->key = key;

		lock_table.insert(std::make_pair(std::make_pair(table_id, key), hash));

		tmp->sentinel = hash;

		entry = hash;
	}
	else{
		tmp->sentinel = it->second;
		entry = it->second;
	}

	lock_t* idx = entry->tail;
	while(idx != nullptr){
		if(tmp->trx_id == idx->trx_id){
			if(tmp->lock_mode == lockmode::SHARED ){
				return tmp;
			}
			else if ( tmp->lock_mode == lockmode::EXCLUSIVE && idx->lock_mode == lockmode::EXCLUSIVE){
				return tmp;
			}
		}
		idx = idx->prev;
	}

	if(entry->head == nullptr && entry->tail == nullptr){
		entry->tail = tmp;
		entry->head = tmp;
		tmp->next = nullptr;
		tmp->prev = nullptr;
	}
	else{
		lock_t* ttmp = entry->tail;
		entry->tail = tmp;
		ttmp->next = tmp;
		tmp->prev = ttmp;
		tmp->next = nullptr;
	}
	entry->size++;

	cond.wait(lock, [&]
	{
		lock_t* idx = tmp->prev;
		if(idx == nullptr){
			return true;
		}
		while(idx != entry->head){
			if(idx->lock_mode == lockmode::EXCLUSIVE){
				return false;
			}
			idx = idx->prev;
		}
		return true;
	});

	return tmp;
}

int LockManager::lock_release(lock_t* lock_obj)
{
	std::unique_lock<std::mutex> lock(m);

	auto& entry = lock_obj->sentinel;
	entry->size--;
	if(entry->size == 0){
		entry->tail = nullptr;
		entry->head = nullptr;
	}
	else{
		if(entry->head == lock_obj && entry->tail == lock_obj){
			entry->head = nullptr;
			entry->tail = nullptr;
		}
		else if(entry->head == lock_obj){
			lock_t* ntmp = lock_obj->next;
			entry->head = ntmp;
			ntmp->prev = nullptr;
		}
		else if(entry->tail == lock_obj){
			lock_t* ptmp = lock_obj->prev;
			entry->tail = ptmp;
			ptmp->next = nullptr;
		}
		else{
			lock_t* ntmp = lock_obj->next;
			lock_t* ptmp = lock_obj->prev;
			ptmp->next = ntmp;
			ntmp->prev = ptmp;
		}
	}
	if(entry->head != nullptr){
		cond.notify_all();
	}
	delete lock_obj;

	return 0;
}

LockManager::LockManager(){
	//TODO
}
