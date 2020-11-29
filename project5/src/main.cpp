#include "db.hpp"
#include "trxAPI.hpp"

#include <cstdio>
#include <thread>
// MAIN

pagenum_t printpage(pagenum_t page);
void printall(pagenum_t page);
//void printallbuff();
void printbuff(std::list<Buffer>::iterator i);
void check(int tid, int res, int i);

int trx1(){
    int trx_id = trx_begin();
    int table_id = 1;
    char val[120];
    int key = 38;
    int result;


    result = db_find(table_id, key, val, trx_id);
    if(result != 0){
        printf("1.1 error");
        printf(" %c\n", val[0]);
    }

    result = db_find(table_id, key, val, trx_id);
    if(result != 0){
        printf("1.2 error");
        printf(" %c\n", val[0]);
    }

    result = db_find(table_id, key, val, trx_id);
    if(result != 0){
        printf("1.3 error");
        printf(" %c\n", val[0]);
    }

    result = db_find(table_id, key, val, trx_id);
    if(result != 0){
        printf("1.4 error");
        printf(" %c\n", val[0]);
    }
  
    trx_commit(trx_id);
}

int trx2(){
    int trx_id = trx_begin();
    int table_id = 1;
    char val[120];
    int key = 38;
    int result;

    
    result = db_find(table_id, 32, val, trx_id);
    if(result != 0) printf("error");


    result = db_update(table_id, 31, val, trx_id);
    if(result != 0) printf("error");

    
    trx_commit(trx_id);
}

int trx3(){
    int trx_id = trx_begin();
    int table_id = 1;
    char val[120];
    int key = 38;
    int result;
    
    val[0] = '#';
    result = db_update(table_id, 32, val, trx_id);
    if(result != 0){
        printf("3.1 error");
    }

    result = db_update(table_id, 31, val, trx_id);
    if(result != 0) printf("error");

    result = db_find(table_id, 32, val, trx_id);
    if(result != 0){
        printf("3.2 error");
        printf(" %c\n", val[0]);
    }
  
    trx_commit(trx_id);
}



int main( int argc, char ** argv ) {
    char* path1 = "/mnt/d/GitHub/ITE2038/project5/project5.db";

    init_db(20);
    int tid1 = open_table(path1);

/*
   int n = 100;


   for(int i = 1; i <= n; i++){
       char tmp[] = { '0' + (i % 10), 0 };
       int result = db_insert(tid1, (int64_t)i, tmp);
       printf("insert ");
       check(tid1, result, i);
   }

   shutdown_db();
*/

    std::thread thread1(trx2);
    std::thread thread2(trx3);

    thread1.join();
    thread2.join();

    shutdown_db();

    printf("success\n");

   return 0;
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