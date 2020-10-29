#include "bpt.hpp"

BPT::BPT(BufferManager& buffermanager){
    buff = buffermanager;
}

void BPT::Setid(int id){
    tid = id;
}

int BPT::Find( Page * root, Record * rec, int key){
    return find(root, rec, key);
}

/* Traces the path from the root to a leaf, searching
 * by key.  Displays information about the path
 * if the verbose flag is set.
 * Returns the leaf containing the given key.
 */
int  BPT::find_leaf( Page * root, Page * c, int key) {
    int i = 0;

    if (root->is_empty) {
        return -1;
    }

    memcpy(c, root, sizeof(Node_t));
    
    while (!c->page->internal.isLeaf) {
        i = 0;
        if(c->page->internal.numkeys == 1 && c->page->internal.precord[0].pnum == 0){
            c->pnum = c->page->internal.more_pnum;
        }
        else{
            while (i < c->page->internal.numkeys) {
                if (key >= c->page->internal.precord[i].key) i++;
                else break;
            }
            i--;
            if(i == -1){
                c->pnum = c->page->internal.more_pnum;
            }
            else{
                c->pnum = c->page->internal.precord[i].pnum;
            }
        }
        buff.Buff_write(c);
        buff.Buff_read(c->pnum, tid, c);
//        file_read_page(c->pnum, &(c->page));
    }

    return 0;
}


/* Finds and returns the record to which
 * a key refers.
 */
int BPT::find( Page * root, Record * rec, int key) {
    int i = 0;
    Page c;
    int result = find_leaf(root, &c, key);
    if (result != 0) return -1;

    for (i = 0; i < c->page.page.leaf.numkeys; i++)
        if (c->page.page.leaf.record[i].key == key) break;
    if (i == c->page.page.leaf.numkeys) {
        return -1;
    }
    else{
        rec = &(c->page.page.leaf.record[i]);
        return 0;
    }
}

/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int BPT::cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}


// INSERTION
/* Creates a new record to hold the value
 * to which a key refers.
 */
void BPT::make_record(Record* new_record, int key, const char* value) {
    new_record->key = key;
    strcpy(new_record->value, value);

}



