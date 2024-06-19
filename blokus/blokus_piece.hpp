#ifndef BLOKUS_PIECE_hpp
#define BLOKUS_PIECE_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "btils.hpp"
#include "bengine.hpp"

#include "blokus_polyominoes.hpp"
#include "blokus_player.hpp"
#include "blokus_game.hpp"

namespace blokus {
    typedef enum {
        MOVE_EAST,
        MOVE_NORTH,
        MOVE_WEST,
        MOVE_SOUTH
    } movementDirections;

    class player;
    class game;

    class piece {
        private:
            Uint16 id = 0;
            Uint8 tiles = 1;

            char x = 0;
            char y = 0;
            std::vector<std::vector<bool>> grid = {};

        public:
            piece(const Uint16 &id = 0) {
                Uint16 maxId = blokus::AMT_BASE;
                if (id < maxId) {
                    this->grid = blokus::basePolyominoes.at(id);
                    for (Uint8 i = 0; i < blokus::basePolyominoes.at(id).size(); i++) {
                        for (Uint8 j = 0; j < blokus::basePolyominoes.at(id).size(); j++) {
                            if (blokus::basePolyominoes.at(id).at(i).at(j)) {
                                this->tiles++;
                            }
                        }
                    }
                } else if (id < (maxId += blokus::AMT_HEXOMINO)) {
                    this->grid = blokus::hexominoes.at(id);
                    this->tiles = 6;
                } else if (id < (maxId += blokus::AMT_HEPTOMINO)) {
                    this->grid = blokus::heptominoes.at(id);
                    this->tiles = 7;
                } else if (id < (maxId += blokus::AMT_OCTOMINO)) {
                    this->tiles = 8;
                } else if (id < (maxId += blokus::AMT_NONOMINO)) {
                    this->tiles = 9;
                } else if (id < (maxId += blokus::AMT_DECOMINO)) {
                    this->tiles = 10;
                } else {
                    this->grid = blokus::basePolyominoes.at(0);
                    this->tiles = 1;
                }
            }

            Uint16 getId() const {
                return this->id;
            }
            Uint8 getTiles() const {
                return this->tiles;
            }
            char getX() const {
                return this->x;
            }
            char getY() const {
                return this->y;
            }

            void rotate(const bool &ccw = true, const Uint8 &rotations = 1) {
                for (char i = rotations % 4; i > 0; i--) {
                    this->grid = btils::rotateMatrix<bool>(this->grid, ccw);
                }
            }
            void flip(const bool &vertical = true, const Uint8 &flips = 1) {
                if (flips % 2 != 0) {
                    this->grid = btils::flipMatrix<bool>(this->grid, vertical);
                }
            }
            void move(const Uint8 &direction) {
                switch (direction) {
                    default:
                    case blokus::MOVE_EAST:
                        x++;
                        break;
                    case blokus::MOVE_NORTH:
                        y++;
                        break;
                    case blokus::MOVE_WEST:
                        x--;
                        break;
                    case blokus::MOVE_SOUTH:
                        y--;
                        break;
                }
            }
            void fixPos(const short &boardSize) {
                bool ok = false, error = false;

                while (!ok) {
                    ok = true;

                    for (Uint8 i = 0; i < this->grid.size(); i++) {
                        for (Uint8 j = 0; j < this->grid.size(); j++) {
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

            std::vector<std::vector<bool>> enlarge(const Uint8 &increase = 1) {
                std::vector<std::vector<bool>> output = this->grid;

                for (Uint8 i = 0; i < increase; i++) {
                    const Uint8 size = this->grid[0].size();

                    this->grid.emplace(this->grid.begin() + ((size % 2 == 0) ? 0 : size));

                    for (Uint8 j = 0; j < size + 1; j++) {
                        this->grid[(size % 2 == 0) ? 0 : size].emplace_back(false);
                    }
                    for (Uint8 j = ((size % 2 == 0) ? 1 : 0); j < size + ((size % 2 == 0) ? 1 : 0); j++) {
                        this->grid[j].emplace(this->grid[j].begin() + ((size % 2 == 0) ? 0 : size), false);
                    }
                }
                return output;
            }
    };
}

#endif /* BLOKUS_PIECE_hpp */
