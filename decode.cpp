#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <sstream>
#include "huffman.hpp"

using namespace std;

void decode_full(vector<int16_t>& decodedData, const vector<uint8_t>& encoded_data_in_bytes, const vector<uint8_t>& huffmancode_str)
{
    unordered_map<int16_t, string> huffmanCode_recovered = deserializeHuffmanTable(huffmancode_str);
    Node* root_recovered = buildTreeFromHuffmanCode(huffmanCode_recovered);
    decodedData = decode(root_recovered, unpackBytesToBits(encoded_data_in_bytes));
}

int main(int argc, char* argv[]) {

    if (argc != 3)
    {
        cerr << "Wrong argument count" << endl;
        return 1;
    }

    ifstream inFile(argv[1], ios::binary);
    if (!inFile) {
        cerr << "Error opening input file" << endl;
        return 1;
    }

    // Read the WAV header
    WAVHeader h_ff;
    inFile.read(reinterpret_cast<char*>(&h_ff), sizeof(WAVHeader));

    // Read the table offset
    huffman_offsets table_offset_ff;
    inFile.read(reinterpret_cast<char*>(&table_offset_ff), sizeof(table_offset_ff));

    // Read Huffman codes
    vector<uint8_t> ht1(table_offset_ff.ht1_size);
    inFile.read(reinterpret_cast<char*>(ht1.data()), table_offset_ff.ht1_size);

    // Read the first encoded data
    vector<uint8_t> encoded_data_in_bytes_ff_1(table_offset_ff.h1_size);
    inFile.read(reinterpret_cast<char*>(encoded_data_in_bytes_ff_1.data()), table_offset_ff.h1_size);

    // Read Huffman codes
    vector<uint8_t> ht2(table_offset_ff.ht2_size);
    inFile.read(reinterpret_cast<char*>(ht2.data()), table_offset_ff.ht2_size);

    // Read the second encoded data
    vector<uint8_t> encoded_data_in_bytes_ff_2(table_offset_ff.h2_size);
    inFile.read(reinterpret_cast<char*>(encoded_data_in_bytes_ff_2.data()), table_offset_ff.h2_size);

    inFile.close();

    vector<int16_t> first_element_of_group_decoded;
    vector<int16_t> other_elements_of_group_decoded;
    decode_full(first_element_of_group_decoded, encoded_data_in_bytes_ff_1, ht1);
    decode_full(other_elements_of_group_decoded, encoded_data_in_bytes_ff_2, ht2);

    ofstream out_wav(argv[2], ios::binary);
    out_wav.write(reinterpret_cast<const char*>(&h_ff), sizeof(WAVHeader));
    vector<int16_t> decoded_data;
    
    size_t j = 0;
    for (size_t i = 0; i < (h_ff.dataSize >> 1); ++i) {
        int16_t a = first_element_of_group_decoded[i / GROUP];

        if (i % GROUP == 0)
        {
            decoded_data.push_back(a);
        }
        else
        {
            decoded_data.push_back(a + other_elements_of_group_decoded[j++]);
        }
    }

    out_wav.write(reinterpret_cast<const char*>(decoded_data.data()), decoded_data.size() * sizeof(int16_t));
    out_wav.close();

    return 0;
}