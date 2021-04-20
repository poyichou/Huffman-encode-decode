#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "heap.h"
#include "huffman_coding.h"
/*
 * Huffman encoded file format:
 *   | freq (4 bytes * 256) | huffman codes (variable) |
 * */

static char *codes[256] = {0};
static node* root = NULL;

heap* file_to_heap(FILE* fp, int freq[]) {
    heap* h = heap_create();
    for (int i = 0; i < 256; i++) {
        fread(&freq[i], sizeof(int), 1, fp);
        if(freq[i]) {
            node *n = create_node((unsigned char)i, freq[i]);
            heap_insert(&h, n);
        }
    }
    return h;
}

void dump_from_huffman_code(FILE* in_fp, char* out_filename, int freq[]) {
    FILE* dec_fp = fopen(out_filename, "wb");
    char rb;
    node *trace_p = root;
    char bit;
    while(1) {
        rb = fgetc(in_fp);
        for (int j = 0; j < 8; j++){
            bit = rb & (1 << (7 - j));
            if (bit) {
                trace_p = trace_p->left;
            } else {
                trace_p = trace_p->right;
            }
            if (trace_p->left == NULL || trace_p->right == NULL) { // leaf
                if (freq[trace_p->unch] <= 0) {
                    fclose(dec_fp);
                    return;
                }
                freq[trace_p->unch]--;
                fputc(trace_p->unch, dec_fp);
                trace_p = root;
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <target_file>\n", argv[0]);
        exit(1);
    }
    char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    char ch;
    unsigned char unch;
    int freq[256] = {0};
    // read freq table
    heap* h = file_to_heap(fp, freq);

    root = create_huffman_tree(h);
    trace_node_tree_gen_codes(root, "", codes);

    char dec_filename[strlen(".dec") + strlen(filename) + 1];
    snprintf(dec_filename, sizeof(dec_filename), "%s.dec", filename);
    dump_from_huffman_code(fp, dec_filename, freq);
    fclose(fp);

    struct stat st;
    stat(filename, &st);
    chmod(dec_filename, st.st_mode);
    rename(dec_filename, filename);
    return 0;
}
