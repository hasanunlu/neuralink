using namespace std;

#define GROUP 2

#pragma pack(push, 1)
struct WAVHeader {
    char riffHeader[4];      // "RIFF"
    uint32_t fileSize;       // File size in bytes
    char waveHeader[4];      // "WAVE"
    char fmtHeader[4];       // "fmt "
    uint32_t fmtChunkSize;   // Size of the fmt chunk
    uint16_t audioFormat;    // Audio format (1 for PCM)
    uint16_t numChannels;    // Number of channels
    uint32_t sampleRate;     // Sampling frequency
    uint32_t byteRate;       // Byte rate = SampleRate * NumChannels * BitsPerSample/8
    uint16_t blockAlign;     // Block align = NumChannels * BitsPerSample/8
    uint16_t bitsPerSample;  // Bits per sample
    char dataHeader[4];      // "data"
    uint32_t dataSize;       // Size of the data chunk
};
#pragma pack(pop)

#pragma pack(push, 1)
struct huffman_offsets {
    uint32_t ht1_size;
    uint32_t h1_size;
    uint32_t ht2_size;
    uint32_t h2_size;
};
#pragma pack(pop)

vector<uint8_t> packBitsToBytes(const string& huffmanEncoded) {
    vector<uint8_t> packedBytes;
    int byte = 0;
    int bitCount = 0;

    for (char bit : huffmanEncoded) {
        byte = (byte << 1) | (bit - '0');
        bitCount++;

        if (bitCount == 8) {
            packedBytes.push_back(static_cast<uint8_t>(byte));
            byte = 0;
            bitCount = 0;
        }
    }

    // Handle remaining bits (if any)
    if (bitCount > 0) {
        byte = byte << (8 - bitCount); // Pad the remaining bits with 0s
        packedBytes.push_back(static_cast<uint8_t>(byte));
    }

    return packedBytes;
}

// Function to unpack bytes to a Huffman encoded string
string unpackBytesToBits(const vector<uint8_t>& packedBytes) {
    string huffmanEncoded;

    for (uint8_t byte : packedBytes) {
        for (int i = 7; i >= 0; --i) {
            huffmanEncoded += ((byte >> i) & 1) ? '1' : '0';
        }
    }

    return huffmanEncoded;
}