/*
 *  bpt.c  
 */
#define Version "1.14"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010-2016  Amittai Aviram  http://www.amittai.com
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, 
 *  this list of conditions and the following disclaimer in the documentation 
 *  and/or other materials provided with the distribution.
 
 *  3. Neither the name of the copyright holder nor the names of its 
 *  contributors may be used to endorse or promote products derived from this 
 *  software without specific prior written permission.
 
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE.
 
 *  Author:  Amittai Aviram 
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *  Original Date:  26 June 2010
 *  Last modified: 17 June 2016
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *  
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

#include "bpt.h"
#include "file.h"

// GLOBALS.

/* The order determines the maximum and minimum
 * number of entries (keys and pointers) in any
 * node.  Every node has at most order - 1 keys and
 * at least (roughly speaking) half that number.
 * Every leaf has as many pointers to data as keys,
 * and every internal node has one more pointer
 * to a subtree than the number of keys.
 * This global variable is initialized to the
 * default value.
 */
int order = DEFAULT_ORDER;

/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
node * queue = NULL;

/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
bool verbose_output = false;


// FUNCTION DEFINITIONS.

// OUTPUT AND UTILITIES

/* Copyright and license notice to user at startup. 
 */
void license_notice( void ) {
    printf("bpt version %s -- Copyright (C) 2010  Amittai Aviram "
            "http://www.amittai.com\n", Version);
    printf("This program comes with ABSOLUTELY NO WARRANTY; for details "
            "type `show w'.\n"
            "This is free software, and you are welcome to redistribute it\n"
            "under certain conditions; type `show c' for details.\n\n");
}


/* Routine to print portion of GPL license to stdout.
 */
void print_license( int license_part ) {
    int start, end, line;
    FILE * fp;
    char buffer[0x100];

    switch(license_part) {
    case LICENSE_WARRANTEE:
        start = LICENSE_WARRANTEE_START;
        end = LICENSE_WARRANTEE_END;
        break;
    case LICENSE_CONDITIONS:
        start = LICENSE_CONDITIONS_START;
        end = LICENSE_CONDITIONS_END;
        break;
    default:
        return;
    }

    fp = fopen(LICENSE_FILE, "r");
    if (fp == NULL) {
        perror("print_license: fopen");
        exit(EXIT_FAILURE);
    }
    for (line = 0; line < start; line++)
        fgets(buffer, sizeof(buffer), fp);
    for ( ; line < end; line++) {
        fgets(buffer, sizeof(buffer), fp);
        printf("%s", buffer);
    }
    fclose(fp);
}


/* First message to the user.
 */
void usage_1( void ) {
    printf("B+ Tree of Order %d.\n", order);
    printf("Following Silberschatz, Korth, Sidarshan, Database Concepts, "
           "5th ed.\n\n"
           "To build a B+ tree of a different order, start again and enter "
           "the order\n"
           "as an integer argument:  bpt <order>  ");
    printf("(%d <= order <= %d).\n", MIN_ORDER, MAX_ORDER);
    printf("To start with input from a file of newline-delimited integers, \n"
           "start again and enter the order followed by the filename:\n"
           "bpt <order> <inputfile> .\n");
}


/* Second message to the user.
 */
void usage_2( void ) {
    printf("Enter any of the following commands after the prompt > :\n"
    "\ti <k>  -- Insert <k> (an integer) as both key and value).\n"
    "\tf <k>  -- Find the value under key <k>.\n"
    "\tp <k> -- Print the path from the root to key k and its associated "
           "value.\n"
    "\tr <k1> <k2> -- Print the keys and values found in the range "
            "[<k1>, <k2>\n"
    "\td <k>  -- Delete key <k> and its associated value.\n"
    "\tx -- Destroy the whole tree.  Start again with an empty tree of the "
           "same order.\n"
    "\tt -- Print the B+ tree.\n"
    "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
    "\tv -- Toggle output of pointer addresses (\"verbose\") in tree and "
           "leaves.\n"
    "\tq -- Quit. (Or use Ctl-D.)\n"
    "\t? -- Print this help message.\n");
}


/* Brief usage note.
 */
