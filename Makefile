all: huffman_decode huffman_encode
%: %.c heap.c
	gcc $^ -o $@
clean:
	rm -f huffman_decode huffman_encode
