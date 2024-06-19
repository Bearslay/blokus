#ifndef BLOKUS_hpp
#define BLOKUS_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <utility>

#include "btils.hpp"
#include "bengine.hpp"

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
        MOVE_EAST,
        MOVE_NORTH,
        MOVE_WEST,
        MOVE_SOUTH
    } movementDirections;

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
        if (!file.is_open()) {
            std::cout << "ERROR: Could not open file " << filepath << "\n";
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

    class player {
        private:
            std::u16string name = u"Red";
            bengine::moddedTexture tile = NULL;

            std::vector<std::vector<blokus::piece>> pieces;

        public:
            player(bengine::window &window, const Uint8 &baseSets = 1, const Uint8 &hexSets = 0, const Uint8 &heptSets = 0) {
                this->tile = bengine::moddedTexture(window.loadTexture("dev/png/tile.png"), {0, 0, 64, 64});

                const Uint8 base = baseSets < 1 ? 1 : (baseSets > 12 ? 12 : baseSets), hex = hexSets > 4 ? 4 : hexSets, hept = heptSets > 2 ? 2 : heptSets;
                this->pieces.emplace_back();
                for (Uint8 i = 0; i < base; i++) {
                    for (Uint8 j = 0; j < blokus::AMT_BASE; j++) {
                        this->pieces[0].emplace_back(blokus::piece(j));
                    }
                }
                Uint16 idStart = blokus::AMT_BASE;

                if (hex > 0) {
                    this->pieces.emplace_back();
                    for (Uint8 i = 0; i < hex; i++) {
                        for (Uint8 j = 0; j < blokus::AMT_HEXOMINO; j++) {
                            this->pieces[1].emplace_back(blokus::piece(idStart + j));
                        }
                    }
                }
                idStart += blokus::AMT_HEXOMINO;

                if (hept > 0) {
                    this->pieces.emplace_back();
                    for (Uint8 i = 0; i < hept; i++) {
                        for (Uint8 j = 0; j < blokus::AMT_HEPTOMINO; j++) {
                            this->pieces[2].emplace_back(blokus::piece(idStart + j));
                        }
                    }
                }
                // idStart += blokus::AMT_HEPTOMINO;
            }

            std::u16string getName() const {
                return this->name;
            }
            bengine::moddedTexture getTile() const {
                return this->tile;
            }
    };

    class game : public bengine::loop {
        private:
            std::vector<blokus::player> players = {};
            Uint8 turn = 0;

            std::vector<std::vector<Uint8>> board = {};

            bengine::basicTexture boardFrame = bengine::basicTexture(this->window.loadTexture("dev/png/board_frame.png"), {0, 0, 1012, 1012});
            bengine::basicTexture emptyCell = bengine::basicTexture(this->window.loadTexture("dev/png/empty_cell.png"), {0, 0, 64, 64});

            void handleEvent() {

            }

            void compute() {
                this->visualsChanged = true;
            }

            void render() {
                this->window.renderBasicTexture(boardFrame, {454, 34, 1012, 1012});

                for (Uint8 i = 0; i < this->board.size(); i++) {
                    for (Uint8 j = 0; j < this->board.size(); j++) {
                        if (this->board.at(i).at(j) == 0) {
                            this->window.renderBasicTexture(emptyCell, {480 + j * 48, 60 + i * 48, 48, 48});
                        }
                    }
                }
            }

        public:
            game(const Uint8 &boardSize = 20, const Uint8 &baseSets = 1, const Uint8 &hexSets = 0, const Uint8 &heptSets = 0, const Uint8 &playerCount = 4) : bengine::loop("Blokus", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_FULLSCREEN) {               
                blokus::basePolyominoes = blokus::readPolyominoFile("dev/polyominoes/base.txt");
                blokus::hexominoes = blokus::readPolyominoFile("dev/polyominoes/hexominoes.txt");
                blokus::heptominoes = blokus::readPolyominoFile("dev/polyominoes/heptominoes.txt");

                const Uint8 players = playerCount < 1 ? 1 : (playerCount > 4 ? 4 : playerCount);
                for (Uint8 i = 0; i < players; i++) {
                    this->players.emplace_back(blokus::player(this->window, baseSets, hexSets, heptSets));
                }

                const Uint8 size = boardSize < 20 ? 20 : (boardSize > 100 ? 100 : boardSize);
                for (Uint8 i = 0; i < size; i++) {
                    this->board.emplace_back();
                    for (Uint8 j = 0; j < size; j++) {
                        this->board[i].emplace_back(0);
                    }
                }
            }
    };
}

#endif /* BLOKUS_hpp */
