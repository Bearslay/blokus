#ifndef BLOKUS_PLAYER_hpp
#define BLOKUS_PLAYER_hpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "btils.hpp"
#include "bengine.hpp"

#include "blokus_polyominoes.hpp"
#include "blokus_piece.hpp"
#include "blokus_game.hpp"

namespace blokus {
    class piece;
    class game;

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
}

#endif /* BLOKUS_PLAYER_hpp */
