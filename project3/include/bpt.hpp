#ifndef __BPT_H__
#define __BPT_H__

#include <cstdlib>
#include <algorithm>
#include <memory.h>

#include "fileAPI.hpp"
#include "page.hpp"
#include "Buffer.hpp"


class BPT{
private:
    BufferManager* buff;
    int leaf_order = 31;
    int internal_order = 248;
    int tid;
    
public:
    BPT(BufferManager* buffermanager);
    void Setid(int id);
    int Insert( Page * root, int key, const char* value );
    int Delete( Page * root, int key );
    int Find( Page * root, Record * rec, int key);

    void Settid(int tid);

private:
    int find_leaf( Page * root, Page * c, int key );
    int find( Page * root, Record * rec, int key );
    int cut( int length );

    // Insertion.

    void make_record(Record* new_record, int key, const char* value);
    void make_leaf( Page* leaf );
    int get_left_index(Page * parent, Page * left);
    int insert_into_leaf( Page * leaf, int key, Record * pointer );
    int insert_into_leaf_after_splitting(Page * root, Page * leaf, Page* new_leaf, int key,  Record * pointer);
    int insert_into_node(Page * root, Page * n, int left_index,  Precord* precord);
    int insert_into_node_after_splitting(Page * root, Page * parent, int left_index, Precord* precord, Page * right);
    int insert_into_parent(Page * root, Page * left, Precord* new_precord, Page * right);
    int insert_into_new_root(Page * left, Precord * key_record, Page* right);
    int start_new_tree(Record * pointer);


    // Deletion.

    int get_parent_index( Page * n );
    void remove_entry_from_node(Page* n, int key);
    int adjust_root(Page * root);
    int coalesce_nodes(Page * root, Page * n, Page * neighbor, int neighbor_index, int k_prime);
    int delete_entry( Page * root, Page * n, int key );


};

#endif