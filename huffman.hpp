#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

// Huffman Tree Node
struct Node {
    int16_t value;
    int freq;
    Node* left;
    Node* right;

    Node(int16_t value, int freq) : value(value), freq(freq), left(nullptr), right(nullptr) {}
};

// Comparator to order the heap
struct Compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

// Function to generate Huffman codes
void generateCodes(Node* root, const string& str, unordered_map<int16_t, string>& huffmanCode) {
    if (root == nullptr)
        return;

    // Leaf node
    if (!root->left && !root->right) {
        huffmanCode[root->value] = str;
    }

    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

// Function to build the Huffman Tree
Node* buildHuffmanTree(const vector<int16_t>& data) {
    // Count frequency of appearance of each value
    unordered_map<int16_t, int> freq;
    for (int16_t value : data) {
        freq[value]++;
    }

    // Create a priority queue to store live nodes of Huffman tree
    priority_queue<Node*, vector<Node*>, Compare> pq;

    // Create a leaf node for each value and add it to the priority queue
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Do until there is more than one node in the queue
    while (pq.size() != 1) {
        // Remove the two nodes of highest priority (lowest frequency) from the queue
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();

        // Create a new internal node with these two nodes as children and with frequency equal to the sum of the two nodes' frequencies
        int sum = left->freq + right->freq;
        Node* node = new Node('\0', sum);
        node->left = left;
        node->right = right;

        // Add the new node to the priority queue
        pq.push(node);
    }

    // The remaining node is the root node and the tree is complete
    return pq.top();
}

Node* buildTreeFromHuffmanCode(const unordered_map<int16_t, string>& huffmanCode) {
    Node* root = new Node(0, 0);

    for (const auto& pair : huffmanCode) {
        Node* current = root;
        const string& code = pair.second;
        int16_t value = pair.first;

        for (char bit : code) {
            if (bit == '0') {
                if (!current->left) {
                    current->left = new Node(0, 0);
                }
                current = current->left;
            } else if (bit == '1') {
                if (!current->right) {
                    current->right = new Node(0, 0);
                }
                current = current->right;
            }
        }
        current->value = value;
    }

    return root;
}

// Function to encode the input data using the Huffman codes
string encode(const vector<int16_t>& data, unordered_map<int16_t, string>& huffmanCode) {
    string encodedString;
    for (int16_t value : data) {
        encodedString += huffmanCode[value];
    }
    return encodedString;
}

// Function to decode the encoded string using the Huffman Tree
vector<int16_t> decode(Node* root, const string& encodedString) {
    vector<int16_t> decodedData;
    Node* currentNode = root;
    for (char bit : encodedString) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        // Leaf node
        if (!currentNode->left && !currentNode->right) {
            decodedData.push_back(currentNode->value);
            currentNode = root;
        }
    }
    return decodedData;
}

string serializeHuffmanTable(const unordered_map<int16_t, string>& huffmanCode) {
    ostringstream oss;
    for (const auto& pair : huffmanCode) {
        oss << pair.first << " " << pair.second << "\n";
    }
    return oss.str();
}

unordered_map<int16_t, string> deserializeHuffmanTable(const string& serializedTable) {
    unordered_map<int16_t, string> huffmanCode;
    istringstream iss(serializedTable);
    int16_t value;
    string code;

    while (iss >> value >> code) {
        huffmanCode[value] = code;
    }

    return huffmanCode;
}