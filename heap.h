#include <stdbool.h>
#pragma once

typedef struct Heap heap;

struct Heap{
	int capacity; //capacity of nodes, not include nodes[0]
	int heapsize; // not include nodes[0]
	struct Node *(nodes[]); //nodes[0] would not be used, nodes start from 1
};

typedef struct Node node;

struct Node{
    int id;
    unsigned char unch;
	int freq;
    bool is_leaf;
    struct Node *left, *right;
};

heap *heap_create();

void heap_destroy(heap* h);

void heap_insert(heap **h, struct Node* node);

void max_heapify(heap* h, int selfidx);

void min_heapify(heap* h, int selfidx);

struct Node *heap_pull(heap *h);

void print_heap(heap* h);

struct Node *create_node(unsigned char unch, int freq, bool is_leaf);
void trace_node_tree_gen_codes(node* n, char *code, char **codes_list);
node* create_huffman_tree(heap* h);
