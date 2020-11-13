#include <lock_table.h>
#include <unordered_map>
#include <utility>
#include <list>
#include <mutex>
#include <condition_variable>

std::mutex m;

struct lock_t {
	std::condition_variable cond;
	hash_t* sentinel = nullptr;
	lock_t* prev = nullptr;
	lock_t* next = nullptr;
};

struct hash_t {
	int table_id;
	int64_t key;
	lock_t* tail = nullptr;
	lock_t* head = nullptr;
	int size = 0;
};


struct pair_hash{
	template <class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2> &p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);

		return h1^h2;
	}
};

std::unordered_map<std::pair<int, int>, hash_t*, pair_hash> lock_table;

int init_lock_table()
{
	return 0;
}

lock_t* lock_acquire(int table_id, int64_t key)
{
	std::unique_lock<std::mutex> lock(m);

	auto it = lock_table.find(std::make_pair(table_id, key));
	lock_t* tmp = new lock_t;
	hash_t* entry;

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

	tmp->cond.wait(lock, [&]{ return entry->head == tmp; });

	return tmp;
}

int lock_release(lock_t* lock_obj)
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
		lock_obj->next->cond.notify_all();
	}
	delete lock_obj;

	return 0;
}
