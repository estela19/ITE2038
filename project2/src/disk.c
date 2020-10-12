#include "disk.h"
#include "file.h"

int open_table(char* pathname) {
    file_open(pathname);
    if (exist_file(pathname)) {
        file_read_header();
    }
    else {
        headerManager.header.free_pnum = 0;
        headerManager.header.root_pnum = 0;
        headerManager.header.numpages = 1;
        file_write_header();
    }
    return table_id++;
}

int db_insert(int64_t key, char* value) {
    Page_t* root = NULL;
    file_read_page(headerManager.header.root_pnum, root);
    int result = insert(root, key, value);
    if (headerManager.modified) {
        file_write_page(0, &(headerManager.header));
    }
    file_write_header();
    return result;
}

int db_find(int64_t key, char* ret_val) {
    Page_t* root = NULL;
    file_read_page(headerManager.header.root_pnum, root);
    Record* tmp = find(root, key);
    ret_val = tmp->value;
    file_write_header();
    return 0;
}

int db_delete(int64_t key) {

}