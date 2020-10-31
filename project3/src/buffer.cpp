#include "buffer.hpp"

#define DBG(x, y) if(false) {printf("%s  p: %d, pin: %d\n", __FUNCTION__, x, y);}

BufferManager::BufferManager(FileManager* filemanager, int buff_size){
    file = filemanager;
    size = buff_size;
}

//success return 0, fail return -1
int BufferManager::Eviction(){
    if(buffManager.empty()){
        return -1;
    }
    else if(buffManager.front().pincnt){
        return -1;
    }
    else{
        auto it = buffManager.begin();
        while(it->pincnt != 0){
            it++;
            if(it == buffManager.end()){
                it = buffManager.begin();
            }
        }

        if(it->is_dirty){
            file->file_write_page(&(it->frame), it->table_id, it->pnum);
        }
        buffManager.erase(it);
        return 0;
    }
}

//page* 에 read한 결과 return
void BufferManager::Buff_read(pagenum_t pnum, int tid, Page* p){
    std::list<Buffer>::iterator it = Buff_find(p);
    if(it == buffManager.end()){
        if(isfull()){
            if(pnum == 133 && tid == 1) {
                printf("error");
            }
            int result = Eviction();
        }

        it = Buff_make(tid, pnum);
    }
    //erase
    it = usedbuffmove(it);
    setPage(p, it);

    DBG(it->pnum, it->pincnt);
//    printf("BUff_read p: %d, pin: %d\n", it->pnum, it->pincnt);
}

/*
//maybe notused
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

*/
void BufferManager::Buff_write(Page* p){
    std::list<Buffer>::iterator it = Buff_find(p);
    //find 실패에 대한 예외처리

    it->pincnt--;
    it->is_dirty = 1;

    DBG(it->pnum, it->pincnt);
//    printf("BUff_write p: %d, pin: %d\n", it->pnum, it->pincnt);
}

//future: not using page tmp
//default internal page
pagenum_t BufferManager::Alloc_page(int tid){
    Page header(tid, 0);
    pagenum_t fnum = header.page->header.free_pnum;
    if (fnum == 0) {
        file->make_free_page(&header);
    }
    fnum = header.page->header.free_pnum;
    Page tmp(tid, fnum);
    header.page->header.free_pnum = tmp.page->free.free_pnum;

    //maybe not useful 
    tmp.page->internal.isLeaf = false;
    tmp.page->internal.parent_pnum = 0;
    tmp.page->internal.more_pnum = 0;
    tmp.page->internal.numkeys = 0;
    return fnum;
}

void BufferManager::Free_page(Page* p){
    Page header(p->table_id, 0);
    std::list<Buffer>::iterator it = Buff_find(p);
//    memset(&(p->page), 0, PSIZE);
    //initialize
    p->page->internal.isLeaf = 0;
    p->page->internal.more_pnum = 0;
    p->page->internal.numkeys = 0;
 
    p->page->free.free_pnum = header.page->header.free_pnum;
    header.page->header.free_pnum = p->pnum;
    p->is_empty = true;
//    buff_find(header)->is_dirty = 1;
    Buff_write(p);
}

void BufferManager::Write_Buffers(int tid){
    for(auto& i : buffManager){
        if(i.table_id == tid && i.is_dirty){
            while(i.pincnt > 0);
            file->file_write_page(&(i.frame), tid, i.pnum);
        }
    }
}

std::list<Buffer>& BufferManager::getBuffmgr(){
    return buffManager;
}

std::list<Buffer>::iterator BufferManager::Buff_find(Page* p){
    pagenum_t pnum = p->pnum;
    int table_id = p->table_id;
    std::list<Buffer>::iterator i;
    for(i = buffManager.begin(); i != buffManager.end(); i++){
        if(i->table_id == table_id && i->pnum == pnum){
            break;
        }
    }
    return i;
}

/*
//maybe not used
auto& BuffManager::Buff_find(int tid, pagenum_t pnum){
    for(auto& i : buffManager){
        if(i.table_id == tid && i.pnum == pnum){
            break;
        }
    }
    return i;
}
*/

std::list<Buffer>::iterator BufferManager::Buff_make(int tid, pagenum_t num){
    //futere: & optimization
    Buffer& tmp = buffManager.emplace_back();
    tmp.pnum = num;
    tmp.table_id = tid;
    //Todo: change &type
    file->file_read_page(&tmp.frame, tid, num);
    //Todo: add to hash table
    return --buffManager.end();
}

void BufferManager::setPage(Page* p, std::list<Buffer>::iterator it){
    p->page = &(it->frame);
    p->pnum = it->pnum;
    p->table_id = it->table_id;
    p->is_empty = false;
    
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

std::list<Buffer>::iterator BufferManager::usedbuffmove(std::list<Buffer>::iterator it){
//    buffManager.push_back(*it);
//    buffManager.erase(it);
    buffManager.splice(buffManager.end(), buffManager, it);
    return  --buffManager.end();
}