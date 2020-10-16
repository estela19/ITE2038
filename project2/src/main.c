#include "disk.h"
#include "bpt.h"
#include "file.h"

#include<stdio.h>
// MAIN

pagenum_t printpage(pagenum_t page);
void printall(pagenum_t page);

int main( int argc, char ** argv ) {
    const char* path = "/mnt/d/github/ITE2038/project2/project2.db";

    
    open_table(path);
   
    Page_t root;

   int n = 20;
   for(int i = 1; i <= n; i++){
       char tmp = (i % 10) + '0';
       if(i == 33){
           int j = 2;
       }
       db_insert((int64_t)i, &tmp);
   }
    db_delete(3);
    file_read_page(headerManager.header.root_pnum, &root);

/*
    char test[120];
    db_find((int64_t)13, test);
    printf("%s\n", test);
*/
   printpage(headerManager.header.root_pnum);
   /*
   for(int i = 1; i < 10; i++){
       printpage(i);
   }
   */
//   /*
//   printpage(1);
//   printpage(252);
//   printpage(63);
//*/
//   printleaf(1);
//   printall(headerManager.header.root_pnum);

    printf("success\n");

   return 0;
}

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
            printf("%d key : %ld //", i, tmp->page.internal.precord[i].key);
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
            printf("%d key : %ld   // ", i, tmp->page.leaf.record[i].key);
            printf("%d value : %s\n", i, tmp->page.leaf.record[i].value);
        }        
    }

    printf("----------------------------\n");
    free(tmp);
}

void printall(pagenum_t num){
    Page_t* tmp = (Page_t*)malloc(sizeof(Page_t));
    file_read_page(num, tmp);

    printpage(tmp->page.internal.more_pnum);
    for(int i = 0; i < tmp->page.internal.numkeys; i++){
        printpage(tmp->page.internal.precord[i].pnum);
    }
    /*
    if(page->page.leaf.isLeaf == 1){
        for(int i = 0; i < page->page.leaf.numkeys; i++){
        }
    }

    printall(page->page.internal.more_pnum);
    for(int i = 0; i < page->page.internal.numkeys; i++){
        printall(page->page.internal.precord[i].pnum);
    }
    */
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