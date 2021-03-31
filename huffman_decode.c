#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
/*
 * Huffman encoded file format:
 *   | freq (4 bytes * 256) | huff_size_bits (4 bytes) | huffman codes (variable) |
 * */

static int freq[256] = {0};
static char *codes[256] = {0};
static node* root = NULL;

heap* file_to_heap(FILE* fp) {
    heap* h = heap_create();
    for (int i = 0; i < 256; i++) {
        char temp[sizeof(int)];
        for (int j = 0; j < sizeof(int); j++) {
            temp[j] = fgetc(fp);
        }
        freq[i] = *((int*)(&temp[0]));
        if(freq[i]) {
            node *n = create_node((unsigned char)i, freq[i]);
            heap_insert(&h, n);
        }
    }
    return h;
}

void dump_from_huffman_code(FILE* in_fp, char* out_filename, int huff_size_bits) {
    printf("Dump to %s\n", out_filename);
    int huff_pad_bits = huff_size_bits % 8;
    if (huff_pad_bits) {
        huff_pad_bits = 8 - huff_pad_bits;
    }
    FILE* dec_fp = fopen(out_filename, "wb");
    char rb;
    node *trace_p = root;
    char bit;
    for (int i = 0; i < (huff_size_bits + huff_pad_bits) / 8; i++) {
        rb = fgetc(in_fp);
        for (int j = 0; j < 8 && i * 8 + j < huff_size_bits ; j++){
            bit = rb & (1 << (7 - j));
            if (bit) {
                trace_p = trace_p->left;
            } else {
                trace_p = trace_p->right;
            }
            if (trace_p->left == NULL || trace_p->right == NULL) { // leaf
                fputc(trace_p->unch, dec_fp);
                trace_p = root;
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <target_file>\n", __FILE__);
        exit(1);
    }
    char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    char ch;
    unsigned char unch;
    // read freq table
    heap* h = file_to_heap(fp);

    int huff_size_bits = 0;
    // read size indicator
    for (int i = 0; i < sizeof(int); i++) {
        ((char*)&huff_size_bits)[i] = fgetc(fp);
    }

    root = create_huffman_tree(h);
    trace_node_tree_gen_codes(root, "", codes);

    char dec_filename[strlen(".dec") + strlen(filename) + 1];
    snprintf(dec_filename, sizeof(dec_filename), "%s.dec", filename);
    dump_from_huffman_code(fp, dec_filename, huff_size_bits);
    return 0;
}
