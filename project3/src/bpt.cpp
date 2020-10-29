#include "bpt.h"
#include "file.h"
#include "disk.h"



/* Traces the path from the root to a leaf, searching
 * by key.  Displays information about the path
 * if the verbose flag is set.
 * Returns the leaf containing the given key.
 */
int  find_leaf( Page * root, Page * c, int key) {
    int i = 0;

    if (root->is_empty) {
        return -1;
    }

    memcpy(c, root, sizeof(Node_t));

    while (!c->page.page.internal.isLeaf) {
        i = 0;
        if(c->page.page.internal.numkeys == 1 && c->page.page.internal.precord[0].pnum == 0){
            c->pnum = c->page.page.internal.more_pnum;
        }
        else{
            while (i < c->page.page.internal.numkeys) {
                if (key >= c->page.page.internal.precord[i].key) i++;
                else break;
            }
            i--;
            if(i == -1){
                c->pnum = c->page.page.internal.more_pnum;
            }
            else{
                c->pnum = c->page.page.internal.precord[i].pnum;
            }
        }
        file_read_page(c->pnum, &(c->page));
    }

    return 0;
}


/* Finds and returns the record to which
 * a key refers.
 */
int find( Page * root, Record * rec, int key) {
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
int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}


// INSERTION
/* Creates a new record to hold the value
 * to which a key refers.
 */
void make_record(Record* new_record, int key, const char* value) {
    new_record->key = key;
    strcpy(new_record->value, value);

}


/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
Node_t * make_node(Node_t* new_node) {
    new_node->page.page.internal.isLeaf = false;
    new_node->page.page.internal.parent_pnum = 0;
    new_node->page.page.internal.more_pnum = 0;
    new_node->pnum = file_alloc_page();

    return new_node;
}

