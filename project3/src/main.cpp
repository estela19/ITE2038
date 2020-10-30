#include "db.hpp"

#include<cstdio>
// MAIN

pagenum_t printpage(pagenum_t page);
void printall(pagenum_t page);

int main( int argc, char ** argv ) {
    char* path = "/mnt/d/github/ITE2038/project3/project3.db";

    init_db(10);
    int tid = open_table(path);
   

   int n = 10;
   for(int i = 1; i <= n; i++){
       char tmp[] = { '0' + (i % 10), 0 };
       db_insert(tid, (int64_t)i, tmp);
   }

    char* ret;
   for(int i = 1; i <= n; i++){
       db_find(tid, (int64_t)i, ret);
       printf("find %s \n", ret);
   }

/*

    for(int i = 1; i <200; i++){
        char ret[120];
        db_find(i, ret);
        if(atoi(ret) == i){
            printf("success %d\n", i);
        }
        else{
            printf("%s\n", ret);
        }
    }
*/
    
   /*
    printpage(3);
    printpage(252);
   printpage(379);
   printpage(4);
*/

/*
   printf("===========after delete=========\n");
   for(int i = 2000; i >=0; i--){
       db_delete(i);
   }
    printpage(0);
*/ 

/*
    char test[120];
    db_find((int64_t)13, test);
    printf("%s\n", test);
   for(int i = 1; i < 10; i++){
       printpage(i);
   }
*/

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