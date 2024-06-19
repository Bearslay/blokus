#ifndef BLOKUS_POLYOMINOES_hpp
#define BLOKUS_POLYOMINOES_hpp

#include <vector>
#include <utility>
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
**/

namespace blokus {
    typedef enum {
        AMT_MONOMINO = 1,
        AMT_DOMINO = 1,
        AMT_TROMINO = 2,
        AMT_TETROMINO = 5,
        AMT_PENTOMINO = 12,
        AMT_HEXOMINO = 35,
        AMT_HEPTOMINO = 108,
        AMT_OCTOMINO = 369,
        AMT_NONOMINO = 1285,
        AMT_DECOMINO = 4655,
        AMT_BASE = 21
    } polyominoAmounts;

    std::vector<std::vector<std::vector<bool>>> basePolyominoes, hexominoes, heptominoes;

    std::vector<std::vector<std::vector<bool>>> readPolyominoFile(const char* filepath) {
        std::vector<std::vector<std::vector<bool>>> output;
        
        std::ifstream file;
        file.open(filepath, std::ios::in);
        if (!file.is_open()) {return output;}

        // Current line being processed from the file
        std::string line;

        std::getline(file, line);
        try {
            std::stoi(line);
        } catch (...) {
            file.close();
            return output;
        }
        // The dimensions of each polyomino in the file
        const unsigned char dims = std::stoi(line);

        std::getline(file, line);
        try {
            std::stoi(line);
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

    std::vector<std::vector<std::vector<bool>>> enlargePolyominoList(const std::vector<std::vector<std::vector<bool>>> &input, const unsigned char &increase = 1) {
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
}

#endif /* BLOKUS_POLYOMINOES_hpp */