void usage_3( void ) {
    printf("Usage: ./bpt [<order>]\n");
    printf("\twhere %d <= order <= %d .\n", MIN_ORDER, MAX_ORDER);
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
void enqueue( node * new_node ) {
    node * c;
    if (queue == NULL) {
        queue = new_node;
        queue->next = NULL;
    }
    else {
        c = queue;
        while(c->next != NULL) {
            c = c->next;
        }
        c->next = new_node;
        new_node->next = NULL;
    }
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
node * dequeue( void ) {
    node * n = queue;
    queue = queue->next;
    n->next = NULL;
    return n;
}


/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
void print_leaves( node * root ) {
    int i;
    node * c = root;
    if (root == NULL) {
        printf("Empty tree.\n");
        return;
    }
    while (!c->is_leaf)
        c = c->pointers[0];
    while (true) {
        for (i = 0; i < c->num_keys; i++) {
            if (verbose_output)
                printf("%lx ", (unsigned long)c->pointers[i]);
            printf("%d ", c->keys[i]);
        }
        if (verbose_output)
            printf("%lx ", (unsigned long)c->pointers[order - 1]);
        if (c->pointers[order - 1] != NULL) {
            printf(" | ");
            c = c->pointers[order - 1];
        }
        else
            break;
    }
    printf("\n");
}


/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
int height( node * root ) {
    int h = 0;
    node * c = root;
    while (!c->is_leaf) {
        c = c->pointers[0];
        h++;
    }
    return h;
}


/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
int path_to_root( node * root, node * child ) {
    int length = 0;
    node * c = child;
    while (c != root) {
        c = c->parent;
        length++;
    }
    return length;
}


/* Prints the B+ tree in the command
 * line in level (rank) order, with the 
 * keys in each node and the '|' symbol
 * to separate nodes.
 * With the verbose_output flag set.
 * the values of the pointers corresponding
 * to the keys also appear next to their respective
 * keys, in hexadecimal notation.
 */
void print_tree( node * root ) {

    node * n = NULL;
    int i = 0;
    int rank = 0;
    int new_rank = 0;

    if (root == NULL) {
        printf("Empty tree.\n");
        return;
    }
    queue = NULL;
    enqueue(root);
    while( queue != NULL ) {
        n = dequeue();
        if (n->parent != NULL && n == n->parent->pointers[0]) {
            new_rank = path_to_root( root, n );
            if (new_rank != rank) {
                rank = new_rank;
                printf("\n");
            }
        }
        if (verbose_output) 
            printf("(%lx)", (unsigned long)n);
        for (i = 0; i < n->num_keys; i++) {
            if (verbose_output)
                printf("%lx ", (unsigned long)n->pointers[i]);
            printf("%d ", n->keys[i]);
        }
        if (!n->is_leaf)
            for (i = 0; i <= n->num_keys; i++)
                enqueue(n->pointers[i]);
        if (verbose_output) {
            if (n->is_leaf) 
                printf("%lx ", (unsigned long)n->pointers[order - 1]);
            else
                printf("%lx ", (unsigned long)n->pointers[n->num_keys]);
        }
        printf("| ");
    }
    printf("\n");
}


/* Finds the record under a given key and prints an
 * appropriate message to stdout.
 */
void find_and_print(node * root, int key, bool verbose) {
    record * r = find(root, key);
    if (r == NULL)
        printf("Record not found under key %d.\n", key);
    else 
        printf("Record at %lx -- key %d, value %d.\n",
                (unsigned long)r, key, r->value);
}


/* Finds and prints the keys, pointers, and values within a range
 * of keys between key_start and key_end, including both bounds.
 */
void find_and_print_range( node * root, int key_start, int key_end,
        bool verbose ) {
    int i;
    int array_size = key_end - key_start + 1;
    int returned_keys[array_size];
    void * returned_pointers[array_size];
    int num_found = find_range( root, key_start, key_end, verbose,
            returned_keys, returned_pointers );
    if (!num_found)
        printf("None found.\n");
    else {
        for (i = 0; i < num_found; i++)
            printf("Key: %d   Location: %lx  Value: %d\n",
                    returned_keys[i],
                    (unsigned long)returned_pointers[i],
                    ((record *)
                     returned_pointers[i])->value);
    }
}


/* Finds keys and their pointers, if present, in the range specified
 * by key_start and key_end, inclusive.  Places these in the arrays
 * returned_keys and returned_pointers, and returns the number of
 * entries found.
 */
int find_range( node * root, int key_start, int key_end, bool verbose,
        int returned_keys[], void * returned_pointers[]) {
    int i, num_found;
    num_found = 0;
    node * n = find_leaf( root, key_start);
    if (n == NULL) return 0;
    for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++) ;
    if (i == n->num_keys) return 0;
    while (n != NULL) {
        for ( ; i < n->num_keys && n->keys[i] <= key_end; i++) {
            returned_keys[num_found] = n->keys[i];
            returned_pointers[num_found] = n->pointers[i];
            num_found++;
        }
        n = n->pointers[order - 1];
        i = 0;
    }
    return num_found;
}



/* Traces the path from the root to a leaf, searching
 * by key.  Displays information about the path
 * if the verbose flag is set.
 * Returns the leaf containing the given key.
 */
Node_t * find_leaf( Node_t * root, int key) {
    int i = 0;
    Node_t * c = root;
    if (c == NULL) {
        return c;
    }
    while (!c->page.page.internal.isLeaf) {
        i = 0;
        while (i < c->page.page.internal.numkeys) {
            if (key >= c->page.page.internal.precord[i].key) i++;
            else break;
        }
        c->pnum = c->page.page.internal.precord[i].pnum;
        file_read_page(c->page.page.internal.precord[i].pnum, c);
    }

    return c;
}


/* Finds and returns the record to which
 * a key refers.
 */
Record * find( Node_t * root, int key) {
    int i = 0;
    Node_t * c = find_leaf(root, key);
    if (c == NULL) return NULL;
    for (i = 0; i < c->page.page.leaf.numkeys; i++)
        if (c->page.page.leaf.record[i].key == key) break;
    if (i == c->page.page.leaf.numkeys) 
        return NULL;
    else
        return &(c->page.page.leaf.record[i]);
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
Record * make_record(int key, const char* value) {
    Record * new_record = (Record *)malloc(sizeof(Record));
    if (new_record == NULL) {
        perror("Record creation.");
        exit(EXIT_FAILURE);
    }
    else {
        new_record->key = key;
        strcpy(new_record->value, value);
    }
    return new_record;
}


/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
Node_t * make_node( void ) {
    Node_t * new_node;
    new_node = malloc(sizeof(Node_t));
    if (new_node == NULL) {
        perror("Node creation.");
        exit(EXIT_FAILURE);
    }
    new_node->page.page.internal.isLeaf = false;
    new_node->page.page.internal.parent_pnum = -1;
    new_node->page.page.internal.more_pnum = -1;
    new_node->pnum = file_alloc_page();

    /*
    new_node->keys = malloc( (order - 1) * sizeof(int) );
    if (new_node->keys == NULL) {
        perror("New node keys array.");
        exit(EXIT_FAILURE);
    }
    new_node->pointers = malloc( order * sizeof(void *) );
    if (new_node->pointers == NULL) {
        perror("New node pointers array.");
        exit(EXIT_FAILURE);
    }
    new_node->is_leaf = false;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    */
    return new_node;
}

/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
Node_t * make_leaf( void ) {
    Node_t * leaf = make_node();
    leaf->page.page.leaf.isLeaf = true;
    return leaf;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to 
 * the node to the left of the key to be inserted.
 */
int get_left_index(Node_t * parent, Node_t * left) {

    int left_index = 0;
    while (left_index <= parent->page.page.internal.numkeys && 
            parent->page.page.internal.precord[left_index].key != left->page.page.internal.precord[0].key)
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

    temp_record = malloc((order + 1) * sizeof(Record));
    if (temp_record == NULL) {
        perror("Temporary record array.");
        exit(EXIT_FAILURE);
    }

    new_precord = malloc(sizeof(Precord));
    if (new_precord = NULL) {
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

    for (i = split, j = 0; i < order; i++, j++) {
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

    //file_write

    return insert_into_parent(root, leaf, new_precord, new_leaf);
}


/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
Node_t * insert_into_node(Node_t * root, Node_t * n, 
        int left_index, Precord* precord) {
    int i;

    for (i = n->page.page.internal.numkeys; i > left_index; i--) {
        n->page.page.internal.precord[i] = n->page.page.internal.precord[i - 1];
    }
    n->page.page.internal.precord[left_index] = *precord;
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

    prime_precord = &temp_precord[split];
    new_node->page.page.internal.more_pnum = temp_precord[split].pnum;
    prime_precord->pnum = new_node->pnum;

    for (++i, j = 0; i < inorder; i++, j++) {
        new_node->page.page.internal.precord[j] = temp_precord[i];
        new_node->page.page.internal.numkeys++;
    }

    free(temp_precord);

    new_node->page.page.internal.parent_pnum = old_node->page.page.internal.parent_pnum;

    //TODO : optimization
    for (i = 0; i <= new_node->page.page.internal.numkeys; i++) {
        file_read_page(new_node->page.page.internal.precord[i].pnum, tmp);
        tmp->page.internal.parent_pnum = new_node->pnum;
        file_write_page(new_node->page.page.internal.numkeys, tmp);
    }

    free(tmp);

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
    Node_t * parent;

    /* Case: new root. */

    if (right->page.page.internal.parent_pnum == 0)
        return insert_into_new_root(left, new_precord);

    file_read_page(left->pnum, &(parent->page));

    /* Case: leaf or node. (Remainder of
     * function body.)  
     */

    /* Find the parent's pointer to the left 
     * node.
     */

    left_index = get_left_index(parent, left);


    /* Simple case: the new key fits into the node. 
     */

    if (parent->page.page.internal.numkeys < inorder)
        return insert_into_node(root, parent, left_index, new_precord);

    /* Harder case:  split a node in order 
     * to preserve the B+ tree properties.
     */

    return insert_into_node_after_splitting(root, parent, left_index, new_precord, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
Node_t * insert_into_new_root(Node_t * left, Precord* key_record) {

    Node_t * root = make_node();

    headerManager.header.root_pnum = root->pnum;
    headerManager.modified = true;

    root->page.page.internal.parent_pnum = 0;
    root->page.page.internal.more_pnum = left->pnum;
    root->page.page.internal.precord[0] = *key_record;
    root->page.page.internal.numkeys++;

    file_page_write(root->pnum, root);

    return root;
}



/* First insertion:
 * start a new tree.
 */
Node_t * start_new_tree( Record * pointer) {

    Node_t * root = make_leaf();
    root->page.page.leaf.parent_pnum = 0;
    root->page.page.leaf.rsib_pnum = 0;
    root->page.page.leaf.record[0] = *pointer;
    root->page.page.leaf.numkeys++;

    headerManager.header.root_pnum = root->pnum;
    headerManager.modified = true;
    file_page_write(root->pnum, root);

    return root;
}



/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
int insert( Node_t * root, int key, const char* value ) {

    Record * pointer;
    Node_t * leaf;

    /* The current implementation ignores
     * duplicates.
     */

    if (find(root, key) != NULL)
        return 0;

    /* Create a new record for the
     * value.
     */
    pointer = make_record(key, value);


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (root == NULL) {
        start_new_tree(pointer);
        return 0;
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
int get_neighbor_index( Node_t * n ) {

    int i;
    Node_t* parent = (Node_t*)malloc(sizeof(Node_t));

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.  
     * If n is the leftmost child, this means
     * return -1.
     */
    
    file_read_page(n->pnum, &(parent->page));
    for (i = 0; i <= parent->page.page.internal.numkeys; i++)
        if (parent->page.page.internal.more_pnum == n->pnum)
            return -1;
        if (parent->page.page.internal.precord[i].pnum == n)
            return i - 1;

    free(parent);

    // Error state.
    printf("Search for nonexistent pointer to node in parent.\n");
    printf("Node:  %#lx\n", (unsigned long)n);
    exit(EXIT_FAILURE);
}


Node_t * remove_entry_from_node(Node_t * n, int key, Node_t * pointer) {

    int i, num_pointers;

    // Remove the key and shift other keys accordingly.
    i = 0;
    while (n->page.page.leaf.record[i].key != key)
        i++;
    for (++i; i < n->page.page.leaf.numkeys; i++)
        n->page.page.leaf.record[i - 1] = n->page.page.leaf.record[i];

    // One key fewer.
    n->page.page.internal.numkeys--;

    // Set the other pointers to NULL for tidiness.
    // A leaf uses the last pointer to point to the next leaf.
    if (n->page.page.internal.isLeaf) {
        for (i = n->page.page.leaf.numkeys; i < order; i++) {
            n->page.page.leaf.record[i].key = 0;
            memset(n->page.page.leaf.record[i].value, 0, sizeof(120));
        }
    }
    else {
        for (i = n->page.page.internal.numkeys; i < inorder; i++) {
            n->page.page.internal.precord[i].key = 0;
            n->page.page.internal.precord[i].pnum = 0;
        }
    }

    return n;
}


Node_t * adjust_root(Node_t * root) {

    Node_t * new_root;

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

    if (root->page.page.internal.numkeys > 0)
        return root;

    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!root->page.page.internal.isLeaf) {
        file_read_page(root->page.page.internal.more_pnum, &(new_root->page));
        new_root->page.page.leaf.parent_pnum = 0;
        file_write_page(new_root->pnum, &(new_root->page));
        
        //change header
        headerManager.header.root_pnum = new_root->pnum;
        headerManager.modified = true;

        file_free_page(root->pnum);
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else {
        headerManager.header.root_pnum = 0;
        headerManager.modified = true;
    }

    free(new_root);

    return new_root;
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
Node_t * coalesce_nodes(Node_t * root, Node_t * n, Node_t * neighbor, int neighbor_index, int k_prime) {

    int i, neighbor_insertion_index;


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
        if (neighbor_index != -1) {
            neighbor->page.page.internal.precord[neighbor_insertion_index].key = k_prime;
            neighbor->page.page.internal.precord[neighbor_insertion_index].pnum = n->page.page.internal.more_pnum;
            neighbor->page.page.internal.numkeys++;
            
            file_free_page(n->pnum);
            file_write_page(neighbor->pnum, &(neighbor->page));

            free(n);
        }
        else {
            n->page.page.internal.precord[0].key = k_prime;
            n->page.page.internal.precord[0].pnum = neighbor->page.page.internal.more_pnum;
            for (i = 0; i < neighbor->page.page.internal.numkeys; i++) {
                n->page.page.internal.precord[i + 1] = neighbor->page.page.internal.precord[i];
            }
            n->page.page.internal.numkeys = neighbor->page.page.internal.numkeys + 1;

            file_free_page(neighbor->pnum);
            file_write_page(n->pnum, &(n->page));

            free(neighbor);
        }

    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        if (neighbor_index != -1) {
            neighbor->page.page.leaf.rsib_pnum = n->page.page.leaf.rsib_pnum;
            file_free_page(n->pnum);

            free(n);
        }
        else {
            for (i = 0; i < neighbor->page.page.leaf.numkeys; i++) {
                n->page.page.leaf.record[i] = neighbor->page.page.leaf.record[i];
            }
            n->page.page.leaf.numkeys = neighbor->page.page.leaf.numkeys;
            n->page.page.leaf.rsib_pnum = neighbor->page.page.leaf.rsib_pnum;

            file_free_page(neighbor->pnum);
            file_write_page(n->pnum, &(n->page));

            free(neighbor);
        }

    }

    root = delete_entry(root, n->page.page.internal.parent_pnum, k_prime, n);

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
Node_t * delete_entry( Node_t * root, Node_t * n, int key, Node_t * pointer ) {


    int min_keys;
    Node_t * neighbor;
    Node_t* parent;
    int neighbor_index;
    int k_prime_index, k_prime;
    int capacity;

    // Remove key and pointer from node.

    n = remove_entry_from_node(n, key, pointer);

    /* Case:  deletion from the root. 
     */

    if (n == root) 
        return adjust_root(root);


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
    file_read_page(n->page.page.internal.parent_pnum, &(parent->page));

    neighbor_index = get_neighbor_index( n );
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = parent->page.page.internal.precord[k_prime_index].key;
    
    if (neighbor_index == -1) {
        file_read_page(parent->page.page.internal.precord[0].pnum, &(neighbor->page));
        neighbor->pnum = parent->page.page.internal.precord[0].pnum;
    }
    else {
        file_read_page(parent->page.page.internal.precord[neighbor_index].pnum, &(neighbor->page));
        neighbor->pnum = parent->page.page.internal.precord[neighbor_index].pnum;
    }


    /* Coalescence. */
    //Merge
    if (n->page.page.leaf.numkeys <= min_keys)
        return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);

    /* Redistribution. */

    //else
    //    return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}



/* Master deletion function.
 */
Node_t * delete(Node_t * root, int key) {

    Node_t * key_leaf;
    Record * key_record;

    key_record = find(root, key);
    key_leaf = find_leaf(root, key);
    if (key_record != NULL && key_leaf != NULL) {
        root = delete_entry(root, key_leaf, key, key_record);
        free(key_record);
    }
    return root;
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