/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
void BPT::make_leaf(Page* leaf ) {
    leaf->page.page.leaf.isLeaf = true;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to 
 * the node to the left of the key to be inserted.
 */
int BPT::get_left_index(Page * parent, Page * left) {
    int left_index = 0;
    if (parent->page->internal.more_pnum == left->pnum)
        return -1;
    while (left_index <= parent->page->internal.numkeys && 
            parent->page->internal.precord[left_index].pnum != left->pnum)
        left_index++;
    return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
int BPT::insert_into_leaf( Page * leaf, int key, Record * pointer ) {

    int i, insertion_point;

    insertion_point = 0;

    while (insertion_point < leaf->page->leaf.numkeys && leaf->page->leaf.record[insertion_point].key < key)
        insertion_point++;

    for (i = leaf->page->leaf.numkeys; i > insertion_point; i--) {
        leaf->page->leaf.record[i] = leaf->page->leaf.record[i - 1];
    }
    leaf->page->leaf.record[insertion_point] = *pointer;
    leaf->page->leaf.numkeys++;

 //   file_write_page(leaf->pnum, leaf);

    return 0;
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
int BPT::insert_into_leaf_after_splitting(Page * root, Page * leaf, Page* new_leaf, int key, Record * pointer) {

    Record* temp_record;
    int insertion_index, split, i, j;
    Precord new_precord;


    temp_record = (Record*)malloc((order + 1) * sizeof(Record));


    insertion_index = 0;
    while (insertion_index < leaf_order && leaf->page->leaf.record[insertion_index].key < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->page->leaf.numkeys; i++, j++) {
        if (j == insertion_index) j++;
        temp_record[j] = leaf->page->leaf.record[i];
    }

    temp_record[insertion_index] = *pointer;

    leaf->page->leaf.numkeys = 0;

    split = cut(leaf_order);

    for (i = 0; i < split; i++) {
        leaf->page->leaf.record[i] = temp_record[i];
        leaf->page->leaf.numkeys++;
    }

    for (i = split, j = 0; i < order + 1; i++, j++) {
        new_leaf->page->leaf.record[j] = temp_record[i];
        new_leaf->page->leaf.numkeys++;
    }

    free(temp_record);

    new_leaf->page->leaf.rsib_pnum = leaf->page->leaf.rsib_pnum;
    leaf->page->leaf.rsib_pnum = new_leaf->pnum;

    //���� �θ� �� split �Ǵ°��?
    new_leaf->page->leaf.parent_pnum = leaf->page->leaf.parent_pnum;
    new_precord.key = new_leaf->page->leaf.record[0].key;
    new_precord.pnum = new_leaf->pnum;

    for(i = leaf->page->leaf.numkeys; i < leaf_order; i++){
        leaf->page->leaf.record[i].key = 0;
        memset(leaf->page->leaf.record[i].value, 0, 120);
    }
    for(i = new_leaf->page->leaf.numkeys; i < leaf_order; i++){
        new_leaf->page->leaf.record[i].key = 0;
        memset(new_leaf->page->leaf.record[i].value, 0, 120);
    }

    //file_write

    // EDITED
 //   file_write_page(new_leaf->pnum, &(new_leaf->page));
 //   file_write_page(leaf->pnum, &(leaf->page));

    return insert_into_parent(root, leaf, new_precord, new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
int BPT::insert_into_node(Page * root, Page * n, 
        int left_index, Precord* precord) {
    int i;

    for (i = n->page->internal.numkeys - 1; i > left_index; i--) {
        n->page->internal.precord[i + 1] = n->page->internal.precord[i];
    }
    n->page->internal.precord[left_index + 1] = *precord;
    n->page->internal.numkeys++;

//    file_write_page(n->pnum, n);

    return 0;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
int BPT::insert_into_node_after_splitting(Page * root, Page * old_node, int left_index, Precord* precord, 
        Page * right) {

    int i, j, split;
    Page_t * tmp;
    int * temp_keys;
    Precord* temp_precord;
    Precord prime_precord;

    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places. 
     * Then create a new node and copy half of the 
     * keys and pointers to the old node and
     * the other half to the new.
     */

    temp_precord = malloc((inorder + 1) * sizeof(Precord));
    if (temp_precord == NULL) {
        perror("Temporary precord array for splitting nodes.");
        exit(EXIT_FAILURE);
    }


    for (i = 0, j = 0; i < old_node->page->internal.numkeys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_precord[j] = old_node->page->internal.precord[i];
    }

    temp_precord[left_index + 1] = *precord;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
    split = cut(internal_order);
    Page new_node(tid, Alloc_page(tid));
    old_node->page->internal.numkeys = 0;

    for (i = 0; i < split; i++) {
        old_node->page->internal.precord[i] = temp_precord[i];
        old_node->page->internal.numkeys++;
    }

    memcpy(prime_precord, &temp_precord[split], sizeof(Precord));
    new_node.page->internal.more_pnum = temp_precord[split].pnum;
    prime_precord.pnum = new_node->pnum;

    for (++i, j = 0; i < inorder + 1; i++, j++) {
        new_node.page->internal.precord[j] = temp_precord[i];
        new_node.page->internal.numkeys++;
    }

    free(temp_precord);

    new_node.page->internal.parent_pnum = old_node->page->internal.parent_pnum;

    //TODO : optimization
    for (i = 0; i < new_node.page->internal.numkeys; i++) {
        Page tmp(tid, new_node.page->internal.precord[i].pnum);
        tmp.page->internal.parent_pnum = new_node.pnum;
//        file_read_page(new_node->page.page.internal.precord[i].pnum, tmp);
//        tmp->page.internal.parent_pnum = new_node->pnum;
//        file_write_page(new_node->page.page.internal.precord[i].pnum, tmp);
    }

    Page tmp(tid, new_node.page->internal.more_pnum);
    tmp.page->internal.parent_pnum = new_node.pnum;

//    file_read_page(new_node->page.page.internal.more_pnum, tmp);
//    tmp->page.internal.parent_pnum = new_node->pnum;
//    file_write_page(new_node->page.page.internal.more_pnum, tmp);


    for(i = old_node->page->internal.numkeys; i < internal_order; i++){
        old_node->page->internal.precord[i].key = 0;
        old_node->page->internal.precord[i].pnum = 0;       
    }
    for(i = new_node.page->internal.numkeys; i < internal_order; i++){
        new_node.page->internal.precord[i].key = 0;
        new_node.page->internal.precord[i].pnum = 0;
    }
    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */

    return insert_into_parent(root, old_node, prime_precord, &new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
int BPT::insert_into_parent(Page * root, Page * left, Precord* new_precord, Page * right) {

    int left_index;

    /* Case: new root. */

    if (left->page->internal.parent_pnum == 0)
        return insert_into_new_root(left, new_precord, right);

    Page parent(tid, left->page->leaf.parent_pnum);
//    parent->pnum = left->page.page.leaf.parent_pnum;
//    file_read_page(parent->pnum, &(parent->page));

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */

    left_index = get_left_index(&parent, left);

    /* Simple case: the new key fits into the node. 
     */

    if (parent->page->internal.numkeys < internal_order){
 //       file_write_page(left->pnum, &(left->page));
 //       file_write_page(right->pnum, &(right->page));
        return insert_into_node(root, &parent, left_index, new_precord);
    }

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */

    return insert_into_node_after_splitting(root, &parent, left_index, new_precord, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
int BPT::insert_into_new_root(Page * left, Precord* key_record, Page* right) {

    Page root(tid, buff.Alloc_page(tid));
    Page header(tid, 0);
    header.page->header.root_pnum = root->pnum;


    root.page->internal.parent_pnum = 0;
    root.page->internal.more_pnum = left->pnum;
    root.page->internal.precord[0] = *key_record;
    root.page->internal.numkeys++;

    left.page->leaf.parent_pnum = root->pnum;
    right.page->leaf.parent_pnum = root->pnum;

//    file_write_page(root->pnum, &(root->page));
//    file_write_page(left->pnum, &(left->page));
//    file_write_page(right->pnum, &(right->page));

    return 0;
}



/* First insertion:
 * start a new tree.
 */
int BPT::start_new_tree( Record * pointer) {
    Page root(tid, buff.Alloc_page(tid));
    make_leaf(&root);

    root->page.leaf.record[0] = *pointer;
    root->page.leaf.numkeys++;

    Page header(tid, 0);
    header.page->header.root_pnum = root->pnum;

    return 0;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
int BPT::Insert( Page * root, int key, const char* value ) {

    Record pointer;
    Page leaf;

    /* The current implementation ignores
     * duplicates.
     */

    Record* r;
    if (find(root, r, key) != 0)
        return -1;

    /* Create a new record for the
     * value.
     */
    make_record(&pointer, key, value);


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root->is_empty) {
        return start_new_tree(&pointer);
    }


    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    find_leaf(root, &leaf, key);

    /* Case: leaf has room for key and pointer.
     */

    if (leaf.page->leaf.numkeys < order) {
        return insert_into_leaf(&leaf, key, &pointer);
    }


    /* Case:  leaf must be split.
     */

    Page new_leaf(tid, buff.Alloc_page(tid));
    make_leaf(&new_leaf);
    return insert_into_leaf_after_splitting(root, &leaf, &new_leaf, key, pointer);
    
}




// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int BPT::get_parent_index( Page * n ) {

    int i;
    Page parent(tid, n->page->internal.parent_pnum);

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
    

    for (i = 0; i < parent.page->internal.numkeys; i++)
        if (parent.page->internal.more_pnum == n->pnum)
            return -1;
        else if (parent.page->internal.precord[i].pnum == n->pnum)
            return i;
}


void BPT::remove_entry_from_node(Page* n, int key) {

    int i, num_pointers;

    // Remove the key and shift other keys accordingly.
    i = 0;
    if(n->page->leaf.isLeaf){
        while (n->page->leaf.record[i].key != key)
            i++;
        for (++i; i < n->page->leaf.numkeys; i++)
            n->page->leaf.record[i - 1] = n->page->leaf.record[i];
    }
    else if(!n->page->internal.isLeaf){
        while(n->page->internal.precord[i].key != key)
            i++;
        for(++i; i < n->page->internal.numkeys; i++){
            n->page->internal.precord[i - 1] = n->page->internal.precord[i];
        }
    }

    // One key fewer.
    n->page->internal.numkeys--;

    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
    if (n->page->internal.isLeaf) {
        for (i = n->page->leaf.numkeys; i < leaf_order; i++) {
            n->page->leaf.record[i].key = 0;
            memset(n->page->leaf.record[i].value, 0, 120);
        }
    }
    else {
        for (i = n->page->internal.numkeys; i < internal_order; i++) {
            n->page->internal.precord[i].key = 0;
            n->page->internal.precord[i].pnum = 0;
        }
    }
//    file_write_page(n->pnum, &(n->page));

}


int BPT::adjust_root(Page * root) {

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

    if (root->page->internal.numkeys > 0){
//        file_write_page(root->pnum, &(root->page));
        return 0;
    }

    /* Case: empty root.
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!root->page->internal.isLeaf) {
        Page new_root(tid, root->page->internal.more_pnum);
        new_root.page->leaf.parent_pnum = 0;

        buff.Free_page(root);
//        file_free_page(root->pnum);
        
        //change header
        Page header(tid, 0);
        header.page->header.root_pnum = new_root.pnum;
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else {
        Page header(tid, 0);
        header.page->header.root_pnum = 0;
        buff.Free_page(root);
//        file_free_page(root->pnum);
    }

    return 0;
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
int BPT::coalesce_nodes(Page * root, Page * n, Page * neighbor, int index, int k_prime) {

//    printf("==Kprime: %d\n", k_prime);

    int k;
    int i, j, neighbor_insertion_index;
    int parent_num;


    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    neighbor_insertion_index = neighbor->page->internal.numkeys;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!n->page->internal.isLeaf) {

        /* Append k_prime.
         */
        if (index != -1) {
            neighbor->page->internal.precord[neighbor_insertion_index].key = k_prime;
            neighbor->page->internal.precord[neighbor_insertion_index].pnum = n->page->internal.more_pnum;
            neighbor->page->internal.numkeys++;

            buff.Free_page(n);    
//            file_free_page(n->pnum);
//            file_write_page(neighbor->pnum, &(neighbor->page));

            Page more_page(tid, n->page->internal.more_pnum);
            more_page.page->internal.parent_pnum = neighbor->pnum;

            parent_num = neighbor->page->internal.parent_pnum;
        }
        else {
            n->page->internal.precord[0].key = k_prime;
            n->page->internal.precord[0].pnum = neighbor->page->internal.more_pnum;

            Page more_page(tid, neighbor->page->internal.more_pnum);
            more_page.page->internal.parent_pnum = n->pnum;
//            file_write_page(more_page.pnum, &(more_page.page));

            for (i = 0; i < neighbor->page->internal.numkeys; i++) {
                n->page->internal.precord[i + 1] = neighbor->page->internal.precord[i];

                Page tmp(tid, neighbor->page->internal.precord[i].pnum);
                tmp.page->internal.parent_pnum = n->pnum;
            }
            n->page->internal.numkeys = neighbor->page->internal.numkeys + 1;

            buff.Free_page(neighbor);
//            file_free_page(neighbor->pnum);
//            file_write_page(n->pnum, &(n->page));

            parent_num = n->page->internal.parent_pnum;
        }

    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        if (index != -1) {
            neighbor->page->leaf.rsib_pnum = n->page->leaf.rsib_pnum;
            
            buff.Free_page(n);
            //file_free_page(n->pnum);
            //file_write_page(neighbor->pnum, &(neighbor->page));

            parent_num = neighbor->page->internal.parent_pnum;
        }
        else {
            for (i = 0; i < neighbor->page->leaf.numkeys; i++) {
                n->page->leaf.record[i] = neighbor->page->leaf.record[i];
                ++n->page->leaf.numkeys;
                --neighbor->page->leaf.numkeys;
            }
            n->page->leaf.rsib_pnum = neighbor->page->leaf.rsib_pnum;

            buff.Free_page(neighbor);
//            file_write_page(n->pnum, &(n->page));

            parent_num = n->page->internal.parent_pnum;
        }

    }

    Page parent(tid, parent_num);

    return delete_entry(root, &parent, k_prime);

}


/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
int BPT::delete_entry( Page * root, Page * n, int key ) {


    int min_keys;
    int index;
    int k_prime_index, k_prime;
    int capacity;

    // Remove key and pointer from node.m

    remove_entry_from_node(n, key);

    /* Case:  deletion from the root. 
     */

    if (n->pnum == root->pnum) {
        memcpy(root, &(n->page), 4096);
        return adjust_root(root);
    }


    /* Case:  deletion from a node below the root.
     * (Rest of function body.)
     */

    /* Determine minimum allowable size of node,
     * to be preserved after deletion.
     */

    //delay merged
    min_keys = 0;

    /* Case:  node stays at or above minimum.
     * (The simple case.)
     */

    if (n->page->internal.numkeys > min_keys)
        return 0;

    /* Case:  node falls below minimum.
     * Either coalescence or redistribution
     * is needed.
     */

    /* Find the appropriate neighbor node with which
     * to coalesce.
     * Also find the key (k_prime) in the parent
     * between the pointer to node n and the pointer
     * to the neighbor.
     */

    Page parent(tid, n->page->internal.parent_pnum);
    Page neighbor;

    index = get_parent_index( n );
    k_prime_index = index == -1 ? 0 : index;
    k_prime = parent.page->internal.precord[k_prime_index].key;

    Page neighbor;

    if (index == -1) {
        buff.Buff_read(parent.page->internal.precord[0].pnum, tid, &neighbor);
    }
    else if(index == 0){
        buff.Buff_read(parent.page->internal.more_pnum, tid, &neighbor);
    }
    else {
        buff.Buff_read(parent.page->internal.precord[index - 1].pnum, tid, &neighbor);
    }


    /* Coalescence. */
    //Merge
    if (n->page->leaf.numkeys <= min_keys)
        return coalesce_nodes(root, n, neighbor, index, k_prime);

    /* Redistribution. */

    //else
    //    return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}



/* Master deletion function.
 */
int BPT::Delete(Page * root, int key) {

    Page key_leaf;
    Record * key_record;

    if (!find(root, key_record, key) && !find_leaf(root, &key_leaf, key)){
        return delete_entry(root, key_leaf, key);
    }
    else{
        return -1;
    }
}

