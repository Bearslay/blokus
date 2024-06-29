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
        COUNT_MONOMINO = 1,
        COUNT_DOMINO = 1,
        COUNT_TROMINO = 2,
        COUNT_TETROMINO = 5,
        COUNT_PENTOMINO = 12,
        COUNT_HEXOMINO = 35,
        COUNT_HEPTOMINO = 108,
        COUNT_OCTOMINO = 369,
        COUNT_NONOMINO = 1285,
        COUNT_DECOMINO = 4655,
        COUNT_BASE = 21,
        TILES_MONOMINO = 1,
        TILES_DOMINO = 2,
        TILES_TROMINO = 3,
        TILES_TETROMINO = 4,
        TILES_PENTOMINO = 5,
        TILES_HEXOMINO = 6,
        TILES_HEPTOMINO = 7,
        TILES_OCTOMINO = 8,
        TILES_NONOMINO = 9,
        TILES_DECOMINO = 10,
        TILES_BASE = 89
    } polyominoAmounts;

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

    std::vector<std::vector<std::vector<bool>>> basePolyominoes = blokus::readPolyominoFile("dev/polyominoes/base.txt");
    std::vector<std::vector<std::vector<bool>>> hexominoes = blokus::readPolyominoFile("dev/polyominoes/hexominoes.txt");
    std::vector<std::vector<std::vector<bool>>> heptominoes = blokus::readPolyominoFile("dev/polyominoes/heptominoes.txt");

    class piece {
        private:
            Uint16 id = 0;
            Uint8 tiles = 0;

            char x = 0;
            char y = 0;
            std::vector<std::vector<bool>> grid = {};

        public:
            piece(const Uint16 &id = 0) {
                this->id = id;
                Uint16 maxId = blokus::COUNT_BASE;
                if (id < maxId) {
                    this->grid = blokus::basePolyominoes.at(id);
                    for (Uint8 i = 0; i < blokus::basePolyominoes.at(id).size(); i++) {
                        for (Uint8 j = 0; j < blokus::basePolyominoes.at(id).size(); j++) {
                            if (blokus::basePolyominoes.at(id).at(i).at(j)) {
                                this->tiles++;
                            }
                        }
                    }
                } else if (id < (maxId += blokus::COUNT_HEXOMINO)) {
                    this->grid = blokus::hexominoes.at(id - blokus::COUNT_BASE);
                    this->tiles = blokus::TILES_HEXOMINO;
                } else if (id < (maxId += blokus::COUNT_HEPTOMINO)) {
                    this->grid = blokus::heptominoes.at(id - blokus::COUNT_BASE - blokus::COUNT_HEXOMINO);
                    this->tiles = blokus::TILES_HEPTOMINO;
                } else if (id < (maxId += blokus::COUNT_OCTOMINO)) {
                    this->tiles = blokus::TILES_OCTOMINO;
                } else if (id < (maxId += blokus::COUNT_NONOMINO)) {
                    this->tiles = blokus::TILES_NONOMINO;
                } else if (id < (maxId += blokus::COUNT_DECOMINO)) {
                    this->tiles = blokus::TILES_DECOMINO;
                } else {
                    this->grid = blokus::basePolyominoes.at(0);
                    this->tiles = blokus::TILES_MONOMINO;
                    this->id = 0;
                }
            }

            void print() const {
                std::cout << this->id << "\n";
                for (Uint8 i = 0; i < this->grid.size(); i++) {
                    for (Uint8 j = 0; j < this->grid.size(); j++) {
                        std::cout << (this->grid.at(i).at(j) ? "██" : "░░");
                    }
                    std::cout << "\n";
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
    };

    class player {
        private:
            std::u16string name = u"Red";
            SDL_Color color = {255, 0, 0, 255};

            std::vector<std::vector<blokus::piece>> pieces;

        public:
            player(const Uint8 &baseSets = 1, const Uint8 &hexSets = 0, const Uint8 &heptSets = 0) {
                const Uint8 base = baseSets < 1 ? 1 : (baseSets > 12 ? 12 : baseSets), hex = hexSets > 4 ? 4 : hexSets, hept = heptSets > 2 ? 2 : heptSets;
                this->pieces.emplace_back();
                for (Uint8 i = 0; i < base; i++) {
                    for (Uint8 j = 0; j < blokus::COUNT_BASE; j++) {
                        this->pieces[0].emplace_back(blokus::piece(j));
                    }
                }
                Uint16 idStart = blokus::COUNT_BASE;

                if (hex > 0) {
                    this->pieces.emplace_back();
                    for (Uint8 i = 0; i < hex; i++) {
                        for (Uint8 j = 0; j < blokus::COUNT_HEXOMINO; j++) {
                            this->pieces[1].emplace_back(blokus::piece(idStart + j));
                        }
                    }
                }
                idStart += blokus::COUNT_HEXOMINO;

                if (hept > 0) {
                    this->pieces.emplace_back();
                    for (Uint8 i = 0; i < hept; i++) {
                        for (Uint8 j = 0; j < blokus::COUNT_HEPTOMINO; j++) {
                            this->pieces[2].emplace_back(blokus::piece(idStart + j));
                        }
                    }
                }
                // idStart += blokus::COUNT_HEPTOMINO;
            }

            void printBasePieces() const {
                for (Uint16 i = 0; i < this->pieces.at(0).size(); i++) {
                    this->pieces.at(0).at(i).print();
                }
            }
            void printHexPieces() const {
                if (this->pieces.size() > 1) {
                    for (Uint16 i = 0; i < this->pieces.at(1).size(); i++) {
                        this->pieces.at(1).at(i).print();
                    }
                }
            }
            void printHeptPieces() const {
                if (this->pieces.size() > 2) {
                    for (Uint16 i = 0; i < this->pieces.at(2).size(); i++) {
                        this->pieces.at(2).at(i).print();
                    }
                }
            }

            Uint16 remainingBasePolyominoes() const {
                return (Uint16)this->pieces.at(0).size();
            }
            Uint16 remainingHexominoes() const {
                return this->pieces.size() > 1 ? (Uint16)this->pieces.at(1).size() : 0;
            }
            Uint16 remainingHeptominoes() const {
                return this->pieces.size() > 2 ? (Uint16)this->pieces.at(2).size() : 0;
            }
            Uint16 remaingingTiles_Base() const {
                Uint16 output = 0;
                for (Uint8 i = 0; i < this->pieces.at(0).size(); i++) {
                    output += this->pieces.at(0).at(i).getTiles();
                }
                return output;
            }
            Uint16 remaingingTiles_Hex() const {
                return this->pieces.size() > 1 ? (Uint16)(this->pieces.at(1).size() * 6) : 0;
            }
            Uint16 remaingingTiles_Hept() const {
                return this->pieces.size() > 1 ? (Uint16)(this->pieces.at(2).size() * 7) : 0;
            }

            std::u16string getName() const {
                return this->name;
            }
            std::u16string setName(const std::u16string &name) {
                return btils::set<std::u16string>(this->name, name);
            }
            SDL_Color getColor() const {
                return this->color;
            }
            SDL_Color setColor(const SDL_Color &color) {
                const SDL_Color output = this->color;
                this->color.r = color.r;
                this->color.g = color.g;
                this->color.b = color.b;
                this->color.a = color.a;
                return output;
            }
    };

    class game : public bengine::loop {
        private:
            std::vector<blokus::player> players = {};
            Uint8 turn = 0;
            Uint8 baseSets = 1;
            Uint8 hexSets = 0;
            Uint8 heptSets = 0;

            std::vector<std::vector<Uint8>> board = {};

            TTF_Font* playerTitleFont = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 25);
            TTF_Font* playerButtonFont = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 24);

            bengine::basicTexture boardFrameBase = bengine::basicTexture(this->window.loadTexture("dev/png/board_frame_base.png"), {0, 0, 1012, 1012});
            bengine::moddedTexture boardFrameCorner = bengine::moddedTexture(this->window.loadTexture("dev/png/board_frame_corner.png"), {0, 0, 506, 506});
            bengine::basicTexture boardFrame;

            bengine::moddedTexture tile = bengine::moddedTexture(this->window.loadTexture("dev/png/tile.png"), {0, 0, 64, 64});
            bengine::basicTexture emptyCell = bengine::basicTexture(this->window.loadTexture("dev/png/empty_cell.png"), {0, 0, 64, 64});
            bengine::basicTexture boardTexture;

            bengine::moddedTexture playerFrameLarge = bengine::moddedTexture(this->window.loadTexture("dev/png/player_frame_large.png"), {0, 0, 392, 1012});
            bengine::moddedTexture playerFrameSmall = bengine::moddedTexture(this->window.loadTexture("dev/png/player_frame_small.png"), {0, 0, 392, 489});
            bengine::moddedTexture playerButtonLarge = bengine::moddedTexture(this->window.loadTexture("dev/png/player_button_large.png"), {0, 0, 366, 52});
            bengine::moddedTexture playerButtonMedium = bengine::moddedTexture(this->window.loadTexture("dev/png/player_button_medium.png"), {0, 0, 180, 52});
            bengine::moddedTexture playerButtonSmall = bengine::moddedTexture(this->window.loadTexture("dev/png/player_button_small.png"), {0, 0, 118, 52});

            void handleEvent() {

            }

            void compute() {
                
            }

            void renderPlayer(const Uint8 &id) {
                this->playerFrameLarge.setColorMod(this->players.at(id).getColor());
                this->playerFrameSmall.setColorMod(this->players.at(id).getColor());
                this->playerButtonLarge.setColorMod(this->players.at(id).getColor());
                this->playerButtonMedium.setColorMod(this->players.at(id).getColor());
                this->playerButtonSmall.setColorMod(this->players.at(id).getColor());
            
                const Uint16 x = id == 0 || id == 3 ? 28 : 1500, y = id == 0 || id == 1 ? 34 : 557;
                const bool largeLayout = (id == 0 && this->players.size() < 4) || (id == 1 && this->players.size() < 3);
                if (largeLayout) {
                    this->window.renderModdedTexture(this->playerFrameLarge, {x, y, this->playerFrameLarge.getFrame().w, this->playerFrameLarge.getFrame().h});
                } else {
                    this->window.renderModdedTexture(this->playerFrameSmall, {x, y, this->playerFrameSmall.getFrame().w, this->playerFrameSmall.getFrame().h});
                }

                this->window.renderText(this->playerTitleFont, (u"Player " + btils::to_u16string<Uint16>(id + 1) + u" - " + this->players.at(id).getName()).c_str(), {x + 13, y + 15}, 0, this->players.at(id).getColor());

                if (this->players.at(id).remainingHexominoes() == 0 && this->players.at(id).remainingHeptominoes() == 0) {
                    this->window.renderModdedTexture(this->playerButtonLarge, {x + 13, y + 53, this->playerButtonLarge.getFrame().w, this->playerButtonLarge.getFrame().h});
                    this->window.renderText(this->playerButtonFont, (btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingBasePolyominoes(), 3, 0)) + u"/" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->baseSets * blokus::COUNT_BASE, 3, 0)) + u" Pieces | " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remaingingTiles_Base(), 3, 0)) + u"/" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->baseSets * blokus::TILES_BASE, 3, 0)) + u" Tiles").c_str(), {x + 16, y + 67}, 0, this->players.at(id).getColor());
                } else if (this->players.at(id).remainingHexominoes() > 0 && this->players.at(id).remainingHeptominoes() == 0) {
                    this->window.renderModdedTexture(this->playerButtonMedium, {x + 13, y + 53, this->playerButtonMedium.getFrame().w, this->playerButtonMedium.getFrame().h});
                    this->window.renderModdedTexture(this->playerButtonMedium, {x + 13 + this->playerButtonMedium.getFrame().w + 6, y + 53, this->playerButtonMedium.getFrame().w, this->playerButtonMedium.getFrame().h});
                    this->window.renderText(this->playerButtonFont, (u"Base: " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingBasePolyominoes(), 3, 0)) + u"|" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remaingingTiles_Base(), 3, 0))).c_str(), {x + 19, y + 67}, 0, this->players.at(id).getColor());
                    this->window.renderText(this->playerButtonFont, (u"Hex:  " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingHexominoes(), 3, 0)) + u"|" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remaingingTiles_Hex(), 3, 0))).c_str(), {x + 205, y + 67}, 0, this->players.at(id).getColor());
                } else if (this->players.at(id).remainingHexominoes() == 0 && this->players.at(id).remainingHeptominoes() > 0) {
                    this->window.renderModdedTexture(this->playerButtonMedium, {x + 13, y + 53, this->playerButtonMedium.getFrame().w, this->playerButtonMedium.getFrame().h});
                    this->window.renderModdedTexture(this->playerButtonMedium, {x + 13 + this->playerButtonMedium.getFrame().w + 6, y + 53, this->playerButtonMedium.getFrame().w, this->playerButtonMedium.getFrame().h});
                    this->window.renderText(this->playerButtonFont, (u"Base: " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingBasePolyominoes(), 3, 0)) + u"|" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remaingingTiles_Base(), 3, 0))).c_str(), {x + 19, y + 67}, 0, this->players.at(id).getColor());
                    this->window.renderText(this->playerButtonFont, (u"Hept: " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingHeptominoes(), 3, 0)) + u"|" + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remaingingTiles_Hept(), 3, 0))).c_str(), {x + 205, y + 67}, 0, this->players.at(id).getColor());
                } else if (this->players.at(id).remainingHexominoes() > 0 && this->players.at(id).remainingHeptominoes() > 0) {
                    this->window.renderModdedTexture(this->playerButtonSmall, {x + 13, y + 53, this->playerButtonSmall.getFrame().w, this->playerButtonSmall.getFrame().h});
                    this->window.renderModdedTexture(this->playerButtonSmall, {x + 13 + this->playerButtonSmall.getFrame().w + 6, y + 53, this->playerButtonSmall.getFrame().w, this->playerButtonSmall.getFrame().h});
                    this->window.renderModdedTexture(this->playerButtonSmall, {x + 13 + (this->playerButtonSmall.getFrame().w + 6) * 2, y + 53, this->playerButtonSmall.getFrame().w, this->playerButtonSmall.getFrame().h});
                
                    this->window.renderText(this->playerButtonFont, (u"Base: " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingBasePolyominoes(), 3, 0))).c_str(), {x + 18, y + 67}, 0, this->players.at(id).getColor());
                    this->window.renderText(this->playerButtonFont, (u"Hex:  " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingHexominoes(), 3, 0))).c_str(), {x + 142, y + 67}, 0, this->players.at(id).getColor());
                    this->window.renderText(this->playerButtonFont, (u"Hept: " + btils::to_u16string(btils::tstr_AddZeros<Uint16>(this->players.at(id).remainingHeptominoes(), 3, 0))).c_str(), {x + 266, y + 67}, 0, this->players.at(id).getColor());
                }
            }
            void updateBoard() {
                // this->window.initTextureCanvas(this->board.size() * 64, this->board.size() * 64);
                // this->window.setRenderTarget(RENDERTARGET_TEXTURE);
                this->window.clear();

                for (Uint8 i = 0; i < this->board.size(); i++) {
                    for (Uint8 j = 0; j < this->board.size(); j++) {
                        if (this->board.at(i).at(j) == 0) {
                            this->window.renderBasicTexture(this->emptyCell, {j * this->tile.getFrame().w, i * this->tile.getFrame().h, this->tile.getFrame().w, this->tile.getFrame().h});
                        } else {
                            this->tile.setColorMod(this->players.at(this->board.at(i).at(j) - 1).getColor());
                            this->window.renderModdedTexture(this->tile, {j * this->tile.getFrame().w, i * this->tile.getFrame().h, this->tile.getFrame().w, this->tile.getFrame().h});
                        }
                    }
                }
                this->window.present();

                // this->boardTexture.setTexture(this->window.getTextureCanvas());
                // this->window.setRenderTarget(RENDERTARGET_WINDOW);
                this->window.clear();
            }
            void renderBoard() {
                this->window.renderBasicTexture(this->boardFrameBase, {454, 34, this->boardFrameBase.getFrame().w, this->boardFrameBase.getFrame().h});
                for (Uint8 i = 0; i < this->players.size(); i++) {
                    this->boardFrameCorner.setColorMod(this->players.at(i).getColor());
                    this->window.renderModdedTexture(this->boardFrameCorner, {454 + (i > 0 && i < 3 ? this->boardFrameCorner.getFrame().w : 0), 34 + (i > 1 ? this->boardFrameCorner.getFrame().h : 0), this->boardFrameCorner.getFrame().w, this->boardFrameCorner.getFrame().h}, i * -90, {this->boardFrameCorner.getFrame().w / 2, this->boardFrameCorner.getFrame().h / 2}, SDL_FLIP_NONE);
                }

                // this->window.renderBasicTexture(this->boardFrame, {454, 34, this->boardFrame.getFrame().w, this->boardFrame.getFrame().h});
                this->window.renderBasicTexture(this->boardTexture, {480, 60, 960, 960});
            }
            void render() {
                blokus::game::renderBoard();
                for (Uint8 i = 0; i < this->players.size(); i++) {
                    blokus::game::renderPlayer(i);
                }
            }

        public:
            game(const Uint8 &boardSize = 20, const Uint8 &baseSets = 1, const Uint8 &hexSets = 0, const Uint8 &heptSets = 0, const Uint8 &playerCount = 4) : bengine::loop("Blokus", 1920, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_FULLSCREEN) {               
                // bengine::loop has a default window size of 1280x1080, this can cause scaling issues if not dealt with
                this->window.setBaseWidth(this->window.getWidth());
                this->window.setBaseHeight(this->window.getHeight());

                // Amount of players for the game; processed to be valid
                const Uint8 players = playerCount < 1 ? 1 : (playerCount > 4 ? 4 : playerCount);
                // Size of the board for the game; processed to be valid
                const Uint8 size = boardSize < 20 ? 20 : (boardSize > 100 ? 100 : boardSize);

                // Player setup, stuff after the for loop shall be removed later
                for (Uint8 i = 0; i < players; i++) {
                    this->players.emplace_back(blokus::player(baseSets, hexSets, heptSets));
                }
                this->players[0].setColor({255, 0, 0, 255});
                this->players[0].setName(u"Bearslay");
                this->players[1].setColor({0, 255, 0, 255});
                this->players[1].setName(u"Barfunkel");
                if (this->players.size() > 2) {
                    this->players[2].setColor({0, 0, 255, 255});
                    this->players[2].setName(u"Belay");
                }
                if (this->players.size() > 3) {
                    this->players[3].setColor({255, 255, 0, 255});
                    this->players[3].setName(u"Charlie");
                }

                // Getting the frames set up for the textures related to the board
                this->boardTexture.setFrame({0, 0, size * this->tile.getFrame().w, size * this->tile.getFrame().h});
                this->boardFrame.setFrame({0, 0, this->boardFrameBase.getFrame().w, this->boardFrameBase.getFrame().h});

                // Grid setup; includes initializing the board itself as well as the grid initial texture (function not used to save a tiny bit of performance b/c why not lol)
                // this->window.initTextureCanvas(size * this->tile.getFrame().w, size * this->tile.getFrame().h);
                // this->window.setRenderTarget(RENDERTARGET_TEXTURE);
                this->window.clear();

                for (Uint8 i = 0; i < size; i++) {
                    this->board.emplace_back();
                    for (Uint8 j = 0; j < size; j++) {
                        this->board[i].emplace_back(0);
                        this->window.renderBasicTexture(this->emptyCell, {j * this->tile.getFrame().w, i * this->tile.getFrame().h, this->tile.getFrame().w, this->tile.getFrame().h});
                    }
                }
                this->window.present();
                // this->boardTexture.setTexture(this->window.getTextureCanvas());
                
                // Board frame setup; texture should not change after this bit
                // this->window.initTextureCanvas(this->boardFrameBase.getFrame().w, this->boardFrameBase.getFrame().h);
                // this->window.clear();

                // this->window.present();
                // this->boardFrame.setTexture(this->window.getTextureCanvas());
                // this->window.setRenderTarget(RENDERTARGET_WINDOW);
                this->window.clear();
            }
            ~game() {
                TTF_CloseFont(this->playerTitleFont);
                TTF_CloseFont(this->playerButtonFont);
            }
    };
}

#endif /* BLOKUS_hpp */
