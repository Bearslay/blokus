#ifndef BLOKUS_PIECE_hpp
#define BLOKUS_PIECE_hpp

#include <iostream>
#include <vector>

#include "btils.hpp"

#include "blokus_polyominoes.hpp"

namespace blokus {
    /// @brief A Piece used for Blokus; contains grid, position, and misc data
    class piece {
        private:
            /// @brief The id of the piece; relates directly to which grid was used from the raw polyomino data set
            unsigned short id = 0;
            /// @brief The amount of tiles that the piece takes up
            unsigned char tiles = 0;

            /// @brief The grid that contains the location of the tiles within the piece; can be rotated/flipped as needed
            std::vector<std::vector<bool>> grid = {};

        public:
            /** Constructor
             * @param id The id of the piece, related to which specific polyomino the piece shall be
             */
            piece(const unsigned short &id = 0) {
                // The base set of polyominoes has a special way of determining the amount of tiles a given piece has, hence the seperation
                if (id < blokus::polyominoAmounts[blokus::POLYTYPE_BASE]) {
                    this->id = id;
                    this->grid = blokus::rawPolyominoData.at(blokus::POLYTYPE_BASE).at(id);
                    for (unsigned char i = 0; i < blokus::rawPolyominoData.at(blokus::POLYTYPE_BASE).at(id).size(); i++) {
                        for (unsigned char j = 0; j < blokus::rawPolyominoData.at(blokus::POLYTYPE_BASE).at(id).at(i).size(); j++) {
                            if (blokus::rawPolyominoData.at(blokus::POLYTYPE_BASE).at(id).at(i).at(j)) {
                                this->tiles++;
                            }
                        }
                    }
                    return;
                }

                // Hyjacking the piece's id for use as a variable before assigning it its real value
                this->id = blokus::polyominoAmounts[blokus::POLYTYPE_BASE];
                // Any non-base piece's properties can be determined purely by its id so can be lumped together code-wise
                for (blokus::polyominoType i = blokus::POLYTYPE_HEX; i <= blokus::POLYTYPE_OCT; i++) {
                    if (id < this->id + blokus::polyominoAmounts[i]) {
                        this->id = id;
                        this->grid = blokus::rawPolyominoData.at(i).at(id - this->id);
                        this->tiles = i + 5;
                        return;
                    }
                    this->id += blokus::polyominoAmounts[i];
                }

                // Default case for a piece given a bad id (1x1 tile)
                this->id = 0;
                this->grid = blokus::rawPolyominoData.at(blokus::POLYTYPE_BASE).at(0);
                this->tiles = 1;
            }

            /// @brief Print the piece's grid to the standard iostream
            void print() const {
                std::cout << this->id << "\n";
                for (unsigned char i = 0; i < this->grid.size(); i++) {
                    for (unsigned char j = 0; j < this->grid.at(i).size(); j++) {
                        std::cout << (this->grid.at(i).at(j) ? "██" : "░░");
                    }
                    std::cout << "\n";
                }
            }

            /** Get the id of the piece
             * @returns The id of the piece
             */ 
            unsigned short getId() const {
                return this->id;
            }
            /** Get the amount of tiles the piece takes up
             * @returns The amount of tiles the piece takes up
             */
            unsigned char getTiles() const {
                return this->tiles;
            }

            /** Rotate the piece by 90 degrees an amount of times
             * @param ccw Whether to rotate counter-clockwise (true) or clockwise (false)
             * @param rotations The amount of times to rotate the piece
             */
            void rotate(const bool &ccw = true, const unsigned char &rotations = 1) {
                for (char i = rotations % 4; i > 0; i--) {
                    this->grid = btils::rotateMatrix<bool>(this->grid, ccw);
                }
            }
            /** Flip the piece an amount of times
             * @param vertical Whether to flip the piece vertically (true) or horizontally (false)
             * @param flips The amount of times to flip the piece
             */
            void flip(const bool &vertical = true, const unsigned char &flips = 1) {
                if (flips % 2 != 0) {
                    this->grid = btils::flipMatrix<bool>(this->grid, vertical);
                }
            }

            /** Get the polynomial grid associated with this piece
             * @returns A 2D boolean vector containing the tiles for this piece
             */
            std::vector<std::vector<bool>> getGrid() const {
                return this->grid;
            }
    };
}

#endif // BLOKUS_PIECE_hpp
