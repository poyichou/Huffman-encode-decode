# Huffman coding
## To run
```
make
[./huffman_encode|./huffman_decode] <target file>
```
## Description
### Similar parts
Some parts of `huffman_encode` and `huffman_decode` are conceptually similar  
- Trying to get `freq`, which stores occurrences of byte 0x00-0xff.  
- Convert `freq` as `node`s and insert them into a min heap.  
- Pull `node`s pair by pair from the min heap, create a new parent `node`, assign the pair as the parent's left/right child, then insert the parent back into the min heap untill there's only one `node` inside the heap, which is `root`.  
- The procedure above eventually generate a huffman tree based on the occurrences of bytes rooted as `root`.  
- When tracing the huffman tree, append `1` for left child, `0` for right child until leaf is reached.  
### Running result
`huffman_encode` encode the content of `<target file>` then store table and encoded content in `<target file>.huffman`  
`huffman_encode` parse table and decode the following content in `<target file>` then store decoded content in `<target file>.dec`  
