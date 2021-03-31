#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

heap *heap_create() {
	heap *h = malloc(sizeof(int) * 2 + sizeof(struct Node*));
	h->capacity = 0;
	h->heapsize = 0;
	return h;
}

void heap_destroy(heap* h) {
	free(h);
}

void heap_insert(heap **h, struct Node* node) {
	if ((*h)->heapsize >= (*h)->capacity) {
		if ((*h)->capacity == 0) {
			*h = realloc(*h, sizeof(int) * 2 + sizeof(struct Node*) * (10 + 1));
			(*h)->capacity = 10;
		} else {
			*h = realloc(*h, sizeof(int) * 2 + sizeof(struct Node*) * ((*h)->capacity * 2 + 1));
			(*h)->capacity *= 2;
		}
	}
	(*h)->heapsize++;
	(*h)->nodes[(*h)->heapsize] = node;
	return;
}

void max_sink_down(heap* h, int selfidx) {
	int childidx;
	if (selfidx * 2 + 1 <= h->heapsize) {
		// right child exists
		if (h->nodes[selfidx * 2]->freq >= h->nodes[selfidx * 2 + 1]->freq && h->nodes[selfidx]->freq < h->nodes[selfidx * 2]->freq) {
			// left child is greater than right child and parent
			// swap with left child
			childidx = selfidx * 2;
		} else if (h->nodes[selfidx * 2]->freq < h->nodes[selfidx * 2 + 1]->freq && h->nodes[selfidx]->freq < h->nodes[selfidx * 2 + 1]->freq) {
			// right child is greater than left child and parent
			// swap with right child
			childidx = selfidx * 2 + 1;
		} else
			return;
	} else if (selfidx * 2 <= h->heapsize && h->nodes[selfidx]->freq < h->nodes[selfidx * 2]->freq) {
		// right child does not exist and left child is greater than parent
		childidx = selfidx * 2;
	} else
		return;

	struct Node *node;
	// swap with child
	node = h->nodes[selfidx];
	h->nodes[selfidx] = h->nodes[childidx];
	h->nodes[childidx] = node;

	max_sink_down(h, childidx);
	return;
}
void min_sink_down(heap* h, int selfidx) {
	int childidx;
	if (selfidx * 2 + 1 <= h->heapsize) {
		// right child exists
		if (h->nodes[selfidx * 2]->freq <= h->nodes[selfidx * 2 + 1]->freq && h->nodes[selfidx * 2]->freq < h->nodes[selfidx]->freq) {
			// left child is less than right child and parent
			// swap with left child
			childidx = selfidx * 2;
		} else if (h->nodes[selfidx * 2 + 1]->freq < h->nodes[selfidx * 2]->freq && h->nodes[selfidx * 2 + 1]->freq < h->nodes[selfidx]->freq) {
			// right child is less than left child and parent
			// swap with right child
			childidx = selfidx * 2 + 1;
		} else
			return;
	} else if (selfidx * 2 <= h->heapsize && h->nodes[selfidx * 2]->freq < h->nodes[selfidx]->freq) {
		// onlt left child exists and left child is less than parent
		childidx = selfidx * 2;
	} else
		return;

	struct Node *node;
	// swap with child
	node = h->nodes[selfidx];
	h->nodes[selfidx] = h->nodes[childidx];
	h->nodes[childidx] = node;

	min_sink_down(h, childidx);
}

// bottom-up
// selfidx of root == 1
// according to node->d
void max_heapify(heap* h, int selfidx) {
	// heapify it's left child and right child
	if (selfidx * 2 <= h->heapsize) {
		max_heapify(h, selfidx * 2);
	}
	if (selfidx * 2 + 1 <= h->heapsize) {
		max_heapify(h, selfidx * 2 + 1);
	}
	//sink down itself
	max_sink_down(h, selfidx);
}
//NOTE
void min_heapify(heap* h, int selfidx) {
	// heapify it's left child and right child
	if (selfidx * 2 <= h->heapsize) {
		min_heapify(h, selfidx * 2);
	}
	if (selfidx * 2 + 1 <= h->heapsize) {
		min_heapify(h, selfidx * 2 + 1);
	}
	//sink down itself
	min_sink_down(h, selfidx);
}

/**
  pull root
  it's user's res[onsibility to heapify
*/
struct Node *heap_pull(heap *h) {
	if (h->heapsize <= 0) {
		return NULL;
	}
	struct Node *node = h->nodes[1];
	h->nodes[1] = h->nodes[h->heapsize];
	h->nodes[h->heapsize] = NULL;
	h->heapsize--;
	return node;
}

void print_heap(heap* h) {
	for (int i = 1; i <= h->heapsize; i++) {
		printf("%u %d\n", h->nodes[i]->unch, h->nodes[i]->freq);
	}
}

struct Node *create_node(unsigned char unch, int freq) {
	static int idx = 0;
	struct Node* node = malloc(sizeof(struct Node));
    node->id = idx;
	node->unch = unch;
	node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    idx++;
	return node;
}
void trace_node_tree_gen_codes(node* n, char *code, char **codes_list) {
    if (n->left == NULL || n->right == NULL) { // leaf
        codes_list[n->unch] = strdup(code);
        return;
    }
    char left_code[strlen(code) + 2], right_code[strlen(code) + 2];
    snprintf(left_code, sizeof(left_code), "%s1", code);
    snprintf(right_code, sizeof(right_code), "%s0", code);
    trace_node_tree_gen_codes(n->left, left_code, codes_list);
    trace_node_tree_gen_codes(n->right, right_code, codes_list);
}
node* create_huffman_tree(heap* h) {
    node* left = NULL, *right = NULL;
    while(1) {
        min_heapify(h, 1);
        node *n = heap_pull(h);
        if(!n) {
            break;
        }
        if(!left) {
            left = n;
        } else if (!right) {
            right = n;

            node *parent = create_node(0, left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            heap_insert(&h, parent);
            left = NULL;
            right = NULL;
        }
    }
    return left;
}
