#ifndef BLOKUS_POLYOMINOES
#define BLOKUS_POLYOMINOES
#include <vector>
#include <fstream>

/** Formatting for polyomino storage files:
 * d (dims of each polyomino; usually the amount of cells in each)
 * c (amount of polyominoes in the file)
 * 
 * [polyomino 1: grid line 1]
 * [polyomino 1: grid line ...]
 * [polyomino 1: grid line d]
 * 
 * [polyomino ...: grid line 1]
 * [polyomino ...: grid line ...]
 * [polyomino ...: grid line d]
 * 
 * [polyomino c: grid line 1]
 * [polyomino c: grid line ...]
 * [polyomino c: grid line d]
 */

// Transfers the data from formatted text files containing polyominoes into a vector to be used
std::vector<std::vector<std::vector<bool>>> readPolyominoFile(const char* filename) {
    std::vector<std::vector<std::vector<bool>>> output;
    
    std::ifstream file;
    file.open(filename, std::ios::in);
    if (!file.is_open()) {return output;}

    // Current line being processed from the file
    std::string line;

    std::getline(file, line);
    try {
        const unsigned char temp = (unsigned char)std::stoi(line);
    } catch (...) {
        file.close();
        return output;
    }
    // The dimensions of each polyomino in the file
    const unsigned char dims = std::stoi(line);

    std::getline(file, line);
    try {
        const unsigned short temp = (unsigned short)std::stoi(line);
    } catch (...) {
        file.close();
        return output;
    }
    // The amount of polyominoes in the file
    const unsigned short count = std::stoi(line);
    
    // Place each polyomino into the array correctly
    for (int i = 0; i < count; i++) {
        output.emplace_back();
        std::getline(file, line);

        for (unsigned char j = 0; j < dims; j++) {
            output[i].emplace_back();
            std::getline(file, line);

            for (unsigned char k = 0; k < line.length(); k++) {
                output[i][j].emplace_back(line[k] == '1');
            }
        }
    }

    file.close();
    return output;
}

// Takes a polyomino grid and adds one row/column to this; where the row/column is added is determined by the input size of the polyomino
std::vector<std::vector<std::vector<bool>>> polyominoesEnlarge(const std::vector<std::vector<std::vector<bool>>> &input, const unsigned char &increase = 1) {
    std::vector<std::vector<std::vector<bool>>> output = input;

    for (unsigned char i = 0; i < increase; i++) {
        const unsigned char size = output[0].size();

        for (unsigned char j = 0; j < output.size(); j++) {
            // Add a new row
            output[j].emplace(output[j].begin() + ((size % 2 == 0) ? 0 : size));

            // Fill new row with false
            for (unsigned char k = 0; k < size + 1; k++) {
                output[j][(size % 2 == 0) ? 0 : size].emplace_back(false);
            }

            // Add a new column
            for (unsigned char k = ((size % 2 == 0) ? 1 : 0); k < size + ((size % 2 == 0) ? 1 : 0); k++) {
                output[j][k].emplace(output[j][k].begin() + ((size % 2 == 0) ? 0 : size), false);
            }
        }
    }

    return output;
}

#endif /* BLOKUS_POLYOMINOES */
