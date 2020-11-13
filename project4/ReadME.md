# LockTable  
전체적인 구조는 다음과 같다.  
![lock_table](uploads/deb85ba2358c306be5bfaa0a8b6f0d24/lock_table.PNG)  
  
## LockTable API  
본 프로젝트에서 제공하는 API는 다음과 같다.  
```cpp
int init_lock_table();
lock_t* lock_acquire(int table_id, int64_t key);
int lock_release(lock_t* lock_obj);
```  
**init_lock_table**  
`hash_table`등의 자료구조를 초기화해주는 함수이나,   
`stl`을 사용하여 초기화가 자동으로 진행되기 때문에 본 코드에서는 아무 동작하지 않고 `return 0`한다.  

**lock_acquire**  
`table_id`와 `key`를 인자로 받아서 해당 `thread`가 권한을 얻기위해 기다리는 함수이다.  
`linked list`의 `entry`인 `lock_t`를 생성하고 `linked list`의 `tail`에 추가해준다.  
`head`가 될 때 까지 기다리며, `head`일 때 권한을 얻는다. 본 함수에서 `conditional variable`을 이용해 `wait`를 수행한다.  

**lock_release**  
`lock_t*`을 인자로 받아서 해당 `lock_t` `entry`를 `linked list`에서 제거해주고 다음 `head`를 깨운다.  
본 함수에서 `conditional variable`을 이용해 `notify(wake up)`을 수행한다.  

## 핵심 자료구조  
#### hash  
`table_id`와 `record_id`를 하나의 `entry`로 대응 시켜야하기 때문에 `table_id`와 `record_id`를 해싱하였다. 해싱방식은 C++ stl의 `unordered_map`을 사용하였다.   
이 경우 `pair`에 대한 `hash`를 정의해주어야 하며 다음과 같이 정의하였다.  
```cpp
struct pair_hash{
	template <class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2> &p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);

		return h1^h2;
	}
};
```  
해싱의 결과로 `hash_t`와 매칭이 되는데, `hash_t`는 각 `table_id`와 `key`와 1대1 대응되고 이에 접근하는 `thread`들을 `thread shafe`하게 동작하도록 `mutex`를 걸어준다.   
`hash_t`는 다음과 같다.  
```cpp
 struct hash_t {
	int table_id;
	int64_t key;
	lock_t* tail = nullptr;
	lock_t* head = nullptr;
	int size = 0;
};
```
  
#### linked list   
초기의 경우 `std::list`를 사용하려했지만, `lock_acquire(lock_t* lock_obj)`함수에서 포인터를 인자로 받기에 `pointer`와 `iterator`의 자유로운 변경이 비교적 어려워 직접 구현하는 방식을 채택했다.  
`linked list`는 `lock_t`를 `entry`로 가지며 `lock_t` 타입은 다음과 같다.  
```c++
struct lock_t {
	std::condition_variable cond;
	hash_t* sentinel = nullptr;
	lock_t* prev = nullptr;
	lock_t* next = nullptr;
};
```  
## multi threading  
본 과제에서는 `mutex`, `lock`, `unlock`과 `condition variable`을 통한 `wait`, `awake(notify)`을 이용한다.  
  
**mutex**  
본 과제에서는 한번에 하나의 스레드만 깨어있으면 되므로 `std::mutex`를 전역변수로 선언해 사용하였다.  

**lock & unlock**  
`std::unique_lock`을 통해 `lock`과 `unlock`을 자동화하였다. 이는 `std::unique_ptr`과 동작이 거의 유사하다.   
  
**wait & notify**  
`lock_t`에 `std::condition_variable`을 두어 이를 이용해 `wait`와 `notify`를 시행한다.  
  
* wait  
`lock_acquire`함수에서 `wait`을 실행하며 해당 `hash entry`의 `head`가 자기자신일 때 까지 `lock`한다.  
  
* notify  
`lock_release`함수에서 `notify_all`을 실행한다.