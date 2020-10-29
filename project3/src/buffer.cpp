#include "buffer.hpp"

class BufferManager{
private:
    std::list<Buffer>buffManager;
    int size;
    
public:
    BufferManager(int buff_size);
    int Eviction();
    void Buff_read(Page* p);
    void Buff_write(Page* p);
    void Buff_find(Page* p);

};

class Buffer{
private:
    Node_t frame;
    int table_id;
    int is_dirty;
    int is_pinned;
    Buffer* prev;
    Buffer* next;
};

BufferManager::BufferManager(int buff_size){
    size = buff_size;
}

//success return 0, fail return -1
int BufferManager::Eviction(){
    if(buffManager.empty()){
        return -1;
    }
    else if(buffManager.front().is_pinned){
        return -1;
    }
    else{
        auto it = buffManager.begin();
        while(it->pincnt != 0){
            it++;
            if(it == buffManager.end()){
                it = bufferManager.begin();
            }
        }

        if(tmp.is_dirty){
            file_write_page(tmp.frame, tmp.table_id, tmp.pnum);
        }
        buffManager.erase(it);
        return 0;
    }
}

//page* 에 read한 결과 return
void BufferManager::Buff_read(pagenum_t pnum, int tid, Page* p){
    auto& it = Buff_find(p);
    if(it == buffManager.end()){
        if(isfull()){
            Eviction();
        }

        Buff_make(tid, pnum);
    }

    setPage(p, it);

    usedbuffmove(it);
}

void BufferManager::Buff_read(int tid, pagenum_t pnum){
    auto& it = Buff_find(tid, pnum);
    if(it == buffManager.end()){
        if(isfull()){
            Eviction();
        }

        Buff_make(tid, pnum);
    }

    usedbuffmove(it);
}

void BufferManager::Buff_write(Page* p){
    auto& it = Buff_find(p);
    //find 실패에 대한 예외처리

    it->pincnt--;
    it->is_dirty = 1;
}

//future: not using page tmp
//default internal page
pagenum_t BufferManager::Alloc_page(int tid){
    Page header(tid, 0);
    pagenum_t& fnum = header.page->header.free_pnum;
    if (fnum == 0) {
        FileManager::make_free_page(&(*Buff_find(&header)));
    }
    Page tmp(tid, fnum);
    header.page->header.free_pnum = tmp.page->free.free_pnum;

    tmp.page->internal.isLeaf = false;
    tmp.page->internal.parent_pnum = 0;
    tmp.page->internal.more_pnum = 0;
    tmp.page->internal.numkeys = 0;
    return fnum;
}

void BufferManager::Free_page(Page* p){
    Page header(tid, 0);
    auto& it = Buff_find(p);
    memset(p->page, 0, PSIZE);
    p->page->free.free_pnum = header.page->header.free_pnum;
    header.page->header.free_pnum = p->pnum;
    p->is_empty = true;
//    buff_find(header)->is_dirty = 1;
    Buff_write(p);
}

auto& BufferManager::Buff_find(Page* p){
    pagenum_t pnum = p->pnum;
    int table_id = p->table_id;
    for(auto& i : buffManager){
        if(i.table_id == table_id && i.pnum == pnum){
            break;
        }
    }
    return i;
}

auto& BuffManager::Buff_find(int tid, pagenum_t pnum){
    for(auto& i : buffManager){
        if(i.table_id == tid && i.pnum == pnum){
            break;
        }
    }
    return i;
}

void BufferManager::Buff_make(int tid, pagenum_t num){
    //futere: & optimization
    Buffer& tmp = *BufferManager.emplace_back();
    tmp.pnum = num;
    tmp.table_id = tid;
    //Todo: change &type
    file_read_page(&tmp.frame, tid, num);
    //Todo: add to hash table

    /*
    Buffer tmp;
    tmp.pnum = num;
    tmp.table_id = tid;
    file_read_page(&tmp.frame, tid, num);
    buffManager.push_back(tmp);
    */
}

void BufferManager::setPage(Page* p, auto& it){
    p->page = it->frame;
    p->pnum = it->pnum;
    p->table_id = it->table_id;
    p->is_empty = false;
    //
    it->pincnt++;
}

int BufferManager::isfull(){
    if(buffManager.size() == size){
        return 1;
    }
    else{
        return 0;
    }
}

void BufferManager::usedbuffmove(auto& it){
        buffManager.push_back(*it);
        buffManager.erase(it);
}