/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
void make_leaf(Node_t* leaf ) {
    make_node(leaf);
    leaf->page.page.leaf.isLeaf = true;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to 
 * the node to the left of the key to be inserted.
 */
int get_left_index(Node_t * parent, Node_t * left) {

    int left_index = 0;
    if (parent->page.page.internal.more_pnum == left->pnum)
        return -1;
    while (left_index <= parent->page.page.internal.numkeys && 
            parent->page.page.internal.precord[left_index].pnum != left->pnum)
        left_index++;
    return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
Node_t * insert_into_leaf( Node_t * leaf, int key, Record * pointer ) {

    int i, insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->page.page.leaf.numkeys && leaf->page.page.leaf.record[insertion_point].key < key)
        insertion_point++;

    for (i = leaf->page.page.leaf.numkeys; i > insertion_point; i--) {
        leaf->page.page.leaf.record[i] = leaf->page.page.leaf.record[i - 1];
    }
    leaf->page.page.leaf.record[insertion_point] = *pointer;
    leaf->page.page.leaf.numkeys++;

    file_write_page(leaf->pnum, leaf);

    return leaf;
}


/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
Node_t * insert_into_leaf_after_splitting(Node_t * root, Node_t * leaf, int key, Record * pointer) {

    Node_t * new_leaf;
    Record* temp_record;
    int insertion_index, split, i, j;
    Precord* new_precord;

    new_leaf = make_leaf();

    temp_record = (Record*)malloc((order + 1) * sizeof(Record));
    if (temp_record == NULL) {
        perror("Temporary record array.");
        exit(EXIT_FAILURE);
    }

    new_precord = (Precord*)malloc(sizeof(Precord));
    if (new_precord == NULL) {
        perror("new preocrd.");
        exit(EXIT_FAILURE);
    }

    insertion_index = 0;
    while (insertion_index < order && leaf->page.page.leaf.record[insertion_index].key < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->page.page.leaf.numkeys; i++, j++) {
        if (j == insertion_index) j++;
        temp_record[j] = leaf->page.page.leaf.record[i];
    }

    temp_record[insertion_index] = *pointer;

    leaf->page.page.leaf.numkeys = 0;

    split = cut(order);

    for (i = 0; i < split; i++) {
        leaf->page.page.leaf.record[i] = temp_record[i];
        leaf->page.page.leaf.numkeys++;
    }

    for (i = split, j = 0; i < order + 1; i++, j++) {
        new_leaf->page.page.leaf.record[j] = temp_record[i];
        new_leaf->page.page.leaf.numkeys++;
    }

    free(temp_record);

    new_leaf->page.page.leaf.rsib_pnum = leaf->page.page.leaf.rsib_pnum;
    leaf->page.page.leaf.rsib_pnum = new_leaf->pnum;

    //���� �θ� �� split �Ǵ°��?
    new_leaf->page.page.leaf.parent_pnum = leaf->page.page.leaf.parent_pnum;
    new_precord->key = new_leaf->page.page.leaf.record[0].key;
    new_precord->pnum = new_leaf->pnum;

    for(i = leaf->page.page.leaf.numkeys; i < order; i++){
        leaf->page.page.leaf.record[i].key = 0;
        // EDITED
        // memset(leaf->page.page.leaf.record[i].value, 0, sizeof(120));
        memset(leaf->page.page.leaf.record[i].value, 0, 120);
    }
    for(i = new_leaf->page.page.leaf.numkeys; i < order; i++){
        new_leaf->page.page.leaf.record[i].key = 0;
        // EDITED
        // memset(new_leaf->page.page.leaf.record[i].value, 0, sizeof(120));
        memset(new_leaf->page.page.leaf.record[i].value, 0, 120);
    }

    //file_write

    // EDITED
    file_write_page(new_leaf->pnum, &(new_leaf->page));
    file_write_page(leaf->pnum, &(leaf->page));

    return insert_into_parent(root, leaf, new_precord, new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
Node_t * insert_into_node(Node_t * root, Node_t * n, 
        int left_index, Precord* precord) {
    int i;

    for (i = n->page.page.internal.numkeys - 1; i > left_index; i--) {
        n->page.page.internal.precord[i + 1] = n->page.page.internal.precord[i];
    }
    n->page.page.internal.precord[left_index + 1] = *precord;
    n->page.page.internal.numkeys++;

    file_write_page(n->pnum, n);

    return root;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
Node_t * insert_into_node_after_splitting(Node_t * root, Node_t * old_node, int left_index, Precord* precord, 
        Node_t * right) {

    int i, j, split;
    Node_t * new_node;
    Page_t * tmp;
    int * temp_keys;
    Precord* temp_precord;
    Precord* prime_precord;

    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places. 
     * Then create a new node and copy half of the 
     * keys and pointers to the old node and
     * the other half to the new.
     */

    temp_precord = malloc((inorder + 1) * sizeof(Precord) );
    if (temp_precord == NULL) {
        perror("Temporary precord array for splitting nodes.");
        exit(EXIT_FAILURE);
    }

    prime_precord = malloc(sizeof(Precord));
    if (prime_precord == NULL) {
        perror("Prime precord array for splitting nodes.");
        exit(EXIT_FAILURE);
    }

    tmp = malloc(sizeof(Page_t));
    if (tmp == NULL) {
    perror("Tmp for splitting nodes.");
    exit(EXIT_FAILURE);
    }

    for (i = 0, j = 0; i < old_node->page.page.internal.numkeys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_precord[j] = old_node->page.page.internal.precord[i];
    }

    temp_precord[left_index + 1] = *precord;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */  
    split = cut(inorder);
    new_node = make_node();
    old_node->page.page.internal.numkeys = 0;

    for (i = 0; i < split; i++) {
        old_node->page.page.internal.precord[i] = temp_precord[i];
        old_node->page.page.internal.numkeys++;
    }

    // EDITED
    // prime_precord = &temp_precord[split];
    memcpy(prime_precord, &temp_precord[split], sizeof(Precord));
    new_node->page.page.internal.more_pnum = temp_precord[split].pnum;
    prime_precord->pnum = new_node->pnum;

    for (++i, j = 0; i < inorder + 1; i++, j++) {
        new_node->page.page.internal.precord[j] = temp_precord[i];
        new_node->page.page.internal.numkeys++;
    }

    free(temp_precord);

    new_node->page.page.internal.parent_pnum = old_node->page.page.internal.parent_pnum;

    //TODO : optimization
    for (i = 0; i < new_node->page.page.internal.numkeys; i++) {
        file_read_page(new_node->page.page.internal.precord[i].pnum, tmp);
        tmp->page.internal.parent_pnum = new_node->pnum;
        file_write_page(new_node->page.page.internal.precord[i].pnum, tmp);
    }

    file_read_page(new_node->page.page.internal.more_pnum, tmp);
    tmp->page.internal.parent_pnum = new_node->pnum;
    file_write_page(new_node->page.page.internal.more_pnum, tmp);

    free(tmp);

    for(i = old_node->page.page.internal.numkeys; i < inorder; i++){
        old_node->page.page.internal.precord[i].key = 0;
        old_node->page.page.internal.precord[i].pnum = 0;       
    }
    for(i = new_node->page.page.internal.numkeys; i < inorder; i++){
        new_node->page.page.internal.precord[i].key = 0;
        new_node->page.page.internal.precord[i].pnum = 0;
    }
    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */

    return insert_into_parent(root, old_node, prime_precord, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
Node_t * insert_into_parent(Node_t * root, Node_t * left, Precord* new_precord, Node_t * right) {

    int left_index;
    Node_t * parent = (Node_t*)malloc(sizeof(Node_t));

    /* Case: new root. */

    if (left->page.page.internal.parent_pnum == 0)
        return insert_into_new_root(left, new_precord, right);

    parent->pnum = left->page.page.leaf.parent_pnum;
    file_read_page(parent->pnum, &(parent->page));

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */

    left_index = get_left_index(parent, left);

    /* Simple case: the new key fits into the node. 
     */

    if (parent->page.page.internal.numkeys < inorder){
        file_write_page(left->pnum, &(left->page));
        file_write_page(right->pnum, &(right->page));
        int k = 0;
        if(parent->pnum == 253){
            k = 1;
        }
        return insert_into_node(root, parent, left_index, new_precord);
    }

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */

    return insert_into_node_after_splitting(root, parent, left_index, new_precord, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
Node_t * insert_into_new_root(Node_t * left, Precord* key_record, Node_t* right) {

    Node_t * root = make_node();

    headerManager.header.root_pnum = root->pnum;
    headerManager.modified = true;

    root->page.page.internal.parent_pnum = 0;
    root->page.page.internal.more_pnum = left->pnum;
    root->page.page.internal.precord[0] = *key_record;
    root->page.page.internal.numkeys++;

    left->page.page.leaf.parent_pnum = root->pnum;
    right->page.page.leaf.parent_pnum = root->pnum;

    file_write_page(root->pnum, &(root->page));
    file_write_page(left->pnum, &(left->page));
    file_write_page(right->pnum, &(right->page));

    return root;
}



/* First insertion:
 * start a new tree.
 */
int start_new_tree( Record * pointer) {
    Node_t root;
    make_leaf(&root);
    root->page.page.leaf.parent_pnum = 0;
    root->page.page.leaf.rsib_pnum = 0;
    root->page.page.leaf.record[0] = *pointer;
    root->page.page.leaf.numkeys++;

    headerManager.header.root_pnum = root->pnum;
    headerManager.modified = true;
    file_write_page(root->pnum, root);

    return 0;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
int insert( Page * root, int key, const char* value ) {

    Record pointer;
    Page leaf;

    /* The current implementation ignores
     * duplicates.
     */

    Record r;
    if (find(root, &r, key) != 0)
        return -1;

    /* Create a new record for the
     * value.
     */
    make_record(&pointer, key, value);


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root->is_empty) {
        return start_new_tree(pointer);
    }


    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    leaf = find_leaf(root, key);

    /* Case: leaf has room for key and pointer.
     */

    if (leaf->page.page.leaf.numkeys < order) {
        leaf = insert_into_leaf(leaf, key, pointer);
        return 0;
    }


    /* Case:  leaf must be split.
     */

    insert_into_leaf_after_splitting(root, leaf, key, pointer);
    return 0;
}




// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_parent_index( Node_t * n ) {

    int i;
    Node_t parent;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
    
    parent.pnum = n->page.page.internal.parent_pnum;
    file_read_page(parent.pnum, &(parent.page));
    for (i = 0; i < parent.page.page.internal.numkeys; i++)
        if (parent.page.page.internal.more_pnum == n->pnum)
            return -1;
        else if (parent.page.page.internal.precord[i].pnum == n->pnum)
            return i;

    // Error state.
    printf("Search for nonexistent pointer to node in parent.\n");
    printf("Node:  %#lx\n", (unsigned long)n);
    exit(EXIT_FAILURE);
}


Node_t * remove_entry_from_node(Node_t * n, int key) {

    int i, num_pointers;

    // Remove the key and shift other keys accordingly.
    i = 0;
    if(n->page.page.leaf.isLeaf){
        while (n->page.page.leaf.record[i].key != key)
            i++;
        for (++i; i < n->page.page.leaf.numkeys; i++)
            n->page.page.leaf.record[i - 1] = n->page.page.leaf.record[i];
    }
    else if(!n->page.page.internal.isLeaf){
        while(n->page.page.internal.precord[i].key != key)
            i++;
        for(++i; i < n->page.page.internal.numkeys; i++){
            n->page.page.internal.precord[i - 1] = n->page.page.internal.precord[i];
        }
    }

    // One key fewer.
    n->page.page.internal.numkeys--;

    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
    if (n->page.page.internal.isLeaf) {
        for (i = n->page.page.leaf.numkeys; i < order; i++) {
            n->page.page.leaf.record[i].key = 0;
            memset(n->page.page.leaf.record[i].value, 0, 120);
        }
    }
    else {
        for (i = n->page.page.internal.numkeys; i < inorder; i++) {
            n->page.page.internal.precord[i].key = 0;
            n->page.page.internal.precord[i].pnum = 0;
        }
    }
    file_write_page(n->pnum, &(n->page));


    return n;
}


Node_t * adjust_root(Node_t * root) {

    Node_t * new_root = (Node_t*)malloc(sizeof(Node_t));

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

    if (root->page.page.internal.numkeys > 0){
 //       if(root->page.page.internal.precord[0].pnum != 0){
            file_write_page(root->pnum, &(root->page));
            return root;
 //       }
    }

    /* Case: empty root.
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!root->page.page.internal.isLeaf) {
        new_root->pnum = root->page.page.internal.more_pnum;
        file_read_page(new_root->pnum, &(new_root->page));
        new_root->page.page.leaf.parent_pnum = 0;
        file_write_page(new_root->pnum, &(new_root->page));
        file_free_page(root->pnum);
        
        //change header
        headerManager.header.root_pnum = new_root->pnum;
        headerManager.modified = true;
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else {
        headerManager.header.root_pnum = 0;
        headerManager.modified = true;
        file_free_page(root->pnum);
    }

    free(new_root);
    return 0;
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
Node_t * coalesce_nodes(Node_t * root, Node_t * n, Node_t * neighbor, int index, int k_prime) {

    printf("==Kprime: %d\n", k_prime);

    int k;
    if(k_prime == 5985){
        k = 1;
    }
    int i, j, neighbor_insertion_index;
    int parent_num;


    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    neighbor_insertion_index = neighbor->page.page.internal.numkeys;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!n->page.page.internal.isLeaf) {

        /* Append k_prime.
         */
        if (index != -1) {
            neighbor->page.page.internal.precord[neighbor_insertion_index].key = k_prime;
            neighbor->page.page.internal.precord[neighbor_insertion_index].pnum = n->page.page.internal.more_pnum;
            neighbor->page.page.internal.numkeys++;
            
            file_free_page(n->pnum);
            file_write_page(neighbor->pnum, &(neighbor->page));

            Node_t more_page;
            more_page.pnum = n->page.page.internal.more_pnum;
            file_read_page(more_page.pnum, &(more_page.page));
            more_page.page.page.internal.parent_pnum = neighbor->pnum;
            file_write_page(more_page.pnum, &(more_page.page));

            free(n);
            parent_num = neighbor->page.page.internal.parent_pnum;
        }
        else {
            n->page.page.internal.precord[0].key = k_prime;
            n->page.page.internal.precord[0].pnum = neighbor->page.page.internal.more_pnum;

            Node_t more_page;
            more_page.pnum = neighbor->page.page.internal.more_pnum;
            file_read_page(more_page.pnum, &(more_page.page));
            more_page.page.page.internal.parent_pnum = n->pnum;
            file_write_page(more_page.pnum, &(more_page.page));

            for (i = 0; i < neighbor->page.page.internal.numkeys; i++) {
                n->page.page.internal.precord[i + 1] = neighbor->page.page.internal.precord[i];

                more_page.pnum = neighbor->page.page.internal.precord[i].pnum;
                file_read_page(more_page.pnum, &(more_page.page));
                more_page.page.page.internal.parent_pnum = n->pnum;
                file_write_page(more_page.pnum, &(more_page.page));
            }
            n->page.page.internal.numkeys = neighbor->page.page.internal.numkeys + 1;

            file_free_page(neighbor->pnum);
            file_write_page(n->pnum, &(n->page));

            free(neighbor);
            parent_num = n->page.page.internal.parent_pnum;
        }

    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        if (index != -1) {
            neighbor->page.page.leaf.rsib_pnum = n->page.page.leaf.rsib_pnum;
            file_free_page(n->pnum);
            file_write_page(neighbor->pnum, &(neighbor->page));

            free(n);
            parent_num = neighbor->page.page.internal.parent_pnum;
        }
        else {
//            if(neighbor->pnum != 0){
                for (i = 0; i < neighbor->page.page.leaf.numkeys; i++) {
                    n->page.page.leaf.record[i] = neighbor->page.page.leaf.record[i];
                    ++n->page.page.leaf.numkeys;
                    --neighbor->page.page.leaf.numkeys;
                }
                // n->page.page.leaf.numkeys = neighbor->page.page.leaf.numkeys;
                // n->page.page.leaf.rsib_pnum = neighbor->page.page.leaf.rsib_pnum;
                n->page.page.leaf.rsib_pnum = neighbor->page.page.leaf.rsib_pnum;

                file_free_page(neighbor->pnum);
                file_write_page(n->pnum, &(n->page));

                free(neighbor);
                parent_num = n->page.page.internal.parent_pnum;
//            }
//            else{
//                parent_num = n->page.page.internal.parent_pnum;
//                file_free_page(n->pnum);
//            }
        }

    }

    Node_t* parent = (Node_t*)malloc(sizeof(Node_t));
    parent->pnum = parent_num;
    file_read_page(parent->pnum, &(parent->page));

    root = delete_entry(root, parent, k_prime);

//    free(parent);

    return root;
}


/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, 
        int k_prime_index, int k_prime) {  

    int i;
    node * tmp;

    /* Case: n has a neighbor to the left. 
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */

    if (neighbor_index != -1) {
        if (!n->is_leaf)
            n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
        for (i = n->num_keys; i > 0; i--) {
            n->keys[i] = n->keys[i - 1];
            n->pointers[i] = n->pointers[i - 1];
        }
        if (!n->is_leaf) {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys];
            tmp = (node *)n->pointers[0];
            tmp->parent = n;
            neighbor->pointers[neighbor->num_keys] = NULL;
            n->keys[0] = k_prime;
            n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
        }
        else {
            n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
            neighbor->pointers[neighbor->num_keys - 1] = NULL;
            n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
            n->parent->keys[k_prime_index] = n->keys[0];
        }
    }

    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {  
        if (n->is_leaf) {
            n->keys[n->num_keys] = neighbor->keys[0];
            n->pointers[n->num_keys] = neighbor->pointers[0];
            n->parent->keys[k_prime_index] = neighbor->keys[1];
        }
        else {
            n->keys[n->num_keys] = k_prime;
            n->pointers[n->num_keys + 1] = neighbor->pointers[0];
            tmp = (node *)n->pointers[n->num_keys + 1];
            tmp->parent = n;
            n->parent->keys[k_prime_index] = neighbor->keys[0];
        }
        for (i = 0; i < neighbor->num_keys - 1; i++) {
            neighbor->keys[i] = neighbor->keys[i + 1];
            neighbor->pointers[i] = neighbor->pointers[i + 1];
        }
        if (!n->is_leaf)
            neighbor->pointers[i] = neighbor->pointers[i + 1];
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */

    n->num_keys++;
    neighbor->num_keys--;

    return root;
}


/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
Node_t * delete_entry( Node_t * root, Node_t * n, int key ) {


    int min_keys;
    Node_t * neighbor;
    Node_t* parent;
    int index;
    int k_prime_index, k_prime;
    int capacity;

/*
    if(!n->page.page.internal.isLeaf && n->page.page.internal.numkeys == 1){
        if(n->page.page.internal.precord[0].pnum != 0){
            if(n->pnum != root->pnum){
               n->page.page.internal.precord[0].pnum = 0;
                file_write_page(n->pnum, &(n->page));
                file_free_page(n->page.page.internal.precord[0].pnum);
                return root;
            }
        }
    }
*/
    // Remove key and pointer from node.m

    n = remove_entry_from_node(n, key);

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

    if (n->page.page.internal.numkeys > min_keys)
        return root;

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

    parent = (Node_t*)malloc(sizeof(Node_t));
    neighbor = (Node_t*)malloc(sizeof(Node_t));
    file_read_page(n->page.page.internal.parent_pnum, &(parent->page));

    index = get_parent_index( n );
    k_prime_index = index == -1 ? 0 : index;
    k_prime = parent->page.page.internal.precord[k_prime_index].key;

    
    if (index == -1) {
        file_read_page(parent->page.page.internal.precord[0].pnum, &(neighbor->page));
        neighbor->pnum = parent->page.page.internal.precord[0].pnum;
    }
    else if(index == 0){
        file_read_page(parent->page.page.internal.more_pnum, &(neighbor->page));
        neighbor->pnum = parent->page.page.internal.more_pnum;
    }
    else {
        file_read_page(parent->page.page.internal.precord[index - 1].pnum, &(neighbor->page));
        neighbor->pnum = parent->page.page.internal.precord[index - 1].pnum;
    }


    /* Coalescence. */
    //Merge
    if (n->page.page.leaf.numkeys <= min_keys)
        return coalesce_nodes(root, n, neighbor, index, k_prime);

    free(parent);
    /* Redistribution. */

    //else
    //    return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}



/* Master deletion function.
 */
int delete(Node_t * root, int key) {

    Node_t * key_leaf;
    Record * key_record;

    key_record = find(root, key);
    key_leaf = find_leaf(root, key);
    if (key_record != NULL && key_leaf != NULL) {
        root = delete_entry(root, key_leaf, key);
        return 0;
    }
    else{
        return -1;
    }
}


void destroy_tree_nodes(node * root) {
    int i;
    if (root->is_leaf)
        for (i = 0; i < root->num_keys; i++)
            free(root->pointers[i]);
    else
        for (i = 0; i < root->num_keys + 1; i++)
            destroy_tree_nodes(root->pointers[i]);
    free(root->pointers);
    free(root->keys);
    free(root);
}


node * destroy_tree(node * root) {
    destroy_tree_nodes(root);
    return NULL;
}

