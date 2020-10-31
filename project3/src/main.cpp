#include "db.hpp"

#include<cstdio>
// MAIN

pagenum_t printpage(pagenum_t page);
void printall(pagenum_t page);
void printallbuff();
void printbuff(std::list<Buffer>::iterator i);
void check(int tid, int res, int i);

int main( int argc, char ** argv ) {
    char* path = "/mnt/d/GitHub/ITE2038/project3/project3.db";

    init_db(10);
    int tid = open_table(path);

   int flag;
   int n = 50;

///*
   for(int i = 1; i <= n; i++){
//       printf("=====insert %d ======\n", i);
       char tmp[] = { '0' + (i % 10), 0 };
       int result = db_insert(tid, (int64_t)i, tmp);
       printf("insert ");
       check(tid, result, i);
   }
//*/
//    printallbuff();
/*
    for(int i = 1; i <= n; i++){
        char ret[120];
        int result = db_find(tid, (int64_t)i, ret);
        printf("find ");
        check(tid, result, i);
    }
*/
    printallbuff();
    

///*
   printf("===========after delete=========\n");
   for(int i = 1; i <= n ; i++){
        int r = db_delete(tid, i);
        printf("delete ");
        check(tid, r, i);
   }
//*/
   printallbuff();
   close_table(tid);


   return 0;
}

/*
pagenum_t printpage(pagenum_t page){
    Page_t* tmp = (Page_t*)malloc(sizeof(Page_t));
    file_read_page(page, tmp);
    if(page == 0){
        printf("header page\n");
        printf("free page num : %u\n", tmp->page.header.free_pnum);
        printf("root page num : %u\n", tmp->page.header.root_pnum);
        printf("num of page : %u\n", tmp->page.header.numpages);
    }

    else if(tmp->page.internal.isLeaf == 0){
        printf("internal page %u\n", page);
        printf("parent page num : %u\n", tmp->page.internal.parent_pnum);
        printf("is_leaf : %d\n", tmp->page.internal.isLeaf);
        printf("num of keys : %d\n", tmp->page.internal.numkeys);
        printf("more page number : %u\n", tmp->page.internal.more_pnum);
        for(int i = 0; i < INTERNAL_ORDER; i++){
            printf("%d key : %ld || ", i, tmp->page.internal.precord[i].key);
            printf("%d pnum : %u\n", i, tmp->page.internal.precord[i].pnum);
        }        
    }

    else if(tmp->page.internal.isLeaf == 1){
        printf("leaf page %u\n", page);
        printf("parent page num : %u\n", tmp->page.leaf.parent_pnum);
        printf("is_leaf : %d\n", tmp->page.leaf.isLeaf);
        printf("num of keys : %d\n", tmp->page.leaf.numkeys);
        printf("right sibling page number : %u\n", tmp->page.leaf.rsib_pnum);
        for(int i = 0; i < DEFAULT_ORDER; i++){
            printf("%d key : %ld   || ", i, tmp->page.leaf.record[i].key);
            printf("%d value : %s\n", i, tmp->page.leaf.record[i].value);
        }        
    }

    printf("----------------------------\n");
    free(tmp);
}

void printleaf(pagenum_t start){
    Page_t tmp;
    file_read_page(start, &tmp);
    printpage(start);
    while(tmp.page.leaf.rsib_pnum != 0){
        printf("-------------------------\n");
        file_read_page(tmp.page.leaf.rsib_pnum, &tmp);
        printpage(tmp.page.leaf.rsib_pnum);
    }
}
*/

void printallbuff(void){
    std::list<Buffer> tmp = BufferManager::getBuffmgr();
    for(auto i = tmp.begin(); i != tmp.end(); i++){
        printbuff(i);
    }
}

void printbuff(std::list<Buffer>::iterator i){
    bool printrecord = true;
    printf("buff-------------------------\n");
    printf("page %d table %d \n", i->pnum, i->table_id);
    printf("dirty %d pin %d\n", i->is_dirty, i->pincnt);
    pagenum_t& page = i->pnum;
    Page_t& tmp = i->frame;

    if(page == 0){
        printf("header page\n");
        printf("free page num : %u\n", tmp.header.free_pnum);
        printf("root page num : %u\n", tmp.header.root_pnum);
        printf("num of page : %u\n", tmp.header.numpages);
    }

    else if(tmp.internal.isLeaf == 0){
        printf("internal page %u\n", page);
        printf("parent page num : %u\n", tmp.internal.parent_pnum);
        printf("is_leaf : %d\n", tmp.internal.isLeaf);
        printf("num of keys : %d\n", tmp.internal.numkeys);
        printf("more page number : %u\n", tmp.internal.more_pnum);
        if(printrecord){
            for(int i = 0; i < 248; i++){
                printf("%d key : %ld || ", i, tmp.internal.precord[i].key);
                printf("%d pnum : %u\n", i, tmp.internal.precord[i].pnum);
            }        
        }
    }

    else if(tmp.internal.isLeaf == 1){
        printf("leaf page %u\n", page);
        printf("parent page num : %u\n", tmp.leaf.parent_pnum);
        printf("is_leaf : %d\n", tmp.leaf.isLeaf);
        printf("num of keys : %d\n", tmp.leaf.numkeys);
        printf("right sibling page number : %u\n", tmp.leaf.rsib_pnum);
        if(printrecord){
            for(int i = 0; i < 32; i++){
                printf("%d key : %ld   || ", i, tmp.leaf.record[i].key);
                printf("%d value : %s\n", i, tmp.leaf.record[i].value);
            }        
        }
    }

    printf("----------------------------\n");
}

void check(int tid, int res, int i){
    if(res == 0){
        printf("tid %d success %d\n", tid, i);
    }
    else{
        printf("tid %d fail %d\n", tid, i);
    }
}