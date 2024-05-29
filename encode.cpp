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

class WAVFile {
public:
    WAVFile(const string& filename) : filename(filename) {}
    
    bool read() {
        ifstream file(filename, ios::binary);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return false;
        }

        // Read header
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
        if (!file) {
            cerr << "Error reading header from file: " << filename << endl;
            return false;
        }

        // Check if it is a valid WAV file
        if (string(header.riffHeader, 4) != "RIFF" || string(header.waveHeader, 4) != "WAVE") {
            cerr << "Invalid WAV file format: " << filename << endl;
            return false;
        }

        // Read data
        data.resize(header.dataSize);
        file.read(reinterpret_cast<char*>(data.data()), header.dataSize);
        if (!file) {
            cerr << "Error reading data from file: " << filename << endl;
            return false;
        }

        return true;
    }

    void printInfo() const {
        cout << "Filename: " << filename << endl;
        cout << "File Size: " << header.fileSize << " bytes" << endl;
        cout << "Audio Format: " << header.audioFormat << endl;
        cout << "Number of Channels: " << header.numChannels << endl;
        cout << "Sample Rate: " << header.sampleRate << " Hz" << endl;
        cout << "Byte Rate: " << header.byteRate << endl;
        cout << "Block Align: " << header.blockAlign << endl;
        cout << "Bits Per Sample: " << header.bitsPerSample << endl;
        cout << "Data Size: " << header.dataSize << " bytes" << endl;
    }

    void printAudioSamples(size_t numSamples) const {
        cout << "First " << numSamples << " audio samples:" << endl;
        for (size_t i = 0; i < numSamples; ++i) {
            if (header.bitsPerSample == 8) {
                cout << static_cast<int>(reinterpret_cast<const uint8_t*>(data.data())[i]) << " ";
            } else if (header.bitsPerSample == 16) {
                cout << reinterpret_cast<const int16_t*>(data.data())[i] << " ";
            } else if (header.bitsPerSample == 32) {
                cout << reinterpret_cast<const int32_t*>(data.data())[i] << " ";
            }
        }
        cout << endl;
    }

    vector<int16_t> get_int16_vector() const {
        vector<int16_t> output;
        for (size_t i = 0; i < (header.dataSize >> 1); ++i) {
            if (header.bitsPerSample == 16) {
                output.push_back(reinterpret_cast<const int16_t*>(data.data())[i]);
            } else {
                cerr << "Unsupported bitdepth " << endl;
            }
        }
        return output;
    }

    WAVHeader get_header() const {
        return header;
    }

private:
    string filename;
    WAVHeader header;
    vector<uint8_t> data;
};

void encode_full(const vector<int16_t>& data, vector<uint8_t>& encoded_data_in_bytes, vector<uint8_t>& huffmancode_str)
{
    Node* root = buildHuffmanTree(data);
    unordered_map<int16_t, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // cout << "Huffman Codes are:\n";
    // for (auto pair : huffmanCode) {
    //     cout << pair.first << " " << pair.second << "\n";
    // }

    huffmancode_str = serializeHuffmanTable(huffmanCode);
    string encodedString = encode(data, huffmanCode);
    encoded_data_in_bytes = packBitsToBytes(encodedString);
}

int main(int argc, char* argv[]) {

    if (argc != 3)
    {
        cerr << "Wrong argument count" << endl;
        return 1;
    }

    WAVFile wav(argv[1]);
    if (wav.read()) {        
        //wav.printInfo();
    }
    //wav.printAudioSamples(20);

    vector<int16_t> data = wav.get_int16_vector();

    vector<int16_t> first_element_of_group;
    vector<int16_t> other_elements_of_group;

    int16_t a = 0;
    for (size_t i = 0; i < data.size(); i++)
    {
        if ((i % GROUP) == 0)
        {
            a = data[i];
            first_element_of_group.push_back(data[i]);
        }
        else
        {
            other_elements_of_group.push_back(data[i] - a);
        }
    }

    vector<uint8_t> encoded_data_in_bytes[2];
    vector<uint8_t> huffmancode_str[2];
    encode_full(first_element_of_group, encoded_data_in_bytes[0], huffmancode_str[0]);
    encode_full(other_elements_of_group, encoded_data_in_bytes[1], huffmancode_str[1]);

    huffman_offsets table_offset;
    table_offset.ht1_size = huffmancode_str[0].size();
    table_offset.h1_size = encoded_data_in_bytes[0].size();
    table_offset.ht2_size = huffmancode_str[1].size();
    table_offset.h2_size = encoded_data_in_bytes[1].size();

    WAVHeader h = wav.get_header();
    ofstream outFile(argv[2], std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(&h), sizeof(WAVHeader));
    outFile.write(reinterpret_cast<const char*>(&table_offset), sizeof(table_offset));

    outFile.write(reinterpret_cast<const char*>(huffmancode_str[0].data()), huffmancode_str[0].size());
    outFile.write(reinterpret_cast<const char*>(encoded_data_in_bytes[0].data()), encoded_data_in_bytes[0].size());
    outFile.write(reinterpret_cast<const char*>(huffmancode_str[1].data()), huffmancode_str[1].size());
    outFile.write(reinterpret_cast<const char*>(encoded_data_in_bytes[1].data()), encoded_data_in_bytes[1].size());
    outFile.close();

    // cout << "encoded size: " << encoded_data_in_bytes[0].size() << endl;
    // cout << "encoded size: " << encoded_data_in_bytes[1].size() << endl;

    return 0;
}