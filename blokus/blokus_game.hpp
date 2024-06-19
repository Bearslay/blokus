#ifndef BLOKUS_GAME_hpp
#define BLOKUS_GAME_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "btils.hpp"
#include "bengine.hpp"

#include "blokus_polyominoes.hpp"
#include "blokus_piece.hpp"
#include "blokus_player.hpp"

namespace blokus {
    class piece;
    class player;

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

#endif /* BLOKUS_GAME_hpp */
