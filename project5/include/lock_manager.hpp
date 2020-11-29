#ifndef __LOCK_MANAGER_H__
#define __LOCK_MANAGER_H__

#include <unordered_map>
#include <utility>
#include <list>
#include <mutex>
#include <condition_variable>
#include <stdint.h>

#include "types.hpp"

struct lock_t;
struct hash_t;

struct lock_t {
//	std::condition_variable cond;
	hash_t* sentinel = nullptr;
	lock_t* prev = nullptr;
	lock_t* next = nullptr;
	lockmode lock_mode;
	lock_t* next_trx = nullptr;
	int trx_id;
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

class LockManager{
public:
    std::unordered_map<std::pair<int, int>, hash_t*, pair_hash> lock_table;

public:
	static LockManager& get();
    lock_t* lock_acquire(int table_id, int64_t key, int trx_id, lockmode mode);
    int lock_release(lock_t* lock_obj);

private:
    std::mutex m;
    std::condition_variable cond;

private:
	LockManager();
};

#endif /* __LOCK_TABLE_H__ */
