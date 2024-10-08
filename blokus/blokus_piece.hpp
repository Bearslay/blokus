#ifndef BLOKUS_PIECE_hpp
#define BLOKUS_PIECE_hpp

#include <iostream>
#include <vector>

#include "btils.hpp"

#include "blokus_polyominoes.hpp"

namespace blokus {
    typedef enum {
        MOVE_EAST,
        MOVE_NORTH,
        MOVE_WEST,
        MOVE_SOUTH
    } movementDirections;

    /// @brief A Piece used for Blokus; contains grid, position, and misc data
    class piece {
        private:
            /// @brief The id of the piece; relates directly to which grid was used from the raw polyomino data set
            unsigned short id = 0;
            /// @brief The amount of tiles that the piece takes up
            unsigned char tiles = 0;

            /// @brief The piece's x-position on the board related to the top-left corner of the piece's grid; can be negative when placed near the edge of the board
            short x = 0;
            /// @brief The piece's y-position on the board related to the top-left corner of the piece's grid; can be negative when placed near the edge of the board
            short y = 0;
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
            /** Get the x-position of the piece
             * @returns The x-position of the piece on the board in relation to the grid's top-left corner
             */
            short getX() const {
                return this->x;
            }
            /** Get the y-position of the piece
             * @returns The y-position of the piece on the board in relation to the grid's top-left corner
             */
            short getY() const {
                return this->y;
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
            /** Move the piece one tile in a direction
             * @param direction Which direction to move the piece (it's best to use the movementDirections enum)
             */
            void move(const unsigned char &direction) {
                switch (direction) {
                    default:
                    case blokus::MOVE_EAST:
                        x++;
                        return;
                    case blokus::MOVE_NORTH:
                        y++;
                        return;
                    case blokus::MOVE_WEST:
                        x--;
                        return;
                    case blokus::MOVE_SOUTH:
                        y--;
                        return;
                }
            }
            /** Return the piece to an in-bounds position in the case that one of its tiles clips out of bounds
             * @param boardSize The dimensions of the board in tiles
             */
            void fixPos(const unsigned char &boardSize) {
                bool ok = false, error = false;

                while (!ok) {
                    ok = true;

                    for (unsigned char i = 0; i < this->grid.size(); i++) {
                        for (unsigned char j = 0; j < this->grid.size(); j++) {
                            if (this->x + i < 0 && this->grid.at(j).at(i)) {
                                this->x++;
                                error = true;
                            }
                            if (this->x + i >= boardSize && this->grid.at(this->grid.size() - 1 - j).at(i)) {
                                this->x--;
                                error = true;
                            }
                            if (this->y + i < 0 && this->grid.at(i).at(j)) {
                                this->y++;
                                error = true;
                            }
                            if (this->y + i >= boardSize && this->grid.at(i).at(this->grid.size() - 1 - j)) {
                                this->y--;
                                error = true;
                            }

                            if (error) {
                                error = false;
                                ok = false;
                                break;
                            }
                        }
                    }
                }
            }
    };
}

#endif // BLOKUS_PIECE_hpp
