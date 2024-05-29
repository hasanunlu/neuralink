#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <sstream>
#include "huffman.hpp"
#include "header.hpp"

using namespace std;

void decode_full(vector<int16_t>& decodedData, const vector<uint8_t>& encoded_data_in_bytes, const string& huffmancode_str)
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

    std::ifstream inFile(argv[1], std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    // Read the WAV header
    WAVHeader h_ff;
    inFile.read(reinterpret_cast<char*>(&h_ff), sizeof(WAVHeader));

    // Read the table offset
    huffman_offsets table_offset_ff;
    inFile.read(reinterpret_cast<char*>(&table_offset_ff), sizeof(table_offset_ff));

    // Read Huffman codes
    std::vector<char> ht1(table_offset_ff.ht1_size);
    inFile.read(ht1.data(), table_offset_ff.ht1_size);
    std::string huffmancode_str_ff_1(ht1.begin(), ht1.end());

    // Read the first encoded data
    std::vector<uint8_t> encoded_data_in_bytes_ff_1(table_offset_ff.h1_size);
    inFile.read(reinterpret_cast<char*>(encoded_data_in_bytes_ff_1.data()), table_offset_ff.h1_size);

    // Read Huffman codes
    std::vector<char> ht2(table_offset_ff.ht2_size);
    inFile.read(ht2.data(), table_offset_ff.ht2_size);
    std::string huffmancode_str_ff_2(ht2.begin(), ht2.end());

    // Read the second encoded data
    std::vector<uint8_t> encoded_data_in_bytes_ff_2(table_offset_ff.h2_size);
    inFile.read(reinterpret_cast<char*>(encoded_data_in_bytes_ff_2.data()), table_offset_ff.h2_size);

    inFile.close();

    vector<int16_t> first_element_of_group_decoded;
    vector<int16_t> other_elements_of_group_decoded;
    decode_full(first_element_of_group_decoded, encoded_data_in_bytes_ff_1, huffmancode_str_ff_1);
    decode_full(other_elements_of_group_decoded, encoded_data_in_bytes_ff_2, huffmancode_str_ff_2);

    ofstream out_wav(argv[2], std::ios::binary);
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