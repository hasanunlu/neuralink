Execute ```make``` to compile encode.cpp and decode.cpp.\
To test encode and decode, call ```./eval.sh```.\
Submitted this code to neuralink already(GROUP=2). It is a good start people who need basic starter code with some heuristic.\
The algorithm uses huffman encoding for two sets of data.\
The first set is every first element of the group. The second set has rest of the group elements.\
Simple 8 element data with group of 4 settings. [**100**, 120, 110, 111, **102**, 150, 200, 80]\
After manupulation the data. Subtrack the first element in every group from the rest.\
First set: huffman_encoding([**100**, **102**])\
Second set: huffman_encoding([20, 10, 11, 48, 98, -22])
```
All recordings successfully compressed.
Original size (bytes): 146800526
Compressed size (bytes): 56494444
Compression ratio: 2.59
```
