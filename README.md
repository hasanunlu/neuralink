To compile encode.cpp and decode.cpp, run ```make```.\
After compiling, you can test the encoding and decoding functionalities by executing ```./eval.sh```.\
This code has already been submitted to Neuralink with **GROUP=2**.\
It serves as a useful starting point for those who need basic starter code with some heuristics.\
The algorithm implemented here employs Huffman encoding for two distinct sets of data:\
The first set comprises the first element of each group, while the second set contains the remaining elements in the groups.\
Consider a simple dataset of 8 elements divided into groups of 4 settings: [**100**, 120, 110, 111, **102**, 150, 200, 80].\
After manipulating the data, subtract the first element in each group from the rest.\
This results in the following sets for Huffman encoding:\
First set: huffman_encoding([**100**, **102**])\
Second set: huffman_encoding([20, 10, 11, 48, 98, -22]).
```
All recordings successfully compressed.
Original size (bytes): 146800526
Compressed size (bytes): 56494444
Compression ratio: 2.59
```
