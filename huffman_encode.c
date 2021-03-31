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

void dump_huffman_code(FILE* in_fp, char* out_filename) {
    printf("Dump to %s\n", out_filename);
    FILE* huff_fp = fopen(out_filename, "wb");
    // write table
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < sizeof(int); j++) {
            fputc(((char*)&freq[i])[j], huff_fp);
        }
    }

    rewind(in_fp);
    int huff_size_bits = 0;
    for (int i = 0; i < 256; i++) {
        if (codes[i]) {
            huff_size_bits += strlen(codes[i]) * freq[i];
        }
    }
    char inc, wb = 0;
    char *temp_code = malloc(256);
    size_t temp_code_size = 256;

    int huff_pad_bits = huff_size_bits % 8;
    if (huff_pad_bits) {
        huff_pad_bits = 8 - huff_pad_bits;
    }
    // write size indicator
    for (int i = 0; i < sizeof(int); i++) {
        fputc(*(((char*)(&huff_size_bits)) + i), huff_fp);
    }

    // write codes
    int j = 0;
    for (int bit_count = 0; bit_count < huff_size_bits + huff_pad_bits; bit_count++) {
        if (bit_count >= huff_size_bits) {
            temp_code[0] = '0'; // padding
        } else if (temp_code[j] == 0) {
            inc = fgetc(in_fp);
            if (strlen(codes[(unsigned char)inc]) > temp_code_size - 1) {
                temp_code = realloc(temp_code, strlen(codes[(unsigned char)inc]) + 1);
                temp_code_size = strlen(codes[(unsigned char)inc]) + 1;
            }
            strcpy(temp_code, codes[(unsigned char)inc]);
            j = 0;
        }
        int in_bit = temp_code[j] - '0';
        int shift = (7 - (bit_count % 8));
        if (in_bit != 0 && in_bit != 1) {
            printf("unknown in_bit %d\n", in_bit);
            exit(1);
        }
        wb |= in_bit << shift;
        if (shift == 0) {
            fputc(wb, huff_fp);
            wb = 0;
        }
        j = (bit_count + 1) < huff_size_bits ? j + 1 : 0;
    }
    free(temp_code);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <target_file>\n", __FILE__);
        exit(1);
    }
    char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    char ch;
    unsigned char unch;
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    //create freq
    for (size_t i = 0; i < file_size; i++) {
        ch = fgetc(fp);
        unch = ch;
        freq[unch]++;
    }
    heap* h = heap_create();
    for (int i = 0; i < 256; i++) {
        if(freq[i]) {
            node *n = create_node((unsigned char)i, freq[i], true);
            heap_insert(&h, n);
        }
    }

    root = create_huffman_tree(h);
    trace_node_tree_gen_codes(root, "", codes);

    char huff_filename[strlen(".huffman") + strlen(filename) + 1];
    snprintf(huff_filename, sizeof(huff_filename), "%s.huffman", filename);
    dump_huffman_code(fp, huff_filename);
    return 0;
}
