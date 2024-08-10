#ifndef BLOKUS_POLYOMINOES_hpp
#define BLOKUS_POLYOMINOES_hpp

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

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

// #define ALIGN_LEFT 0
// #define ALIGN_CENTER 1
// #define ALIGN_RIGHT 2

// class paddedGrid {
//     public:
//         unsigned int calcUsableLength(const unsigned int &length, const unsigned short &cellAmount, const unsigned short &gapDim) {
//             return length - gapDim * (cellAmount - 1);
//         }
//         unsigned short calcCellDim(const unsigned int &length, const unsigned short &cellAmount, const unsigned short &gapDim) {
//             return this->calcUsableLength(length, cellAmount, gapDim) / cellAmount;
//         }
//         unsigned short calcOffset(const unsigned int &length, const unsigned short &cellAmount, const unsigned short &gapDim, const unsigned char &alignment = ALIGN_CENTER) {
//             switch (alignment) {
//                 case ALIGN_LEFT:
//                     return 0;
//                 default:
//                 case ALIGN_CENTER:
//                     return (this->calcUsableLength(length, cellAmount, gapDim) - this->calcCellDim(length, cellAmount, gapDim) * cellAmount) / 2;
//                 case ALIGN_RIGHT:
//                     return this->calcUsableLength(length, cellAmount, gapDim) - this->calcCellDim(length, cellAmount, gapDim) * cellAmount;
//             }
//         }
//         unsigned short calcOffset(const unsigned int &usableLength, const unsigned short &cellAmount, const unsigned short &cellDim, const unsigned char &alignment = ALIGN_CENTER) {
//             switch (alignment) {
//                 case ALIGN_LEFT:
//                     return 0;
//                 default:
//                 case ALIGN_CENTER:
//                     return (usableLength - cellDim * cellAmount) / 2;
//                 case ALIGN_RIGHT:
//                     return usableLength - cellDim * cellAmount;
//             }
//         }

//         unsigned short calcSquareCellDim(const unsigned int &width, const unsigned int &height, const unsigned short &columns, const unsigned short &rows, const unsigned short &gapWidth, const unsigned short &gapHeight) {
//             unsigned short width = this->calcCellDim(width, columns, gapWidth);
//             unsigned short height = this->calcCellDim(height, rows, gapHeight);
//             return width < height ? width : height;
//         }
// };

namespace blokus {
    const unsigned short polyominoAmounts[6] = {21, 35, 108, 369, 1285, 4655};
    const unsigned char polyominoTiles[6] = {0, 6, 7, 8, 9, 10};
    const unsigned short polyominoTileTotals[6] = {89, 210, 756, 2952, 11565, 46550};

    typedef std::size_t polyType;
    typedef std::size_t polyominoType;
    typedef enum {
        POLYTYPE_BASE = 0,
        POLYTYPE_HEXOMINO = 1,
        POLYTYPE_HEPTOMINO = 2,
        POLYTYPE_OCTOMINO = 3,
        POLYTYPE_NONOMINO = 4,
        POLYTYPE_DECOMINO = 5,
        POLYTYPE_HEX = 1,
        POLYTYPE_HEPT = 2,
        POLYTYPE_OCT = 3,
        POLYTYPE_NON = 4,
        POLYTYPE_DEC = 5,
        POLYTYPE_SENTINAL = -1
    } polyominoTypes;

    std::vector<std::vector<std::vector<bool>>> readPolyominoFile(const char* filepath) {
        std::vector<std::vector<std::vector<bool>>> output;
        
        std::ifstream file;
        file.open(filepath, std::ios::in);
        if (!file.is_open()) {
            std::cout << "ERROR: Could not open file" << filepath << "\n";
            return output;
        }

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
        for (unsigned short i = 0; i < count; i++) {
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

    std::vector<std::vector<std::vector<std::vector<bool>>>> rawPolyominoData = {
        blokus::readPolyominoFile("dev/polyominoes/base.txt"),
        blokus::readPolyominoFile("dev/polyominoes/hexominoes.txt"),
        blokus::readPolyominoFile("dev/polyominoes/heptominoes.txt"),
        blokus::readPolyominoFile("dev/polyominoes/octominoes.txt")
    };
}

#endif // BLOKUS_POLYOMINOES_hpp
