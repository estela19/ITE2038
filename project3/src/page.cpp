#include "page.hpp"


Page::Page(){
    this.is_empty = true;
}

Page::Page(int tid, pagenum_t pnum){
    Buff_read(pnum, tid, &this);
    this.pnum = pnum;
    this.tid = tid;
    this.is_empty = false;
}

Page::~Page(){
    if(!is_empty){
        Buff_write(&this);
    }
}

//maybe not used

Page_t& Page::getPage(){
    return page;
}

pagenum_t Page::getPagenum(){
    return pnum;
}

int Page::getParentnum(){
    return page.internal.parent_pnum;
}

int Page::getIsleaf(){
    return page.internal.isLeaf;
}

int Page::getNumkeys(){
    return page.internal.numkeys;
}

pagenum_t Page::getRsibnum(){
    if(getIsleaf()){
        return page.leaf.rsib_pnum;
    }
    else{
        return 0;
    }
}

pagenum_t Page::getMorepnum(){
    if(getIsleaf()){
        return 0;
    }
    else{
        return page.internal.more_pnum;
    }
}

Precord* Page::getPrecord(){
    if(getIsleaf()){
        return nullptr;
    }
    else{
        return page.internal.precord;
    }
}

Record* Page::getRecord(){
    if(getIsleaf()){
        return page.leaf.record;
    }
    else{
        return nullptr;
    }
}

void Page::setPagenum(pagenum_t num){

}
