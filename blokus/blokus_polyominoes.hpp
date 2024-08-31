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

namespace blokus {
    /// @brief The amount of polyominoes in each set
    const unsigned short polyominoAmounts[6] = {21, 35, 108, 369, 1285, 4655};
    /// @brief The amount of tiles each polyomino within each set takes up (the amount of cells that makes up a given polyomino)
    const unsigned char polyominoTiles[6] = {0, 6, 7, 8, 9, 10};
    /// @brief The total amount of tiles each polyomino set takes up (pretty much blokus::polyominoAmounts[x] * blokus::polyominoTiles[x])
    const unsigned short polyominoTileTotals[6] = {89, 210, 756, 2952, 11565, 46550};

    typedef std::size_t polyType;
    typedef std::size_t polyominoType;
    typedef enum {
        POLYTYPE_BASE = 0,             // Monominoes, Dominoes, Triominoes, Tetrominoes, Pentominoes
        POLYTYPE_HEXOMINO = 1,         // Hexominoes
        POLYTYPE_HEPTOMINO = 2,        // Heptominoes
        POLYTYPE_OCTOMINO = 3,         // Octominoes
        POLYTYPE_NONOMINO = 4,         // Nonominoes
        POLYTYPE_DECOMINO = 5,         // Decominoes
        POLYTYPE_HEX = 1,              // Hexominoes
        POLYTYPE_HEPT = 2,             // Heptominoes
        POLYTYPE_OCT = 3,              // Octominoes
        POLYTYPE_NON = 4,              // Nonominoes
        POLYTYPE_DEC = 5,              // Decominoes
        POLYTYPE_SENTINAL = UINT64_MAX // A sentinal value for polyomino types
    } polyominoTypes;

    /** Load a text file containing polyomino information into a 3-dimensional std::vector
     * @param filepath The path to the file to read from
     * @returns A 3-dimensional std::vector containing the information for a list of polyominoes
     */
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

    /** Increase the dimensions of a polyomino grid by an amount while keeping the original grid pseudo-centered
     * @param input The inputted polyomino grid to enlarge
     * @param cycles The amount of times to increase the grid size
     * @returns The enlarged polyomino grid
     */
    std::vector<std::vector<bool>> enlargePolyominoGrid(const std::vector<std::vector<bool>> &input, const unsigned char &cycles = 1) {
        std::vector<std::vector<bool>> output = input;

        for (unsigned char i = 0; i < cycles; i++) {
            // Add a new row
            output.emplace(output.begin() + ((output.size() % 2 == 0) ? 0 : output.size()));

            // Fill new row with false
            for (unsigned char j = 0; j < output.size() + 1; j++) {
                output[(output.size() % 2 == 0) ? 0 : output.size()].emplace_back(false);
            }

            // Add a new column
            for (unsigned char j = ((output.size() % 2 == 0) ? 1 : 0); j < output.size() + ((output.size() % 2 == 0) ? 1 : 0); j++) {
                output[j].emplace(output[j].begin() + ((output.size() % 2 == 0) ? 0 : output.size()), false);
            }
        }

        return output;
    }

    /** Increase the dimensions of a list of polyomino grids by an amount while keeping the original grids pseudo-centered
     * @param input The inputted list of polyomino grids to enlarge
     * @param cycles The amount of times to increase all of the grids' size
     * @returns The list of enlarged polyomino grids
     */
    std::vector<std::vector<std::vector<bool>>> enlargePolyominoList(const std::vector<std::vector<std::vector<bool>>> &input, const unsigned char &cycles = 1) {
        std::vector<std::vector<std::vector<bool>>> output;

        for (unsigned char i = 0; i < output.size(); i++) {
            output.emplace_back(blokus::enlargePolyominoGrid(input.at(i), cycles));
        }

        return output;
    }

    /// @brief A list of lists of grids that represent the cell data of each polyomino
    const std::vector<std::vector<std::vector<std::vector<bool>>>> rawPolyominoData = {
        blokus::readPolyominoFile("dev/polyominoes/base.txt"),
        blokus::readPolyominoFile("dev/polyominoes/hexominoes.txt"),
        blokus::readPolyominoFile("dev/polyominoes/heptominoes.txt"),
        blokus::readPolyominoFile("dev/polyominoes/octominoes.txt")
    };
}

#endif // BLOKUS_POLYOMINOES_hpp
